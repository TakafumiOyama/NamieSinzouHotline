#include "ofApp.h"

float *audio_input = NULL;

float *magnitude, *phase, *power;
float *magnitude_average, *magnitude_average_snapshot;
float *circle_phase;
const int buffer_size = 1024, fft_size = 512;

fft myfft;

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	ofSetCircleResolution(64);
	ofBackground(0, 0, 0);
	ofSetFrameRate(60);

	//Serial Port
	int baud;
	std::cout << "Imput Now Baud-rate (maybe 19200 or 57600): " << std::endl;
	std::cin >> baud;
	string port;
	std::cout << "Imput PortID: " << std::endl;
	std::cin >> port;
	string com = "COM";
	serial.setup(com + port, baud);
	serial.startContinuousRead(false);
	ofAddListener(serial.NEW_MESSAGE, this, &ofApp::onNewMessage);
	message = "";
	remember = false;

	//FFT
	// もろもろの配列を生成&初期化
	audio_input = new float[buffer_size];
	magnitude = new float[fft_size];
	phase = new float[fft_size];
	power = new float[fft_size];

	magnitude_average = new float[fft_size];
	magnitude_average_snapshot = new float[fft_size];

	circle_phase = new float[fft_size];

	for (int i = 0; i < fft_size; i++)
	{
		magnitude[i] = 0;
		phase[i] = 0;
		power[i] = 0;
		magnitude_average_snapshot[i] = 0;
		magnitude_average[i] = 0;
	}

	// オーディオインプットを初期化
	ofSoundStreamSetup(0, 1, this, 44100, buffer_size, 4);

	//GUI
	/*int initBandsNum = 7;
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
	}*/
	counter = 0;
}

void ofApp::onNewMessage(string & message)
{
	cout << "onNewMessage, message: " << message << "\n";
}

//--------------------------------------------------------------
void ofApp::update() {
	// FFT振幅の平均値を計算
	for (int i = 0; i < fft_size; i++)
	{
		float x = 0.025;
		magnitude_average[i] = (magnitude[i] * x) + (magnitude_average[i] * (1 - x));
	}

	for (int i = 0; i < fft_size; i++)
	{
		//  環境ノイズをキャンセル
		float v = fabs(magnitude_average[i] - magnitude_average_snapshot[i]);

		//v *= 0.1;

		// 円の回転角(0.0 ~ 1.0)を更新
		circle_phase[i] = v;
		//while (circle_phase[i] > 1) circle_phase[i] -= 1;
	}

	//Get FFT Value
	fftValueDB1[counter] = circle_phase[10];
	fftValueDB2[counter] = circle_phase[110];
	fftValueDB3[counter] = circle_phase[410];

	float sum[3] = { 0 };
	float flatFftValue[3] = { 0 };
	for (int i = 0; i < 10; i++) {
		sum[0] += fftValueDB1[i];
		sum[1] += fftValueDB2[i];
		sum[2] += fftValueDB3[i];
	}
	for (int i = 0; i < 3; i++) {
		flatFftValue[i] = sum[i] / 10;
	}
	//backColor = sum / 10;
	volumeLow = ofMap(flatFftValue[0], 0, 1, 0, 255);
	volumeMid = ofMap(flatFftValue[1], 0, 1, 0, 255);
	volumeHigh = ofMap(flatFftValue[2], 0, 1, 0, 255);
	//flatVolume = powf(volume, 0.5);
	serial.writeString("H");
	serial.writeByte(volumeLow);
	serial.writeByte(volumeMid);
	serial.writeByte(volumeHigh);

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
	if (counter > 10) {
		counter = 0;
	}

	// FFT振幅の平均値を計算
	for (int i = 0; i < fft_size; i++)
	{
		float x = 0.025;
		magnitude_average[i] = (magnitude[i] * x) + (magnitude_average[i] * (1 - x));
	}

	for (int i = 0; i < fft_size; i++)
	{
		//  環境ノイズをキャンセル
		float v = fabs(magnitude_average[i] - magnitude_average_snapshot[i]);

		v *= 0.1;

		// 円の回転角(0.0 ~ 1.0)を更新
		circle_phase[i] += v;
		while (circle_phase[i] > 1) circle_phase[i] -= 1;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	//背景色
	/*float backgroundColor = backColor * 10 + colorFactor;
	ofBackground(abs(sin(ofGetElapsedTimef()/10)*77), backgroundColor, backgroundColor);*/

	//float width = float(ofGetWidth()) / float(nBandsToGet) / 2.0f; //円の場所を設定
	//															   //周波数ごとに円を描画
	//for (int i = 0; i < nBandsToGet; i++) {
	//	/*int b = float(255) / float(nBandsToGet) *i;
	//	int g = colorFactor;
	//	int r = 255 - b;*/
	//	float flatFftValue = (fftValueDB1[0] + fftValueDB1[1] + fftValueDB1[2] + fftValueDB1[3] + fftValueDB1[4]) / 5;

	//	float r = (fftValueDB1[0] + fftValueDB1[1] + fftValueDB1[2] + fftValueDB1[3] + fftValueDB1[4]) / 5 * 255; //FFTの値を平滑化してから0-1を0-255にマッピング
	//	float g = (fftValueDB2[0] + fftValueDB2[1] + fftValueDB2[2] + fftValueDB2[3] + fftValueDB2[4]) / 5 * 255;
	//	float b = (fftValueDB3[0] + fftValueDB3[1] + fftValueDB3[2] + fftValueDB3[3] + fftValueDB3[4]) / 5 * 255;
	//	ofSetColor(r, g, b);

	//	//ofCircle(circlePos[i], flatFftValue * 100);
	//	ofCircle(ofGetWidth() / 2 + width * 0, ofGetHeight() / 2, powf(flatFftValue, 0.5) * 100);
	//	ofCircle(ofGetWidth() / 2 + width * i, ofGetHeight() / 2, powf(fft[i], 0.5) * 300);
	//	ofCircle(ofGetWidth() / 2 - width * i, ofGetHeight() / 2, powf(fft[i], 0.5) * 300);
	//}

	ofCircle(ofGetWidth() / 5, ofGetHeight() / 2, volumeLow);
	ofCircle(ofGetWidth() / 2, ofGetHeight() / 2, volumeMid);
	ofCircle(ofGetWidth() / 5 * 4, ofGetHeight() / 2, volumeHigh);
	//GUI
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// スペースキーを押したときにFFT振幅平均値のスナップショットをコピー
	if (key == ' ')
	{
		memcpy(magnitude_average_snapshot, magnitude_average, sizeof(float) * fft_size);
	}
}

void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

void ofApp::audioReceived(float * input, int bufferSize, int nChannels) {
	// 入力された音声シグナルを audio_input にコピー
	memcpy(audio_input, input, sizeof(float) * bufferSize);

	float avg_power = 0.0f;

	// アドオンのFFTクラスを使ってスペクトルを解析、 magnitude が欲しかったFFT振幅の値になる。
	myfft.powerSpectrum(0, (int)fft_size, audio_input, buffer_size,
		magnitude, phase, power, &avg_power);

	// 結果の値の大小が激しかったので平方根をとった
	for (int i = 0; i < fft_size; i++)
	{
		magnitude[i] = powf(magnitude[i], 0.5);
	}
}

