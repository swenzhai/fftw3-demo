#ifndef SOUND_H
#define SOUND_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <AL/al.h>
#include <AL/alc.h>

class Sound {
public:

	Sound(char* filename) {
		alGenSources(1, &(this->source));
		alSourcef(this->source, AL_PITCH, 1);
		alSourcef(this->source, AL_GAIN, 1);
		alSource3f(this->source, AL_POSITION, 0, 0, 0);
		alSource3f(this->source, AL_VELOCITY, 0, 0, 0);
		alSourcei(this->source, AL_LOOPING, AL_FALSE);

		std::fstream file(filename, std::fstream::in | std::fstream::binary);
		char xbuffer[5];
		xbuffer[4] = '\0';

		file.read(xbuffer, 4);
		if (strcmp(xbuffer, "RIFF") != 0) {
			alDeleteSources(1, &(this->source));
			std::cout << "Not a wav file!" << std::endl;
			return;
		}

		file.read(xbuffer, 4);

		file.read(xbuffer, 4);
		if (strcmp(xbuffer, "WAVE") != 0) {
			alDeleteSources(1, &(this->source));
			std::cout << "Not a wav file!" << std::endl;
			return;
		}

		file.read(xbuffer, 4);
		if (strcmp(xbuffer, "fmt ") != 0) {
			alDeleteSources(1, &(this->source));
			std::cout << "Invalid WAV file!" << std::endl;
			return;
		}

		file.read(xbuffer, 4);

		file.read(xbuffer, 2);
		this->audioFormat = BytesToShort(xbuffer);
		if (this->audioFormat != 1) {
			alDeleteSources(1, &(this->source));
			std::cout << "File not in PCM! Cannot read it!" << std::endl;
			return;
		}

		file.read(xbuffer, 2);
		this->channels = BytesToShort(xbuffer);

		file.read(xbuffer, 4);
		this->sampleRate = BytesToInt(xbuffer);

		file.read(xbuffer, 4);
		this->byteRate = BytesToInt(xbuffer);

		file.read(xbuffer, 2);

		file.read(xbuffer, 2);
		this->bitsPerSample = BytesToShort(xbuffer);
		if (this->bitsPerSample != 8 && this->bitsPerSample != 16) {
			alDeleteSources(1, &(this->source));
			std::cout << "File is not written as 8-bit or 16-bit!" << std::endl;
			return;
		}

		file.read(xbuffer, 4);
		if (strcmp(xbuffer, "data") != 0) {
			alDeleteSources(1, &(this->source));
			std::cout << "Invalid WAV file!" << std::endl;
			return;
		}

		file.read(xbuffer, 4);
		this->audioSize = BytesToInt(xbuffer);

		this->audioData = (unsigned char*)malloc(audioSize);
		file.read(reinterpret_cast<char*>(audioData), audioSize);

		file.close();

		if (channels == 1) {
			if (this->bitsPerSample == 8) this->format = AL_FORMAT_MONO8;
			else this->format = AL_FORMAT_MONO16;
		}
		else {
			if (this->bitsPerSample == 8) this->format = AL_FORMAT_STEREO8;
			else this->format = AL_FORMAT_STEREO16;
		}

		alGenBuffers(1, &(this->buffer));
		alBufferData(this->buffer, this->format, this->audioData, this->audioSize, this->sampleRate);
		alSourcei(this->source, AL_BUFFER, this->buffer);
	}

	void dealloc() {
		free(this->audioData);
		alDeleteSources(1, &(this->source));
		alDeleteBuffers(1, &(this->buffer));
	}

	void play() {
		alSourcePlay(this->source);
		std::cout << "playing" << std::endl;
	}
	void pause() {
		alSourcePause(this->source);
	}

	void setPitch(float val) {
		alSourcef(source, AL_PITCH, val);
	}

	void setGain(float val) {
		alSourcef(source, AL_GAIN, val);
	}

	void setPosition(float* position) {
		this->setPosition(position[0], position[1], position[2]);
	}

	void setPosition(float x, float y, float z) {
		alSource3f(source, AL_POSITION, x, y, z);
	}

	void setVelocity(float* velocity) {
		this->setPosition(velocity[0], velocity[1], velocity[2]);
	}

	void setVelocity(float x, float y, float z) {
		alSource3f(source, AL_VELOCITY, x, y, z);
	}

	void isLooping(bool state) {
		if (state) alSourcei(this->source, AL_LOOPING, AL_TRUE);
		else alSourcei(this->source, AL_LOOPING, AL_FALSE);
	}

	unsigned short getAudioFormat(){ return this->audioFormat; }
	unsigned short getChannels() { return this->channels; }
	unsigned int getSampleRate() { return this->sampleRate; }
	unsigned int getByteRate() { return this->byteRate; }
	unsigned int getBitsPerSample() { return this->bitsPerSample; }
	unsigned int getAudioSize() { return this->audioSize; }
	unsigned char* getAudioData() { return this->audioData; }

private:
	ALuint source, buffer;
	ALenum format;
	unsigned short audioFormat, channels, blockAlign;
	unsigned int sampleRate, byteRate, bitsPerSample, audioSize;
	unsigned char* audioData;

	unsigned short BytesToShort(char* buffer) {
		unsigned short out = 0, byte0, byte1;

		byte0 = (unsigned short)(buffer[0]) & 0x00ff;
		byte1 = (unsigned short)(buffer[1] << 8) & 0xff00;

		out += byte0 | byte1;
		return out;
	}

	unsigned int BytesToInt(char* buffer) {
		unsigned int out = 0, byte0, byte1, byte2, byte3;

		byte0 = (unsigned int)(buffer[0]) & 0x000000ff;
		byte1 = (unsigned int)(buffer[1] << 8) & 0x0000ff00;
		byte2 = (unsigned int)(buffer[2] << 16) & 0x00ff0000;
		byte3 = (unsigned int)(buffer[3] << 24) & 0xff000000;

		out += byte0 | byte1 | byte2 | byte3;
		return out;
	}
};

#endif