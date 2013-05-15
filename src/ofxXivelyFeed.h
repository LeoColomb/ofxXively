/****************************
* This is an openFrameworks addon for talking to Xively
* www.openframeworks.cc
* xively.com
*
* Works with of release 0.7.3
*
* Written by CJ
* www.carljohanrosen.com
*
* Updated by Leo Colombaro - 2013
*
*
* There is an issue with threading and broken internet connection which causes the
* app to crash if connection was broken when the application was started. Therefore
* the addon has the possibility to run non-threaded which causes the app to hang
* while waiting for xively.com to respond. The choice is yours...
****************************/

#ifndef OFX_XIVELY_FEED_H
#define OFX_XIVELY_FEED_H

#include "ofMain.h"

#define OFX_XIVELY_MIN_INTERVAL    5
#define OFX_XIVELY_GET             0
#define OFX_XIVELY_PUT             1
#define OFX_XIVELY_CSV             0
#define OFX_XIVELY_EEML            1

#include "Poco/Net/HTTPSession.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/Timespan.h"
#include "Poco/Net/HTTPSStreamFactory.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"

#include <fstream>

using namespace std;
using namespace Poco::Net;
using namespace Poco;

struct ofxXivelyLocation {
	string sDomain;
	string sExposure;
	string sDisposition;
	string sName;
	string sLat;
	string sLon;
};

struct ofxXivelyData {
	int iId;
	vector<string> pTags;
	float fValue;
	float fValueMin;
	float fValueMax;
};

struct ofxXivelyRequest {
	ofxXivelyRequest() {}
	~ofxXivelyRequest() {
		clearHeaders();
	}

	int            method;             /// GET or PUT
	int            format;             /// CSV or EEML
	string         url;
	int            timeout;            /// connection timeout

	vector<string> headerIds;          /// http header values/ids
	vector<string> headerValues;

	string         data;

	// ----------------------------------------------------------------------
	void addHeader(string id, string value){
		headerIds.push_back( id );
		headerValues.push_back( value );
	}
	// ----------------------------------------------------------------------
	void clearHeaders(){
		headerIds.clear();
		headerValues.clear();
	}
};

struct ofxXivelyResponse {
	ofxXivelyResponse(HTTPResponse& pocoResponse, istream &bodyStream, string _url, int _format) {
		status = pocoResponse.getStatus();
		timestamp = pocoResponse.getDate();
		reasonForStatus = pocoResponse.getReasonForStatus(pocoResponse.getStatus());
		contentType = pocoResponse.getContentType();

		StreamCopier::copyToString(bodyStream, responseBody);
		url = _url;
		format = _format;
	}
	~ofxXivelyResponse() {}

	int             status; 				/// return code for the response ie: 200 = OK
	string          reasonForStatus;		/// text explaining the status
	string          responseBody;		    /// the actual response
	string          contentType;			/// the mime type of the response
	Timestamp timestamp;		        /// time of the response
	string          url;
	int             format;                 /// CSV/EEML
};

class ofxXivelyFeed: public ofThread {
public:
	ofxXivelyFeed(bool _bThreaded);
	virtual ~ofxXivelyFeed();

	void					setMinInterval(float fSeconds);
	void					setApiKey(string _sApiKey);
	void					setFeedId(int _iId);
	int						getFeedId() {return iFeedId;}
	void					setVerbose(bool _bVerbose) {bVerbose = _bVerbose;}

	bool                    getLastRequestOk() {return bLastRequestOk;}
	float                   getLastResponseTime() {return fLastResponseTime;}

	int						getDatastreamCount() {return pData.size();}
	float					getValue(int _datastream);
	ofxXivelyData*			getDataStruct(int _datastream);

protected:
	bool                    bThreaded;

	bool                    bRequestQueued;
	ofxXivelyRequest        request;
	void                    threadedFunction();
	void                    sendRequest(ofxXivelyRequest request);

	ofEvent<ofxXivelyResponse> responseEvent;
	virtual void            onResponse(ofxXivelyResponse& response) = 0;
	/// dummy function, just to make ofxXivelyFeed impossible to instantiate
	bool                    bLastRequestOk;
	float                   fLastResponseTime;

	bool					bApiKeyOk;
	string				sApiKey;

	string                  sApiUrl;

	int						iFeedId;

	/// FEED DATA ->
	vector<string>::iterator itTags;
	vector<ofxXivelyData>::iterator itData;
	vector<ofxXivelyData> pData;
	/// <- FEED DATA

	float					fMinInterval;

	bool					bVerbose;
};

#endif
