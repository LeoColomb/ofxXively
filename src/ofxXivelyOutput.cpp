#include "ofxXivelyOutput.h"

ofxXivelyOutput::ofxXivelyOutput(bool _bThreaded): ofxXivelyFeed(_bThreaded) {
	ofAddListener(responseEvent, this, &ofxXivelyOutput::onResponse);
	fLastOutput = ofGetElapsedTimef();
}

ofxXivelyOutput::~ofxXivelyOutput() {}

bool ofxXivelyOutput::output(int _format, bool _force) {
	if (ofGetElapsedTimef() - fLastOutput < fMinInterval && !_force)
		return false;

	if (bThreaded && bRequestQueued)
		return false;

	if (sApiKey == "" || iFeedId == -1)
	{
		bLastRequestOk = false;
		return false;
	}

	if (_format == OFX_XIVELY_CSV)
	{
		request.method = OFX_XIVELY_GET;
		request.format = OFX_XIVELY_CSV;
		request.clearHeaders();
		request.addHeader("X-ApiKey", sApiKey);
		char pcUrl[256];
		sprintf(pcUrl, "%s%d.csv", sApiUrl.c_str(), iFeedId);
		request.url = pcUrl;
		request.timeout = 5;
	}
	else if (_format == OFX_XIVELY_EEML)
	{
		request.method = OFX_XIVELY_GET;
		request.format = OFX_XIVELY_EEML;
		request.clearHeaders();
		request.addHeader("X-ApiKey", sApiKey);
		char pcUrl[256];
		sprintf(pcUrl, "%s%d.xml", sApiUrl.c_str(), iFeedId);
		request.url = pcUrl;
		request.timeout = 5;
	}
	else
	{
		/// unrecognized format
		return false;
	}

	fLastOutput = ofGetElapsedTimef();

	if (bThreaded)
		bRequestQueued = true;
	else
		sendRequest(request);
	return true;
}

bool ofxXivelyOutput::parseResponseCsv(string _response) {
	bool bEOL = false;
	int i = 0;
	while (!bEOL)
	{
		int iPos = _response.find_first_of(",");
		bEOL = iPos < 0;

		if (pData.size() <= i)
		{
			ofxXivelyData d;
			d.iId = i;
			pData.push_back(d);
		}

		ofxXivelyData& data = pData.at(i);
		string sValue = _response.substr(0, iPos);
		while (sValue.at(0) == ' ')
			sValue = sValue.substr(1);
		data.fValue = atof(sValue.c_str());
		_response = _response.substr(iPos+1);

		++i;
	}

	return true;
}

bool ofxXivelyOutput::parseResponseEeml(string _response) {
	if (bVerbose) printf("[XIVELY] start parsing eeml\n");
	try
	{
		pData.clear();
		DOMParser parser;
		AttrMap* pMap;
		AutoPtr<Document> pDoc = parser.parseMemory(_response.c_str(), _response.length());

		NodeIterator itElem(pDoc, NodeFilter::SHOW_ELEMENT);

		Node* pNode = itElem.nextNode();
		while (pNode)
		{
			if (pNode->nodeName() == XMLString("environment"))
			{
				pMap = (AttrMap*)pNode->attributes();
				sUpdated = pMap->getNamedItem("updated")->nodeValue();
			}

			if (pNode->nodeName() == XMLString("title"))
				sTitle = pNode->firstChild()->getNodeValue();
			if (pNode->nodeName() == XMLString("status"))
				sStatus = pNode->firstChild()->getNodeValue();
			if (pNode->nodeName() == XMLString("description"))
				sDescription = pNode->firstChild()->getNodeValue();
			if (pNode->nodeName() == XMLString("website"))
				sWebsite = pNode->firstChild()->getNodeValue();

			if (pNode->nodeName() == XMLString("location"))
			{
				//				pMap = (AttrMap*)pNode->attributes();
				//				location.sDomain = pMap->getNamedItem("domain")->nodeValue();
				//				location.sExposure = pMap->getNamedItem("exposure")->nodeValue();
				//				location.sDisposition = pMap->getNamedItem("disposition")->nodeValue();

				NodeIterator itChildren(pNode, NodeFilter::SHOW_ELEMENT);
				Node* pChild = itChildren.nextNode();
				while (pChild)
				{
					if (pChild->nodeName() == XMLString("name"))
						location.sName = pChild->firstChild()->nodeValue();
					if (pChild->nodeName() == XMLString("lat"))
						location.sLat = pChild->firstChild()->nodeValue();
					if (pChild->nodeName() == XMLString("lon"))
						location.sLon = pChild->firstChild()->nodeValue();

					pChild = itChildren.nextNode();
				}
			}

			if (pNode->nodeName() == XMLString("data"))
			{
				ofxXivelyData data;

				pMap = (AttrMap*)pNode->attributes();
				data.iId = atoi(pMap->getNamedItem("id")->nodeValue().c_str());

				NodeIterator itChildren(pNode, NodeFilter::SHOW_ELEMENT);
				Node* pChild = itChildren.nextNode();
				while (pChild)
				{
					if (pChild->nodeName() == XMLString("tag"))
						data.pTags.push_back(pChild->firstChild()->getNodeValue());

					if (pChild->nodeName() == XMLString("value"))
					{
						data.fValue = atof(pChild->firstChild()->getNodeValue().c_str());

						pMap = (AttrMap*)pChild->attributes();
						data.fValueMin = atof(pMap->getNamedItem("minValue")->nodeValue().c_str());
						data.fValueMax = atof(pMap->getNamedItem("maxValue")->nodeValue().c_str());
					}

					pChild = itChildren.nextNode();
				}

				pData.push_back(data);
			}

			pNode = itElem.nextNode();
		}
	}
	catch (Exception& exc)
	{
		printf("[XIVELY] Parse xml exception: %s\n", exc.displayText().c_str());
		return false;
	}
	if (bVerbose) printf("[XIVELY] finished parsing eeml\n");

	return true;
}

void ofxXivelyOutput::onResponse(ofxXivelyResponse &response) {
	if (bVerbose)
	{
		printf("[XIVELY] received response with status %d\n", response.status);
		printf("[XIVELY] %s\n", response.reasonForStatus.c_str());
		printf("[XIVELY] %s\n", response.responseBody.c_str());
	}

	if (response.status == 200)
	{
		bool bParsedOk;
		if (response.format == OFX_XIVELY_CSV)
			bParsedOk = parseResponseCsv(response.responseBody);
		else if (response.format == OFX_XIVELY_EEML)
			bParsedOk = parseResponseEeml(response.responseBody);

		if (bParsedOk)
		{
			bLastRequestOk = true;
			fLastResponseTime = ofGetElapsedTimef();
		}
		else
		{
			bLastRequestOk = false;
		}
	}
	else
	{
		bLastRequestOk = false;
		printf("[XIVELY] Error: response failed with status %d\n", response.status);
		printf("[XIVELY] %s\n", response.responseBody.c_str());
	}
}
