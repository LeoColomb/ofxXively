#include "ofxXivelyFeed.h"

#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/StreamConverter.h"
#include "Poco/URIStreamOpener.h"

#include "Poco/Exception.h"

#include <fstream>

ofxXivelyFeed::ofxXivelyFeed(bool _bThreaded)
{
	bThreaded = _bThreaded;
	bVerbose = true;

	sApiUrl = "https://api.xively.com/";
	sApiKey = "";
	iFeedId = -1;

	fMinInterval = OFX_XIVELY_MIN_INTERVAL;
	bRequestQueued = false;
	bLastRequestOk = true;
	fLastResponseTime = -1.f;

	if (bThreaded)
		startThread();
}

ofxXivelyFeed::~ofxXivelyFeed()
{
	if (bThreaded)
		stopThread();
}

void
	ofxXivelyFeed::setMinInterval(float fSeconds)
{
	if (fSeconds > OFX_XIVELY_MIN_INTERVAL)
		fMinInterval = fSeconds;
}

void
	ofxXivelyFeed::setApiKey(std::string _sApiKey)
{
	sApiKey = _sApiKey;
}

void
	ofxXivelyFeed::setFeedId(int _iId)
{
	iFeedId = _iId;
}

void
	ofxXivelyFeed::threadedFunction()
{
	if (bVerbose) printf("[XIVELY] Thread started\n");
	while (true)
	{
		// check if new request is available
		if (bRequestQueued)
		{
			if (bVerbose) printf("[XIVELY] new request available\n");

			sendRequest(request);

			bRequestQueued = false;
		}

		// sleep
		ofSleepMillis(1000);
	}
}

void
	ofxXivelyFeed::sendRequest(ofxXivelyRequest request)
{
	try{
		URI uri( request.url.c_str() );
		string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		HTTPClientSession session(uri.getHost(), uri.getPort());
		session.setTimeout(Timespan(request.timeout, 0));

		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		if (request.method == OFX_XIVELY_PUT)
			req.setMethod(HTTPRequest::HTTP_PUT);

		/// headers
		for(unsigned int i=0; i<request.headerIds.size(); i++){
			const std::string name = request.headerIds[i].c_str();
			const std::string val = request.headerValues[i].c_str();
			req.set(name, val);
		}

		req.set("Content-Length", ofToString((int)request.data.length()));

		if (bVerbose) printf("[XIVELY] ------------------------------\n");
		if (bVerbose) printf("[XIVELY] write data request\n");
		session.sendRequest(req) << request.data;

		if (bVerbose) printf("[XIVELY] about to receive a response\n");
		HTTPResponse res;
		istream& rs = session.receiveResponse(res);
		if (bVerbose) printf("[XIVELY] received a session response\n");

		if (bVerbose) printf("[XIVELY] create new response object\n");
		ofxXivelyResponse response = ofxXivelyResponse(res, rs, path, request.format);

		if (bVerbose) printf("[XIVELY] broadcast response event\n");
		ofNotifyEvent(responseEvent, response, this);

		if (bVerbose) printf("[XIVELY] ------------------------------\n\n");
	}catch (Exception& exc){
		printf("[XIVELY] Poco exception nr %d: %s\n", exc.code(), exc.displayText().c_str());
		bLastRequestOk = false;
	}
}

float
	ofxXivelyFeed::getValue(int _datastream)
{
	if (_datastream < pData.size())
		return pData.at(_datastream).fValue;

	return 0.f;
}

ofxXivelyData*
	ofxXivelyFeed::getDataStruct(int _datastream)
{
	if (_datastream >= pData.size())
		return NULL;

	return &pData.at(_datastream);
}