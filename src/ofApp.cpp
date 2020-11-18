#include "ofApp.h"
#include <vector>

float noiseScaleX = 8;
float noiseScaleY = 8;
float cutoff = .4;
bool highres = false;
float seed;
float speed = 10;

float mouseX, mouseY;

using namespace glm;

vec2 player = vec2(0,0);

struct line {
	vec2 a;
	vec2 b;
};

vector<line> lines;
vector<vector<line>> polylines;

vector<line> polygon;

vec2 hits [360];
int rangelow = 0;
int rangehigh = 10;


int getState(int a, int b, int c, int d) {
  return a * 8 + b * 4  + c * 2 + d * 1;
}

vec2 lineCast (const vector<line> &field, const line l) {
	
	vec2 out;
	float dist = ofDistSquared(l.a.x, l.a.y, l.b.x, l.b.y);
	vec2 closest = l.b;
	float testdist;
	
	for (auto i = field.begin(); i != field.end(); i++) {
		if(ofLineSegmentIntersection(i->a, i->b, l.a, l.b, out)){
			testdist = ofDistSquared(l.a.x, l.a.y, out.x, out.y);
			if(testdist < dist) {
				dist = testdist;
				closest = out;
			}
		};
	}
	
	return closest;
}

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
			noises[i][j] = ofNoise(i/noiseScaleX + seed, j/noiseScaleY + seed) > cutoff ? 1 : 0;
		}
	}
	
	for(int l = 0; l < HEIGHT; l++) {
		noises[0][l] = 1;
		noises[WIDTH-1][l] = 1;
	}
	for(int k = 0; k < WIDTH; k++) {
		noises[k][0] = 1;
		noises[k][HEIGHT - 1] = 1;
	}
	
	// clear spawn
	noises[(int)floor(WIDTH/2)][(int)floor(HEIGHT/2)] = 1;
	noises[(int)floor(WIDTH/2) + 1][(int)floor(HEIGHT/2)] = 1;
	noises[(int)floor(WIDTH/2)][(int)floor(HEIGHT/2) + 1] = 1;
	noises[(int)floor(WIDTH/2) + 1][(int)floor(HEIGHT/2) + 1] = 1;
	
	vec2 a, b, c, d;	
	int x, y;
	
	for(int i = 0; i < WIDTH - 1; i++) {
		for(int j = 0; j < HEIGHT - 1; j++) {
			
			x = i * SCALE;
			y = j * SCALE;
			
			ofSetColor(noises[i][j] * 255);
			ofDrawRectangle(x, y, SCALE, SCALE);
			
			//marching squares
			a = vec2(x + SCALE * .5	, 	y				);
			b = vec2(x + SCALE		,	y + SCALE * .5	);
			c = vec2(x + SCALE * .5	,	y + SCALE		);
			d = vec2(x				,	y + SCALE * .5	); 
			
			int state = getState(noises[i][j], noises[i+1][j], noises[i+1][j+1], noises[i][j+1]);
			
			switch (state) {
			  case 1:  
				lines.push_back((struct line){c, d});
				break;
			  case 2:  
				lines.push_back((struct line){b, c});
				break;
			  case 3:  
				lines.push_back((struct line){b, d});
				break;
			  case 4:  
				lines.push_back((struct line){a, b});
				break;
			  case 5:  
				lines.push_back((struct line){a, d});
				lines.push_back((struct line){b, c});
				break;
			  case 6:  
				lines.push_back((struct line){a, c});
				break;
			  case 7:  
				lines.push_back((struct line){a, d});
				break;
			  case 8:  
				lines.push_back((struct line){a, d});
				break;
			  case 9:  
				lines.push_back((struct line){a, c});
				break;
			  case 10: 
				lines.push_back((struct line){a, b});
				lines.push_back((struct line){c, d});
				break;
			  case 11: 
				lines.push_back((struct line){a, b});
				break;
			  case 12: 
				lines.push_back((struct line){b, d});
				break;
			  case 13: 
				lines.push_back((struct line){b, c});
				break;
			  case 14: 
				lines.push_back((struct line){c, d});
				break;
			  }
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
    
    //~ backgroundImage.load("B.jpg");
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

	vec2 out;

	for(float i = .5; i < 360.5; i++) {
		cast.b.x = cast.a.x + cos(radians(i)) * 100;
		cast.b.y = cast.a.y + sin(radians(i)) * 100;
		hits[(int)i] = lineCast(lines, cast);
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
	ofSetLineWidth(6);
	
	for (auto l = lines.begin(); l != lines.end(); l++) {
		ofDrawLine(l->a, l->b);
	}
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
