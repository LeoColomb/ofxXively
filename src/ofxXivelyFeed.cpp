#include "ofxXivelyFeed.h"

ofxXivelyFeed::ofxXivelyFeed(bool _bThreaded) {
	bThreaded = _bThreaded;
	bVerbose = true;

	sApiUrl = "https://api.xively.com/v2/feeds/";
	sApiKey = "";
	iFeedId = -1;

	fMinInterval = OFX_XIVELY_MIN_INTERVAL;
	bRequestQueued = false;
	bLastRequestOk = true;
	fLastResponseTime = -1.f;

	try {
		HTTPSStreamFactory::registerFactory();
		SharedPtr<PrivateKeyPassphraseHandler> pConsoleHandler = new KeyConsoleHandler(false);
		SharedPtr<InvalidCertificateHandler> pInvalidCertHandler = new ConsoleCertificateHandler(true);
		Context::Ptr pContext = new Context(Context::CLIENT_USE, "", Context::VERIFY_NONE);
		SSLManager::instance().initializeClient(pConsoleHandler, pInvalidCertHandler, pContext);
	}
	catch (Poco::SystemException & PS) {
		ofLogError("ofxXively") << "couldn't create factory: " << PS.displayText();
	}

	if (bThreaded)
		startThread();
}

ofxXivelyFeed::~ofxXivelyFeed() {
	if (bThreaded)
		stopThread();
}

void ofxXivelyFeed::setMinInterval(float fSeconds) {
	if (fSeconds > OFX_XIVELY_MIN_INTERVAL)
		fMinInterval = fSeconds;
}

void ofxXivelyFeed::setApiKey(string _sApiKey) {
	sApiKey = _sApiKey;
}

void ofxXivelyFeed::setFeedId(int _iId) {
	iFeedId = _iId;
}

void ofxXivelyFeed::threadedFunction() {
	ofLogVerbose("Xively") << "Thread started";

	while (true)
	{
		// check if new request is available
		if (bRequestQueued)
		{
			ofLogVerbose("Xively") << "New request available";

			sendRequest(request);
			bRequestQueued = false;
		}

		// sleep
		ofSleepMillis(1000);
	}
}

void ofxXivelyFeed::sendRequest(ofxXivelyRequest request) {
	try{
		URI uri(request.url.c_str());
		string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		ofPtr<HTTPSession> session;
		istream * rs;
		HTTPSClientSession * httpsSession = new HTTPSClientSession(uri.getHost(), uri.getPort());
		httpsSession->setTimeout(Timespan(request.timeout, 0));

		HTTPRequest req(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		if (request.method == OFX_XIVELY_PUT)
			req.setMethod(HTTPRequest::HTTP_PUT);

		/// headers
		for (unsigned int i = 0; i < request.headerIds.size(); i++){
			const string name = request.headerIds[i].c_str();
			const string val = request.headerValues[i].c_str();
			req.set(name, val);
		}

		req.set("Content-Length", ofToString((int) request.data.length()));

		ofLogVerbose("Xively") << "-----------------------------";
		ofLogVerbose("Xively") << "write data request";
		httpsSession->sendRequest(req) << request.data;

		ofLogVerbose("Xively") << "about to receive a response";
		HTTPResponse res;
		rs = &httpsSession->receiveResponse(res);
		session = ofPtr<HTTPSession>(httpsSession);
		ofLogVerbose("Xively") << "received a session response";

		ofLogVerbose("Xively") << "create new response object";
		ofxXivelyResponse response = ofxXivelyResponse(res, *rs, path, request.format);

		ofLogVerbose("Xively") << "broadcast response event";
		ofNotifyEvent(responseEvent, response, this);

		ofLogVerbose("Xively") << "------------------------------";
	}
	catch (Exception& exc) {
		ofLogError("ofxXively") << "Poco exception nr " << exc.code() << ": " << exc.displayText();
		bLastRequestOk = false;
	}
}

float ofxXivelyFeed::getValue(int _datastream) {
	if (_datastream < pData.size())
		return pData.at(_datastream).fValue;

	return 0.f;
}

ofxXivelyData * ofxXivelyFeed::getDataStruct(int _datastream) {
	if (_datastream >= pData.size())
		return NULL;

	return &pData.at(_datastream);
}
