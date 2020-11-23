#pragma once

#include <vector>
#include "ofMain.h"

using namespace glm;

#define PATH_WALL 0
#define PATH_CLEAR 1

#define WIDTH 50
#define HEIGHT 50
#define SCALE 10

typedef struct node {
	int x;
	int y;
	int free;
	struct node* cameFrom;
	float g;
	float h;
	float f;
} Node;

extern bool path_FindPath (vector<vec2>* path, Node nodes[WIDTH][HEIGHT], vec2 startPoint, vec2 endPoint);
