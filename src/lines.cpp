#include "lines.h"

int getState(int a, int b, int c, int d) {
  return a * 8 + b * 4  + c * 2 + d * 1;
}

vec2 lines_lineCast (const vector<line> &field, const line l) {

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

vec2 lines_lineCast (vector<line>* sectors, int index, int indexmax, const line l) {

	vec2 out;
	float dist = ofDistSquared(l.a.x, l.a.y, l.b.x, l.b.y);
	vec2 closest = l.b;
	float testdist;

  int sectorsize;
  for(int i = index; i < indexmax;   i++) {
    sectorsize = sectors[i].size();
    for(int j = 0; j < sectorsize; j++) {
      if(ofLineSegmentIntersection(sectors[i][j].a, sectors[i][j].b, l.a, l.b, out)) {
        testdist = ofDistSquared(l.a.x, l.a.y, out.x, out.y);
        if(testdist < dist) {
          dist = testdist;
          closest = out;
        }
      }
    }
  }

	return closest;
}

void lines_marchingSquares (vector<line>* lines, float noises[WIDTH][HEIGHT]){
  vec2 a, b, c, d;
  int x, y;

  for(int i = 0; i < WIDTH - 1; i++) {
    for(int j = 0; j < HEIGHT - 1; j++) {

      x = i * SCALE;
      y = j * SCALE;

      //marching squares
      a = vec2(x + SCALE * .5	, 	y				);
      b = vec2(x + SCALE		,	y + SCALE * .5	);
      c = vec2(x + SCALE * .5	,	y + SCALE		);
      d = vec2(x				,	y + SCALE * .5	);

      int state = getState(noises[i][j], noises[i+1][j], noises[i+1][j+1], noises[i][j+1]);

      switch (state) {
        case 1:
        lines->push_back((struct line){c, d});
        break;
        case 2:
        lines->push_back((struct line){b, c});
        break;
        case 3:
        lines->push_back((struct line){b, d});
        break;
        case 4:
        lines->push_back((struct line){a, b});
        break;
        case 5:
        lines->push_back((struct line){a, d});
        lines->push_back((struct line){b, c});
        break;
        case 6:
        lines->push_back((struct line){a, c});
        break;
        case 7:
        lines->push_back((struct line){a, d});
        break;
        case 8:
        lines->push_back((struct line){a, d});
        break;
        case 9:
        lines->push_back((struct line){a, c});
        break;
        case 10:
        lines->push_back((struct line){a, b});
        lines->push_back((struct line){c, d});
        break;
        case 11:
        lines->push_back((struct line){a, b});
        break;
        case 12:
        lines->push_back((struct line){b, d});
        break;
        case 13:
        lines->push_back((struct line){b, c});
        break;
        case 14:
        lines->push_back((struct line){c, d});
        break;
        }
    }
  }
}
