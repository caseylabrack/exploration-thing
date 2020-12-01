#pragma once

#include "projconsts.h"
#include <vector>
#include "ofMain.h"

using namespace glm;

struct line {
	vec2 a;
	vec2 b;
};

extern vec2 lines_lineCast (vector<line>* sectors, int index, int indexmax, const line l);
extern void lines_marchingSquares (vector<line>* lines, float noises[WIDTH][HEIGHT]);
