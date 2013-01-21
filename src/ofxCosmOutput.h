#ifndef OFX_COSM_OUTPUT_H
#define OFX_COSM_OUTPUT_H

#include "ofMain.h"

#include "ofxCosmFeed.h"

class ofxCosmOutput: public ofxCosmFeed
{
public:
	ofxCosmOutput(bool _bThreaded = true);
	~ofxCosmOutput();

	bool					output(int _format = OFX_COSM_CSV, bool _force = false);
	bool                    parseResponseEeml(string _response);
	bool                    parseResponseCsv(string _response);
	void                    onResponse(ofxCosmResponse& response);

	ofxCosmLocation&		getLocation() {return location;}
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

	ofxCosmLocation		location;
	/// <- INFO

	float                   fLastOutput;
};

#endif
