#include "ofxCosmFeed.h"

#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/StreamConverter.h"
#include "Poco/URIStreamOpener.h"

#include "Poco/Exception.h"

#include <fstream>

ofxCosmFeed::ofxCosmFeed(bool _bThreaded)
{
	bThreaded = _bThreaded;
	bVerbose = true;

	sApiUrl = "http://www.cosm.com/api/";
	sApiKey = "";
	iFeedId = -1;

	fMinInterval = OFX_COSM_MIN_INTERVAL;
	bRequestQueued = false;
	bLastRequestOk = true;
	fLastResponseTime = -1.f;

	if (bThreaded)
		startThread();
}

ofxCosmFeed::~ofxCosmFeed()
{
	if (bThreaded)
		stopThread();
}

void
	ofxCosmFeed::setMinInterval(float fSeconds)
{
	if (fSeconds > OFX_COSM_MIN_INTERVAL)
		fMinInterval = fSeconds;
}

void
	ofxCosmFeed::setApiKey(std::string _sApiKey)
{
	sApiKey = _sApiKey;
}

void
	ofxCosmFeed::setFeedId(int _iId)
{
	iFeedId = _iId;
}

void
	ofxCosmFeed::threadedFunction()
{
	if (bVerbose) printf("[COSM] Thread started\n");
	while (true)
	{
		// check if new request is available
		if (bRequestQueued)
		{
			if (bVerbose) printf("[COSM] new request available\n");

			sendRequest(request);

			bRequestQueued = false;
		}

		// sleep
		ofSleepMillis(1000);
	}
}

void
	ofxCosmFeed::sendRequest(ofxCosmRequest request)
{
	try{
		URI uri( request.url.c_str() );
		string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		HTTPClientSession session(uri.getHost(), uri.getPort());
		session.setTimeout(Timespan(request.timeout, 0));

		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		if (request.method == OFX_COSM_PUT)
			req.setMethod(HTTPRequest::HTTP_PUT);

		/// headers
		for(unsigned int i=0; i<request.headerIds.size(); i++){
			const std::string name = request.headerIds[i].c_str();
			const std::string val = request.headerValues[i].c_str();
			req.set(name, val);
		}

		req.set("Content-Length", ofToString((int)request.data.length()));

		if (bVerbose) printf("[COSM] ------------------------------\n");
		if (bVerbose) printf("[COSM] write data request\n");
		session.sendRequest(req) << request.data;

		if (bVerbose) printf("[COSM] about to receive a response\n");
		HTTPResponse res;
		istream& rs = session.receiveResponse(res);
		if (bVerbose) printf("[COSM] received a session response\n");

		if (bVerbose) printf("[COSM] create new response object\n");
		ofxCosmResponse response = ofxCosmResponse(res, rs, path, request.format);

		if (bVerbose) printf("[COSM] broadcast response event\n");
		ofNotifyEvent(responseEvent, response, this);

		if (bVerbose) printf("[COSM] ------------------------------\n\n");
	}catch (Exception& exc){
		printf("[COSM] Poco exception nr %d: %s\n", exc.code(), exc.displayText().c_str());
		bLastRequestOk = false;
	}
}

float
	ofxCosmFeed::getValue(int _datastream)
{
	if (_datastream < pData.size())
		return pData.at(_datastream).fValue;

	return 0.f;
}

ofxCosmData*
	ofxCosmFeed::getDataStruct(int _datastream)
{
	if (_datastream >= pData.size())
		return NULL;

	return &pData.at(_datastream);
}