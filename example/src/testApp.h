#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxCosm.h"

//--------------------------------------------------------
class testApp : public ofSimpleApp
{
	public:
        testApp();
        ~testApp();

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();

	private:
		ofxCosmOutput*               out;
		ofxCosmInput*                in;

        int                             iCounter;
};

#endif
