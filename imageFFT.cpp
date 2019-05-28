/*
 * this demo use fftw3 and opencv to show 2D fft result
 */
#include <iostream>
#include <fftw3.h>
#include <math.h>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <stdio.h>
using namespace cv;
using namespace std;

int main() {
    /* load original image */
    Mat img_src = imread("/media/swen/D_Disk/1.jpg", IMREAD_GRAYSCALE);

    /* create new image for FFT & IFFT result */
    Mat img_fft = img_src.clone();
    Mat img_ifft = img_src.clone();
    cout<<img_src.size;

    /* get image properties */
    int width = img_src.cols;
    int height = img_src.rows;
    int step = width;
    uchar *img_src_data = (uchar *) img_src.data;
    uchar *img_fft_data = (uchar *) img_fft.data;
    uchar *img_ifft_data = (uchar *) img_ifft.data;

    /* initialize arrays for fftw operations */
    fftw_complex *data_in = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * width * height);
    fftw_complex *fft = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * width * height);
    fftw_complex *ifft = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * width * height);

    /* create plans */
    fftw_plan plan_f = fftw_plan_dft_2d(height, width, data_in, fft, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan plan_b = fftw_plan_dft_2d(height, width, fft, ifft, FFTW_BACKWARD, FFTW_ESTIMATE);

    int i, j, k;
    /* load img_src's data to fftw input */
    for (i = 0, k = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            // method 1: 输入数据乘以(-1)^（i+j），即可中心化
            //data_in[k][0] = (double) img_src_data[i * step + j];
            data_in[k][0] = pow(-1, i + j) * (double) img_src_data[i * step + j];
            data_in[k][1] = 0.0;
            k++;
        }
    }

    /* perform FFT */
    fftw_execute(plan_f);

    /* perform IFFT */
    fftw_execute(plan_b);

    /* normalize FFT result */
    double maxx = 0.0, minn = 10000000000.0;
    for (i = 0; i < width * height; ++i) {
        //fft[i][0] = log(sqrt(fft[i][0] * fft[i][0] + fft[i][1] * fft[i][1]));
        fft[i][0] = log(sqrt(fft[i][0] * fft[i][0] + fft[i][1] * fft[i][1])+1); //plus 1 to avoid log(0)
        maxx = fft[i][0] > maxx ? fft[i][0] : maxx;
        minn = fft[i][0] < minn ? fft[i][0] : minn;
    }

    for (i = 0; i < width * height; ++i) {
        fft[i][0] = 255.0 * (fft[i][0] - minn) / (maxx - minn);
    }

    /* copy FFT result to img_fft's data */
    int i0, j0;
    for (i = 0, k = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            if (i < height / 2)
                i0 = i + height / 2;
            else
                i0 = i - height / 2;
            if (j < width / 2)
                j0 = j + width / 2;   // method 2
            else
                j0 = j - width / 2;

            img_fft_data[i * step + j] = (uchar) fft[/*k++*/i0 * width + j0][0];
        }
    }

    /* normalize IFFT result */
    for (i = 0; i < width * height; ++i) {
        ifft[i][0] /= width * height;
    }

    /* copy IFFT result to img_ifft's data */
    for (i = 0, k = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            img_ifft_data[i * step + j] = (uchar) ifft[k++][0];
        }
    }

    /* display images */
    cvNamedWindow("original_image", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("FFT", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("IFFT", CV_WINDOW_AUTOSIZE);

    imshow("original_image", img_src);
    imshow("FFT", img_fft);
    imshow("IFFT", img_ifft);

    cvWaitKey(0);

    /* free memory */
    cvDestroyWindow("original_image");
    cvDestroyWindow("FFT");
    cvDestroyWindow("IFFT");

    fftw_destroy_plan(plan_f);
    fftw_destroy_plan(plan_b);
    fftw_free(data_in);
    fftw_free(fft);
    fftw_free(ifft);
}