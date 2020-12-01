#pragma once

#include "projconsts.h"
#include "ofMain.h"
#include "pathfind.h"
#include "lines.h"

typedef struct camera {
	float x;
	float y;
	float w;
	float h;
} camera;

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		float seed;

		ofShader shader;
		ofImage backgroundImage;
		ofFbo maskFbo;

		vec2 player;
		camera cam;

		int currentSector;
		float noises[WIDTH][HEIGHT];
		Node nodes[WIDTH][HEIGHT];
		vector<line> linesSectors[(int)WIDTH/SECTOR_WIDTH];

};
