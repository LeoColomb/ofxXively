#ifndef OFX_XIVELY_OUTPUT_H
#define OFX_XIVELY_OUTPUT_H

#include "ofMain.h"

#include "ofxXivelyFeed.h"

#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/DOM/AttrMap.h"
#include "Poco/Exception.h"

#include <fstream>

using namespace std;
using namespace Poco::XML;
using namespace Poco;

class ofxXivelyOutput : public ofxXivelyFeed
{
public:
	ofxXivelyOutput(bool _bThreaded = true);
	~ofxXivelyOutput();

	bool output(int _format = OFX_XIVELY_CSV, bool _force = false);
	bool parseResponseEeml(string _response);
	bool parseResponseCsv(string _response);
	void onResponse(ofxXivelyResponse& response);

	ofxXivelyLocation&	getLocation() { return location; }
	string& getTitle() { return sTitle; }
	string&	getStatus() { return sStatus; }
	string&	getDescription() { return sDescription; }
	string&	getWebsite() { return sWebsite; }
	string& getUpdated() { return sUpdated; }

private:

	/// INFO ABOUT FEED ->
	string sTitle;
	string sStatus;
	string sDescription;
	string sWebsite;
	string sUpdated;

	ofxXivelyLocation location;
	/// <- INFO

	float fLastOutput;
};

#endif
