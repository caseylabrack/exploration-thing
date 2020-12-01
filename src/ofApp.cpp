#include "ofApp.h"
#include <vector>

using namespace glm;

float speed = 10;

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
	seed = ofRandom(100); // 1;

	cam = {250, 250, 500, 500};

	player = vec2(250, 250);

	float noiseScaleX = 8;
	float noiseScaleY = 8;
	float cutoff = .35;

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

	vector<line> lines;
	lines_marchingSquares(&lines, noises);

	int sector;
	for(int i = 0; i < lines.size(); i++) {
		sector = (int)floorf(lines[i].a.x / SCALE / SECTOR_WIDTH);
		linesSectors[sector].push_back(lines[i]);
	}

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

	maskFbo.allocate(ofGetWidth(), 500, GL_RGBA);
	maskFbo.begin();
  ofClear(0,0,0,0);
  maskFbo.end();

  backgroundImage.allocate(ofGetWidth(),500,OF_IMAGE_COLOR_ALPHA);
  backgroundImage.setColor(ofColor::black);
  backgroundImage.update();
}

//--------------------------------------------------------------
void ofApp::update(){

	player.x = ofGetMouseX() + cam.x - cam.w/2;
	player.y = ofGetMouseY() + cam.y - cam.h/2;

	currentSector = (int)player.x / SCALE / SECTOR_WIDTH;

	line cast;
	cast.a = player;

	for(float i = .5; i < 360.5; i++) {
		cast.b.x = cast.a.x + cos(radians(i)) * 100;
		cast.b.y = cast.a.y + sin(radians(i)) * 100;
		hits[(int)i] = lines_lineCast(linesSectors,
																		currentSector > 0 ? currentSector - 1 : 0, // sector behind, unless at the start
																		currentSector < WIDTH / SECTOR_WIDTH ? currentSector + 1 : (int)WIDTH / SECTOR_WIDTH, // sector ahead, unless at the end
																		cast);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofPushStyle();
	ofSetColor(ofColor::black);
	ofDrawBitmapString("camX: " + ofToString(cam.x) + " camY: " + ofToString(cam.y), 10, 510);
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate(),2), 10, 530);
	ofDrawBitmapString("player: " + ofToString(player.x) + " :" + ofToString(player.y), 10, 550);
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::purple);
	maskFbo.begin();
	ofClear(0,0,0,0);
	ofBeginShape();
	for(int i = 0; i < 360; i++) {
		ofVertex(hits[i].x - (cam.x - cam.w/2), hits[i].y-(cam.y - cam.h/2));
	}
	ofEndShape(true);
  maskFbo.end();
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::black);
	shader.begin();
	shader.setUniformTexture("maskTex", maskFbo.getTexture(), 1 );
	shader.setUniform2f("pos", vec2(ofGetMouseX(), ofGetMouseY()));
	backgroundImage.draw(0, 0);
	shader.end();
	ofPopStyle();

	ofPushMatrix();

	ofTranslate(-(cam.x - cam.w/2), -(cam.y - cam.h/2));

	ofPushStyle();
	ofSetColor(ofColor::black);
	ofDrawCircle(player.x, player.y, 1);
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::antiqueWhite);
	ofNoFill();
	ofSetLineWidth(1);

	int low = currentSector > 0 ? currentSector - 1 : 0;
	int high = currentSector < WIDTH / SECTOR_WIDTH ? currentSector + 1 : WIDTH / SECTOR_WIDTH;
	for(int i = low; i <= high; i++) {
		for (int l = 0; l < linesSectors[i].size(); l++) {
			ofDrawLine(linesSectors[i][l].a, linesSectors[i][l].b);
		}
	}
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::green);
	ofNoFill();
	// for(int i = 0; i < linesSectors[sector].size(); i++) {
	// 	for(int j = 0; j < HEIGHT; j++) {
	// 		if(!nodes[i][j].free) continue;
	// 		ofDrawCircle(i*10,j*10,2);
	// 	}
	// }
	// for(int i = 0; i < WIDTH; i++) {
	// 	for(int j = 0; j < HEIGHT; j++) {
	// 		if(!nodes[i][j].free) continue;
	// 		ofDrawCircle(i*10,j*10,2);
	// 	}
	// }
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::red);
	ofDrawCircle(startPoint.x, startPoint.y, 3);
	ofDrawCircle(endPoint.x, endPoint.y, 3);
	pathshow.draw();
	ofPopStyle();

	ofPushStyle();
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::pink);
	ofSetLineWidth(4);

	for(int i = 0; i < linesSectors[currentSector].size(); i++) {
		ofDrawLine(linesSectors[currentSector][i].a, linesSectors[currentSector][i].b);
	}
	ofPopStyle();

	ofPopMatrix();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	 switch(key) {

		 case OF_KEY_RIGHT:
		 case 'd':
		 // player.x += speed;
		 cam.x += speed;
		 break;

		 case OF_KEY_LEFT:
		 case 'a':
		 // player.x -= speed;
		 cam.x -= speed;
		 break;

		 case OF_KEY_UP:
		 case 'w':
		 // player.y -= speed;
		 cam.y -= speed;
		 break;

		 case OF_KEY_DOWN:
		 case 's':
		 // player.y += speed;
		 cam.y += speed;
		 break;
	 }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

	endPoint = vec2(x + cam.x - cam.w/2, y + cam.y - cam.h/2);

	bool found = path_FindPath(&path, nodes, startPoint, endPoint);

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
