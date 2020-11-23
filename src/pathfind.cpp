#include "ofMain.h"
#include "pathfind.h"

using namespace glm;

int coordToNodeIndex (float x) {
	return (int)roundf(x/10);
}

float hueristic (Node* a, Node* b) {
	return ofDist(a->x, a->y, b->x, b->y);
}

bool path_FindPath(vector<vec2>* path, Node nodes[WIDTH][HEIGHT], vec2 startPoint, vec2 endPoint) {

  for(int i = 0; i < WIDTH; i++) {
    for(int j = 0; j < HEIGHT; j++) {
      nodes[i][j].g = std::numeric_limits<float>::max();
      nodes[i][j].f = std::numeric_limits<float>::max();
      nodes[i][j].h = 0;
    }
  }

  Node* startNode = &nodes[coordToNodeIndex(startPoint.x)][coordToNodeIndex(startPoint.y)];
  Node* endNode = &nodes[coordToNodeIndex(endPoint.x)][coordToNodeIndex(endPoint.y)];

  vector<Node*> neighbors;

  vector<Node*> openSet;
  openSet.push_back(startNode);
  startNode->g = 0;
  float dist = hueristic(startNode, endNode);
  startNode->f = dist;

  while(!openSet.empty()) {
    Node* current;
    int currentIndexInOpenSet;
    float lowestF = std::numeric_limits<float>::max();
    float d;
    for(int i = 0; i < openSet.size(); i++) {
      d = openSet[i]->f;
      if(d < lowestF) {
        current = openSet[i];
        lowestF = d;
        currentIndexInOpenSet = i;
      }
    }
    if(current->x == endNode->x && current->y == endNode->y) {

      path->clear();

      Node* step = current;
      while(!(step->x==startNode->x && step->y==startNode->y)){
        path->push_back(vec2(step->x, step->y));
        step = step->cameFrom;
      }
      path->push_back(vec2(startNode->x, startNode->y));

      return true;
    } else {
      openSet.erase(openSet.begin() + currentIndexInOpenSet);
      neighbors.clear();
      if(current->x > 0) 							neighbors.push_back(&nodes[coordToNodeIndex(current->x)-1	][coordToNodeIndex(current->y)]		);
      if(current->x < 500) 						neighbors.push_back(&nodes[coordToNodeIndex(current->x)+1	][coordToNodeIndex(current->y)]		);
      if(current->y > 0) 							neighbors.push_back(&nodes[coordToNodeIndex(current->x)		][coordToNodeIndex(current->y)-1]	);
      if(current->y < 500) 						neighbors.push_back(&nodes[coordToNodeIndex(current->x)		][coordToNodeIndex(current->y)+1]	);
      if(current->x > 0 && current->y > 0) 		neighbors.push_back(&nodes[coordToNodeIndex(current->x)-1	][coordToNodeIndex(current->y)-1]	);
      if(current->x > 0 && current->y < 500) 		neighbors.push_back(&nodes[coordToNodeIndex(current->x)-1	][coordToNodeIndex(current->y)+1]	);
      if(current->x < 500 && current->y > 0) 		neighbors.push_back(&nodes[coordToNodeIndex(current->x)+1	][coordToNodeIndex(current->y)-1]	);
      if(current->x < 500 && current->y < 500)	neighbors.push_back(&nodes[coordToNodeIndex(current->x)+1	][coordToNodeIndex(current->y)+1])	;

      for(int i = 0; i < neighbors.size(); i++) {
        if(neighbors[i]->free == PATH_WALL) continue;
        float distCost = (current->x!=neighbors[i]->x && current->y!=neighbors[i]->y) ? 1.41421356237 : 1; // adjacent dist is 1, diag is √2
        float testGScore = current->g + distCost;
        if(testGScore < neighbors[i]->g) {
          neighbors[i]->cameFrom = current;
          neighbors[i]->g = testGScore;
          neighbors[i]->f = testGScore + hueristic(current, neighbors[i]);

          bool inOpenSet = false;
          for(int j = 0; j < openSet.size(); j++) {
            if(openSet[j]->x == neighbors[i]->x && openSet[j]->y == neighbors[i]->y) {
              inOpenSet = true;
            }
          }
          if(inOpenSet==false) {
            openSet.push_back(neighbors[i]);
          }
        }
      }
    }
  }

  return false;
}
