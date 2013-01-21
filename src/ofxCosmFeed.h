/****************************
* This is an openFrameworks addon for talking to Cosm
* www.openframeworks.cc
* www.cosm.com
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
* while waiting for www.cosm.com to respond. The choice is yours...
****************************/

#ifndef OFX_COSM_FEED_H
#define OFX_COSM_FEED_H

#include "ofMain.h"

#define OFX_COSM_MIN_INTERVAL    5
#define OFX_COSM_GET             0
#define OFX_COSM_PUT             1
#define OFX_COSM_CSV             0
#define OFX_COSM_EEML            1


#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/Timespan.h"

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
//using Poco::Net::HTMLForm;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;
using Poco::Timespan;

struct ofxCosmLocation
{
	std::string				sDomain;
	std::string				sExposure;
	std::string				sDisposition;

	std::string				sName;
	std::string				sLat;
	std::string				sLon;
};

struct ofxCosmData
{
	int						iId;
	std::vector<std::string> pTags;
	float					fValue;
	float					fValueMin;
	float					fValueMax;
};

struct ofxCosmRequest
{
    ofxCosmRequest()
    {
    }
    ~ofxCosmRequest(){
        clearHeaders();
    }

	int             method;             /// GET or PUT
	int             format;             /// CSV or EEML
	string          url;
	int             timeout;            /// connection timeout

	vector <string> headerIds;          /// http header values/ids
	vector <string> headerValues;

	string          data;

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

struct ofxCosmResponse
{
    ofxCosmResponse(HTTPResponse& pocoResponse, std::istream &bodyStream, string _url, int _format)
    {
        status=pocoResponse.getStatus();
		timestamp=pocoResponse.getDate();
		reasonForStatus=pocoResponse.getReasonForStatus(pocoResponse.getStatus());
		contentType = pocoResponse.getContentType();

		StreamCopier::copyToString(bodyStream, responseBody);
        url = _url;
        format = _format;
    }
    ~ofxCosmResponse()
    {}

	int             status; 				/// return code for the response ie: 200 = OK
	string          reasonForStatus;		/// text explaining the status
	string          responseBody;		    /// the actual response
	string          contentType;			/// the mime type of the response
	Poco::Timestamp timestamp;		        /// time of the response
	string          url;
	int             format;                 /// CSV/EEML
};

class ofxCosmFeed: public ofThread
{
public:
	ofxCosmFeed(bool _bThreaded);
	virtual ~ofxCosmFeed();

	void					setMinInterval(float fSeconds);
	void					setApiKey(std::string _sApiKey);
	void					setFeedId(int _iId);
	int						getFeedId()
                            {return iFeedId;}
	void					setVerbose(bool _bVerbose)
                            {bVerbose = _bVerbose;}

    bool                    getLastRequestOk()
                            {return bLastRequestOk;}
    float                   getLastResponseTime()
                            {return fLastResponseTime;}

	int						getDatastreamCount() {return pData.size();}
	float					getValue(int _datastream);
	ofxCosmData*			getDataStruct(int _datastream);

protected:
    bool                    bThreaded;

	bool                    bRequestQueued;
	ofxCosmRequest       request;
	void                    threadedFunction();
    void                    sendRequest(ofxCosmRequest request);

    ofEvent<ofxCosmResponse> responseEvent;
    virtual void            onResponse(ofxCosmResponse& response) =0;
                            /// dummy function, just to make ofxCosmFeed impossible to instantiate
    bool                    bLastRequestOk;
    float                   fLastResponseTime;


	bool					bApiKeyOk;
	std::string				sApiKey;

	string                  sApiUrl;

	int						iFeedId;

	/// FEED DATA ->
	std::vector<std::string>::iterator itTags;
	std::vector<ofxCosmData>::iterator itData;
	std::vector<ofxCosmData> pData;
	/// <- FEED DATA

	float					fMinInterval;

	bool					bVerbose;
};

#endif
