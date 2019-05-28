/*
 * fftw demo using openal to deal with audio file
 */
#include <iostream>
#include <stdio.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <fftw3.h>

#include "Sound.h"


//takes all 8/16-bit mono/stereo audio and stores it in a float[] (channels are averaged for stereo)
void audiotoFloat(unsigned char* in, float* out, int inSize, int outSize, int sampleSize, int channels);
void audiotoDouble(unsigned char* in, double* out, int inSize, int outSize, int sampleSize, int channels);

int main() {
	ALCdevice* device = alcOpenDevice(NULL);
	ALCcontext* context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);

	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);

	Sound music("../test.wav");


	//number of samples (or LR frames) to collect
	const int sampleNum = 128;

	//getting size of a sample's channel and number of channels
	const int sampleSize = music.getBitsPerSample() / 8;
	const int channels = music.getChannels();

	//number of bytes to pull from audio (from beginning)
	const int dataSize = (music.getChannels() == 2) ? 2*sampleSize*sampleNum : sampleSize*sampleNum;

	//will hold data
	unsigned char* data = (unsigned char*)malloc(dataSize);

	for (int i = 0; i < dataSize; i++) {
		data[i] = music.getAudioData()[i];
	}

	double *samples;
	fftw_complex *out;
	fftw_plan p;

	samples = (double*)fftw_malloc(sizeof(double) * sampleNum);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * sampleNum);

	p = fftw_plan_dft_r2c_1d(sampleNum, samples, out, FFTW_ESTIMATE);

	//storing audio data in "data"
	//float* samplesf = (float*)fftw_malloc(sizeof(float) * sampleNum);
	//audiotoFloat(data, samplesf, dataSize, sampleNum, sampleSize, channels);
	//for (int i = 0;i<sampleNum;i++){
	//    samples[i]=samplesf[i];
	//}
    audiotoDouble(data, samples, dataSize, sampleNum, sampleSize, channels);

	//executing fft
	fftw_execute(p);

	//printing out frequency coefficients (complex numbers)
	for (int i = 0; i < sampleNum; i++) {
		std::cout << "[" << out[i][0] << ",\t" << out[i][1] << "]" << std::endl;
	}

	//enter char to quit
	fgetc(stdin);

	//deallocating everthing
	free(data);
	fftw_destroy_plan(p);
	fftw_free(samples);
	fftw_free(out);
	music.dealloc();

	alcDestroyContext(context);
	alcCloseDevice(device);

	return 0;
}


//takes all 8/16-bit mono/stereo audio and stores it in a float[] (channels are averaged for stereo)
void audiotoFloat(unsigned char* in, float* out, int inSize, int outSize, int sampleSize, int channels) {
	int inPlace = 0, outPlace = 0;
	while (inPlace < inSize && outPlace < outSize) {
		char bytes[4];
		for (int i = 0; i < 4; i++) bytes[i] = in[inPlace++];

		if (channels == 2) {
			if (sampleSize == 1) {
				float first = ((int)bytes[0] + (int)bytes[1]) / 2.0f;
				float second = ((int)bytes[2] + (int)bytes[3]) / 2.0f;
				out[outPlace++] = first;
				out[outPlace++] = second;
			}
			if (sampleSize == 2) {
				int left = ((int)bytes[0] & 0x00ff) | (((int)bytes[1] << 8) & 0xff00);
				int right = ((int)bytes[2] & 0x00ff) | (((int)bytes[3] << 8) & 0xff00);

				out[outPlace++] = ((long)left + (long)right) / 2.0f;
			}
		}
		else if (channels == 1) {
			if (sampleSize == 1) {
				for (int i = 0; i < 4; i++) out[outPlace++] = bytes[i];
			}
			if (sampleSize == 2) {
				int first = ((int)bytes[0] & 0x00ff) | (((int)bytes[1] << 8) & 0xff00);
				int second = ((int)bytes[2] & 0x00ff) | (((int)bytes[3] << 8) & 0xff00);

				out[outPlace++] = (float)first;
				out[outPlace++] = (float)second;
			}
		}
	}
}
//takes all 8/16-bit mono/stereo audio and stores it in a float[] (channels are averaged for stereo)
void audiotoDouble(unsigned char* in, double* out, int inSize, int outSize, int sampleSize, int channels) {
    int inPlace = 0, outPlace = 0;
    while (inPlace < inSize && outPlace < outSize) {
        char bytes[4];
        for (int i = 0; i < 4; i++) bytes[i] = in[inPlace++];

        if (channels == 2) {
            if (sampleSize == 1) {
                double first = ((int)bytes[0] + (int)bytes[1]) / 2.0f;
                double second = ((int)bytes[2] + (int)bytes[3]) / 2.0f;
                out[outPlace++] = first;
                out[outPlace++] = second;
            }
            if (sampleSize == 2) {
                int left = ((int)bytes[0] & 0x00ff) | (((int)bytes[1] << 8) & 0xff00);
                int right = ((int)bytes[2] & 0x00ff) | (((int)bytes[3] << 8) & 0xff00);

                out[outPlace++] = ((long)left + (long)right) / 2.0f;
            }
        }
        else if (channels == 1) {
            if (sampleSize == 1) {
                for (int i = 0; i < 4; i++) out[outPlace++] = bytes[i];
            }
            if (sampleSize == 2) {
                int first = ((int)bytes[0] & 0x00ff) | (((int)bytes[1] << 8) & 0xff00);
                int second = ((int)bytes[2] & 0x00ff) | (((int)bytes[3] << 8) & 0xff00);

                out[outPlace++] = (double)first;
                out[outPlace++] = (double)second;
            }
        }
    }
}