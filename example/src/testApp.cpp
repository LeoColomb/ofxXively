#include "testApp.h"

testApp::testApp()
{
	iCounter = 0;

	out = new ofxXivelyOutput(true);       /// threaded
	out->setApiKey("6f438ce7b9318cad96259a5b6cff964150ce248706bfcddf9850f742922fed8f");
	/// To read a feed you need a xively api key. More info at xively.com
	out->setFeedId(1543);
	out->setVerbose(false);
	out->setMinInterval(5);
	out->output(OFX_XIVELY_EEML, true);    /// forcing update = ignoring min interval

	in = new ofxXivelyInput(false);        /// not threaded
	in->setApiKey("1c7c8101fdaf393b0cb0f326c097eeebb63329d1f912d164bd49d256627657ba");
	/// To update a feed you need to have created it (I think?) otherwise you'll get an 'Unautharized' error
	in->setFeedId(143);
	in->setVerbose(true);
	in->setMinInterval(15);
	in->setDatastreamCount(1);              /// number of streams to update
}

testApp::~testApp()
{
	delete in;
	delete out;
}

//--------------------------------------------------------------
void testApp::setup(){
}

//--------------------------------------------------------------
void testApp::update(){
	/// Values are updated from Xively if min interval has passed since last update
	/// Override this by setting the 'force' argument to true
	/// Feeds can be read as EEML or CSV
	out->output(OFX_XIVELY_CSV, false);

	/// value is set each 'update' but input to xively is done only after min interval has passed
	/// You can control this maually by some timing functionality, or adjusting in->setMinInterval()
	/// Input can only be done as CSV
	in->setValue(0, ofRandom(0,100));
	in->input();

	/// A counter to see if threads are working
	iCounter++;
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(10, 50, 0);
	ofSetColor(255,255,255);
	ofFill();

	char pcText[256];
	sprintf(pcText, "Counter: %d\n", iCounter);
	ofDrawBitmapString(pcText, 20, ofGetHeight()-5);

	/// OUTPUT
	if (out->getLastRequestOk())
		ofSetColor(50, 230, 40);
	else
		ofSetColor(250, 60, 40);

	ofDrawBitmapString("OUTPUT", 20, 20);
	sprintf(pcText, "Feed id: %d\n", out->getFeedId());
	ofDrawBitmapString(pcText, 20, 35);
	sprintf(pcText, "Title: %s\n", out->getTitle().c_str());
	ofDrawBitmapString(pcText, 20, 50);
	sprintf(pcText, "Location name: %s\n", out->getLocation().sName.c_str());
	ofDrawBitmapString(pcText, 20, 65);
	sprintf(pcText, "Location latitude/longitude: %s/%s\n", out->getLocation().sLat.c_str(), out->getLocation().sLon.c_str());
	ofDrawBitmapString(pcText, 20, 80);
	sprintf(pcText, "Status: %s\n", out->getStatus().c_str());
	ofDrawBitmapString(pcText, 20, 95);
	sprintf(pcText, "Description: %s\n", out->getDescription().substr(0,256).c_str());
	ofDrawBitmapString(pcText, 20, 110);

	sprintf(pcText, "Last response time: %.0f\n", out->getLastResponseTime());
	ofDrawBitmapString(pcText, 20, 155);
	sprintf(pcText, "Datastreams: %d\n", out->getDatastreamCount());
	ofDrawBitmapString(pcText, 20, 170);
	for (int i = 0; i < out->getDatastreamCount(); ++i)
	{
		sprintf(pcText, "Value %d: %f\n", i, out->getValue(i));
		ofDrawBitmapString(pcText, 20, 185+15*i);
	}

	/// INPUT
	if (in->getLastRequestOk())
		ofSetColor(50, 230, 40);
	else
		ofSetColor(250, 60, 40);

	ofDrawBitmapString("INPUT", 20, 300);
	sprintf(pcText, "Datastreams: %d\n", in->getDatastreamCount());
	ofDrawBitmapString(pcText, 20, 315);
	for (int i = 0; i < in->getDatastreamCount(); ++i)
	{
		sprintf(pcText, "Value %d: %f\n", i, in->getValue(i));
		ofDrawBitmapString(pcText, 20, 330+15*i);
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	/// Press 'e' to refresh feed info compleately as EEML
	if (key == 'e')
		out->output(OFX_XIVELY_EEML, true);
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
}
