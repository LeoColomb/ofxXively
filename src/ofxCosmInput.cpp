#include "ofxCosmInput.h"

#include "Poco/Exception.h"

#include <fstream>

ofxCosmInput::ofxCosmInput(bool _bThreaded)
	: ofxCosmFeed(_bThreaded)
{
	ofAddListener(responseEvent, this, &ofxCosmInput::onResponse);
	fLastInput = ofGetElapsedTimef();
}

ofxCosmInput::~ofxCosmInput()
{
}

std::string
	ofxCosmInput::makeCsv()
{
	char pcCsv[1024];
	pcCsv[0] = '\0';

	bool bPrependComma = false;
	for (itData = pData.begin(); itData != pData.end(); ++itData)
	{
		if (bPrependComma)
			sprintf(pcCsv, "%s,\0", pcCsv);
		else
			bPrependComma = true;

		sprintf(pcCsv, "%s%f\0", pcCsv, (*itData).fValue);
	}

	return std::string(pcCsv);
}

bool
	ofxCosmInput::input(int _format, bool _force)
{
	if (ofGetElapsedTimef() - fLastInput < fMinInterval && !_force)
		return false;

	if (bThreaded && bRequestQueued)
		return false;

	if (sApiKey == "" || iFeedId == -1)
	{
		bLastRequestOk = false;
		return false;
	}

	if (_format == OFX_COSM_CSV)
	{
		request.method = OFX_COSM_PUT;
		request.format = OFX_COSM_CSV;
		request.clearHeaders();
		request.addHeader("X-CosmApiKey", sApiKey);
		char pcUrl[256];
		sprintf(pcUrl, "%s%d.csv", sApiUrl.c_str(), iFeedId);
		request.url = pcUrl;
		request.data = makeCsv();
		request.timeout = 5;
	}
	else
	{
		/// unrecognized format
		return false;
	}

	fLastInput = ofGetElapsedTimef();

	if (bThreaded)
		bRequestQueued = true;
	else
		sendRequest(request);
	return true;
}

void
	ofxCosmInput::onResponse(ofxCosmResponse &response)
{
	if (bVerbose)
	{
		printf("[COSM] received response with status %d\n", response.status);
		printf("[COSM] %s\n", response.reasonForStatus.c_str());
		printf("[COSM] %s\n", response.responseBody.c_str());
	}

	if (response.status == 200)
	{
		/// input OK
		bLastRequestOk = true;
		fLastResponseTime = ofGetElapsedTimef();
		if (bVerbose) printf("[COSM] Input succeded\n");
	}
	else
	{
		bLastRequestOk = false;
		printf("[COSM] Error: response failed with status %d\n", response.status);
		printf("[COSM] %s\n", response.responseBody.c_str());
	}
}

void
	ofxCosmInput::setDatastreamCount(int _datastreams)
{
	while (pData.size() > _datastreams)
		pData.pop_back();

	while (pData.size() < _datastreams)
	{
		ofxCosmData data;
		data.iId = pData.size();
		data.fValue = 0.f;
		data.fValueMin = 0.f;
		data.fValueMax = 0.f;

		pData.push_back(data);
	}
}

bool
	ofxCosmInput::setValue(int _datastream, float _value)
{
	if (_datastream >= pData.size())
		return false;

	pData.at(_datastream).fValue = _value;
	return true;
}