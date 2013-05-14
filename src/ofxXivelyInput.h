#ifndef OFX_XIVELY_INPUT_H
#define OFX_XIVELY_INPUT_H

#include "ofMain.h"

#include "ofxXivelyFeed.h"

class ofxXivelyInput: public ofxXivelyFeed
{
public:
	ofxXivelyInput(bool _bThreaded = true);
	~ofxXivelyInput();

	bool					input(int _format = OFX_XIVELY_CSV, bool _force = false);
	/// supports only CSV input at the moment
	void                    onResponse(ofxXivelyResponse& response);

	void					setDatastreamCount(int _datastrams);
	bool					setValue(int _datastream, float _value);

private:
	std::string				makeCsv();

	float					fLastInput;
};

#endif
