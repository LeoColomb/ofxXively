#ifndef OFX_XIVELY_OUTPUT_H
#define OFX_XIVELY_OUTPUT_H

#include "ofMain.h"

#include "ofxXivelyFeed.h"

class ofxXivelyOutput: public ofxXivelyFeed
{
public:
	ofxXivelyOutput(bool _bThreaded = true);
	~ofxXivelyOutput();

	bool					output(int _format = OFX_XIVELY_CSV, bool _force = false);
	bool                    parseResponseEeml(string _response);
	bool                    parseResponseCsv(string _response);
	void                    onResponse(ofxXivelyResponse& response);

	ofxXivelyLocation&		getLocation() {return location;}
	std::string&			getTitle() {return sTitle;}
	std::string&			getStatus() {return sStatus;}
	std::string&			getDescription() {return sDescription;}
	std::string&			getWebsite() {return sWebsite;}
	std::string&            getUpdated() {return sUpdated;}

private:

	/// INFO ABOUT FEED ->
	std::string				sTitle;
	std::string				sStatus;
	std::string				sDescription;
	std::string				sWebsite;

	std::string             sUpdated;

	ofxXivelyLocation		location;
	/// <- INFO

	float                   fLastOutput;
};

#endif
