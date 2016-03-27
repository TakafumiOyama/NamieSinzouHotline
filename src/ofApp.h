#pragma once

#include "ofMain.h"
#include "ofxSimpleSerial.h"
#include "fft.h"
#include "ofxGui.h"

#define BUFFER_SIZE 256
#define NUM_WINDOWS 80

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void windowResized(int w, int h);
	void keyPressed(int key);
	void keyReleased(int key);
	void onNewMessage(string & message);

	ofVec2f circlePos[1024];
	float backColor;
	//FFT
	ofSoundPlayer player;
	float *fft;//FFT‰ğÍŒ‹‰Ê
	float position;
	float volumeLow, volumeMid, volumeHigh;
	float flatVolume;
	ofxIntSlider nBandsToGet;//ü”g”‚Ì‰ğ‘œ“x

	float fftValueDB1[10] = { 0 };
	float fftValueDB2[10] = { 0 };
	float fftValueDB3[10] = { 0 };
	int counter;
	//Serial
	ofxSimpleSerial serial;
	string message;
	unsigned char mess;
	bool remember;
	//GUI
	ofxPanel gui;
	ofxFloatSlider colorFactor;

	void audioReceived(float * input, int bufferSize, int nChannels);
};
