#ifndef OFX_COSM_INPUT_H
#define OFX_COSM_INPUT_H

#include "ofMain.h"

#include "ofxCosmFeed.h"

class ofxCosmInput: public ofxCosmFeed
{
public:
	ofxCosmInput(bool _bThreaded = true);
	~ofxCosmInput();

	bool					input(int _format = OFX_COSM_CSV, bool _force = false);
                            /// supports only CSV input at the moment
	void                    onResponse(ofxCosmResponse& response);

	void					setDatastreamCount(int _datastrams);
	bool					setValue(int _datastream, float _value);

private:
	std::string				makeCsv();

	float					fLastInput;
};

#endif
