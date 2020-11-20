#pragma once

#include "ofMain.h"

#define WIDTH 50
#define HEIGHT 50
#define SCALE 10

typedef struct node {
	int x;
	int y;
	int free;
	struct node* cameFrom;
	int g;
	int h;
	int f;
} Node;


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
		
		ofImage img;
		float noises[WIDTH][HEIGHT];
		ofShader shader;
		ofImage backgroundImage;
		ofFbo maskFbo;
		
		node nodes[WIDTH][HEIGHT];

};
