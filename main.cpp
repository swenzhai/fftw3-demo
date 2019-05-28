#include <iostream>
#include <fftw3.h>
#include <math.h>

int main() {
    fftw_complex *in, *out;
    fftw_plan p;
    int N = 8;
    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*N);
    for (int i = 0;i<N;i++){
        in[i][0]=sin(i);
        in[i][1]=cos(i);
    }
    p = fftw_plan_dft_1d(N,in,out,FFTW_FORWARD,FFTW_ESTIMATE);

    fftw_execute(p);

    for(int i=0;i<N;i++)/*OUTPUT*/
    {
        printf("%f,%fi\n",in[i][0],in[i][1]);
    }
    printf("\n");
    for(int i=0;i<N;i++)/*OUTPUT*/
    {
        printf("%f,%fi\n",out[i][0],out[i][1]);
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    std::cout << "Hello, World!" << std::endl;
    return 0;
}