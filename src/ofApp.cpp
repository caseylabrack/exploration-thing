#include "ofApp.h"
#include <vector>

float noiseScaleX = 8;
float noiseScaleY = 8;
float cutoff = .35;
bool highres = false;
float seed;
float speed = 10;

float mouseX, mouseY;

using namespace glm;

vec2 player = vec2(0,0);

vector<line> lines;
vector<vector<line>> polylines;

vector<line> polygon;

vec2 hits [360];

vec2 startPoint = vec2(250,250);
vec2 endPoint = vec2(0,0);
vector<vec2> path;
ofPolyline pathshow;


//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(ofColor::white);
	ofSeedRandom();
	//~ seed = 1;
	seed = ofRandom(100);
	printf("seed set to %f\n", seed);

	player = vec2(250, 250);

	for(int i = 0; i < WIDTH; i++) {
		for(int j = 0; j < HEIGHT; j++) {
			noises[i][j] = ofNoise(i/noiseScaleX + seed, j/noiseScaleY + seed) > cutoff ? PATH_CLEAR : PATH_WALL;
		}
	}

	for(int l = 0; l < HEIGHT; l++) {
		noises[0][l] = PATH_WALL;
		noises[WIDTH-1][l] = PATH_WALL;
	}
	for(int k = 0; k < WIDTH; k++) {
		noises[k][0] = PATH_WALL;
		noises[k][HEIGHT - 1] = PATH_WALL;
	}

	// clear spawn
	for(int i = 20; i < 30; i++) {
		for(int j = 20; j < 30; j++) {
				noises[i][j] = PATH_CLEAR;
		}
	}

	lines_marchingSquares(&lines, noises);

	int passable;
	Node n;
	for(int i = 0; i < WIDTH; i++) {
		for(int j = 0; j < HEIGHT; j++) {
			passable = PATH_CLEAR;
			if(noises[i][j] == PATH_WALL) 					passable = PATH_WALL;
			if(i > 0) if(noises[i-1][j] == PATH_WALL) 		passable = PATH_WALL;
			if(i < WIDTH) if(noises[i+1][j] == PATH_WALL) 	passable = PATH_WALL;
			if(j > 0) if(noises[i][j-1] == PATH_WALL) 		passable = PATH_WALL;
			if(j < HEIGHT) if(noises[i][j+1] == PATH_WALL) 	passable = PATH_WALL;
			n.x = i * 10;
			n.y = j * 10;
			n.free = passable;
			nodes[i][j] = n;
		}
	}

	//~ line test = lines[0];

	//~ vector<int> indices;
	//~ for (auto i = lines.begin(); i != lines.end(); i++) {
		//~ indices[i] = 0;
	//~ }

	//~ for (int l = 0; l < lines.size(); l++) {
		//~ if(test.a==lines[l].b) {
			//~ polygon.push_back(lines[l].b);
		//~ }
	//~ }

	//~ printf("%lu number of lines\n", lines.size());


	#ifdef TARGET_OPENGLES
		shader.load("shadersES2/shader");
	#else
		if(ofIsGLProgrammableRenderer()){
			shader.load("shadersGL3/shader");
		}else{
			shader.load("shadersGL2/shader");
		}
	#endif

	maskFbo.allocate(500, 500, GL_RGBA);
	maskFbo.begin();
  ofClear(0,0,0,0);
  maskFbo.end();

  backgroundImage.allocate(500,500,OF_IMAGE_COLOR_ALPHA);
  backgroundImage.setColor(ofColor::black);
  backgroundImage.update();
}

//--------------------------------------------------------------
void ofApp::update(){

	player.x = mouseX;
	player.y = mouseY;

	line cast;
	cast.a = player;

	for(float i = .5; i < 360.5; i++) {
		cast.b.x = cast.a.x + cos(radians(i)) * 100;
		cast.b.y = cast.a.y + sin(radians(i)) * 100;
		hits[(int)i] = lines_lineCast(lines, cast);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){


	ofPushStyle();
	ofSetColor(ofColor::black);
	ofDrawCircle(player.x, player.y, 1);
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::black);
	ofDrawBitmapString("mouseX: " + ofToString(mouseX) + " mouseY: " + ofToString(mouseY), 10, 510);
	ofPopStyle();


	ofPushStyle();
	ofSetColor(ofColor::purple);
	maskFbo.begin();
	ofClear(0,0,0,0);
	ofBeginShape();
	for(int i = 0; i < 360; i++) {
		ofVertex(hits[i].x, hits[i].y);
	}
	ofEndShape(true);
    maskFbo.end();
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::black);
	shader.begin();
	shader.setUniformTexture("maskTex", maskFbo.getTexture(), 1 );
	shader.setUniform2f("pos", vec2(mouseX, mouseY));
	backgroundImage.draw(0, 0);
	shader.end();
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::antiqueWhite);
	ofNoFill();
	ofSetLineWidth(1);

	for (auto l = lines.begin(); l != lines.end(); l++) {
		ofDrawLine(l->a, l->b);
	}
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::green);
	ofNoFill();
	for(int i = 0; i < WIDTH; i++) {
		for(int j = 0; j < HEIGHT; j++) {
			if(!nodes[i][j].free) continue;
			ofDrawCircle(i*10,j*10,2);
		}
	}
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::red);
	ofDrawCircle(startPoint.x, startPoint.y, 3);
	ofDrawCircle(endPoint.x, endPoint.y, 3);
	pathshow.draw();
	ofPopStyle();



}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	 switch(key) {

		 case OF_KEY_RIGHT:
		 case 'd':
		 player.x += speed;
		 break;

		 case OF_KEY_LEFT:
		 case 'a':
		 player.x -= speed;
		 break;

		 case OF_KEY_UP:
		 case 'w':
		 player.y -= speed;
		 break;

		 case OF_KEY_DOWN:
		 case 's':
		 player.y += speed;
		 break;
	 }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

	mouseX = x;
	mouseY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

	endPoint = vec2(x, y);

	bool found = path_FindPath(&path, nodes, startPoint, vec2(x,y));

	pathshow.clear();
	for(int i = 0; i < path.size(); i ++) pathshow.addVertex(path[i].x, path[i].y);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
