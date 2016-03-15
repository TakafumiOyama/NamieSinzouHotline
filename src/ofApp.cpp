#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofSetCircleResolution(64);
	ofBackground(0, 0, 0);
	ofSetFrameRate(60);

	//Serial Port
	serial.setup("COM8", 57600);
	serial.startContinuousRead(false);
	ofAddListener(serial.NEW_MESSAGE, this, &ofApp::onNewMessage);
	message = "";
	remember = false;

	//FFT
	player.loadSound("cat_out.mp3");
	//player.loadSound("voice1.mp3");
	player.setLoop(true);
	player.play();

	//GUI
	int initBandsNum = 7;
	int minBandsNum = 3;
	int maxBandsNum = 15;
	float initColorFactor = 10;
	float minColorFactor = 0;
	float maxColorFactor = 100;
	gui.setup();
	gui.add(nBandsToGet.setup("Circle Num", initBandsNum, minBandsNum, maxBandsNum));
	gui.add(colorFactor.setup("Color", initColorFactor, minColorFactor, maxColorFactor));
	
	for (int i = 0; i < 1024; i++) {
		circlePos[i].x = ofRandom(0, ofGetWidth());
		circlePos[i].y = ofRandom(0, ofGetHeight());
	}
	counter = 0;
}

void ofApp::onNewMessage(string & message)
{
	cout << "onNewMessage, message: " << message << "\n";
}

//--------------------------------------------------------------
void ofApp::update() {
	ofSoundUpdate();
	//Get FFT Value
	fft = ofSoundGetSpectrum(nBandsToGet);
	
	fftValueDB1[counter] = powf(fft[0], 0.5);
	fftValueDB2[counter] = powf(fft[1], 0.5);
	fftValueDB3[counter] = powf(fft[2], 0.5);

	float sum = 0;
	for (int i = 0; i < 5; i++) {
		sum += fftValueDB1[i];
	}
	float flatFftValue = sum / 5;
	backColor = sum / 5;
	volume = ofMap(flatFftValue, 0, 1, 0, 255);
	flatVolume = powf(volume, 0.5);
	serial.writeByte(volume);

	/*char sendValue = (char)lowValue;
	mess = (unsigned char)volume;
	message += sendValue;*/

	//update messsage
	//if (message != "" && remember == false)
	//{
	//	//cout << "sending message: " << message << "\n";
	//	//serial.writeString(message);
	//	serial.writeByte(mess);
	//	message = "";
	//}

	for (int i = 0; i < 1024; i++) {
		circlePos[i].x += 1;
		circlePos[i].y += 1;
		/*int x = ofRandom(0, 4);
		switch (x) {
		case(0) :
			circlePos[i].x += 5;
			circlePos[i].y += 5;
			break;
		case(1) :
			circlePos[i].x -= 5;
			circlePos[i].y += 5;
			break;
		case(2) :
			circlePos[i].x += 5;
			circlePos[i].y -= 5;
			break;
		case(3) :
			circlePos[i].x -= 5;
			circlePos[i].y -= 5;
			break;
		}*/
		if (circlePos[i].x > ofGetWidth()) {
			circlePos[i].x -= ofGetWidth();
		}
		if (circlePos[i].x < 0) {
			circlePos[i].x += ofGetWidth();
		}
		if (circlePos[i].y > ofGetHeight()) {
			circlePos[i].y -= ofGetHeight();
		}
		if (circlePos[i].y < 0) {
			circlePos[i].y += ofGetHeight();
		}
	}
	counter++;
	if (counter > 5) {
		counter = 0;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	//îwåiêF
	float backgroundColor = backColor * 10 + colorFactor;
	ofBackground(abs(sin(ofGetElapsedTimef()/10)*77), backgroundColor, backgroundColor);

	float width = float(ofGetWidth()) / float(nBandsToGet) / 2.0f; //â~ÇÃèÍèäÇê›íË
																   //é¸îgêîÇ≤Ç∆Ç…â~Çï`âÊ
	for (int i = 0; i < nBandsToGet; i++) {
		/*int b = float(255) / float(nBandsToGet) *i;
		int g = colorFactor;
		int r = 255 - b;*/
		float flatFftValue = (fftValueDB1[0] + fftValueDB1[1] + fftValueDB1[2] + fftValueDB1[3] + fftValueDB1[4]) / 5;

		float r = (fftValueDB1[0] + fftValueDB1[1] + fftValueDB1[2] + fftValueDB1[3] + fftValueDB1[4]) / 5 * 255; //FFTÇÃílÇïΩääâªÇµÇƒÇ©ÇÁ0-1Ç0-255Ç…É}ÉbÉsÉìÉO
		float g = (fftValueDB2[0] + fftValueDB2[1] + fftValueDB2[2] + fftValueDB2[3] + fftValueDB2[4]) / 5 * 255;
		float b = (fftValueDB3[0] + fftValueDB3[1] + fftValueDB3[2] + fftValueDB3[3] + fftValueDB3[4]) / 5 * 255;
		ofSetColor(r, g, b);

		//ofCircle(circlePos[i], flatFftValue * 100);
		ofCircle(ofGetWidth() / 2 + width * 0, ofGetHeight() / 2, powf(flatFftValue, 0.5) * 100);
		ofCircle(ofGetWidth() / 2 + width * i, ofGetHeight() / 2, powf(fft[i], 0.5) * 300);
		ofCircle(ofGetWidth() / 2 - width * i, ofGetHeight() / 2, powf(fft[i], 0.5) * 300);
	}
	//GUI
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 32: //space
		remember = true;
		break;
	}
}

void ofApp::keyReleased(int key) {
	switch (key) {
	case 32: //space
		remember = false;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

