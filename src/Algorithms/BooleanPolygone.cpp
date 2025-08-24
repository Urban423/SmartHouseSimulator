#include "Polygon.h"

unsigned char getIntersectionPoints(std::vector<Vector2>& intersectionPoints, unsigned char& intersectionsCounter, Shapes::Polygon& a, Shapes::Polygon& b) {
	bool interect = false;
	int i = 0;
	int j = 0;
	while(i < a.size() && j < b.size()) {
		std::pair<Vector2, Vector2> edgeA =  a.getEdge(i);
		std::pair<Vector2, Vector2> edgeB =  b.getEdge(j);
		
		Vector2 intersectionA;
		Vector2 intersectionB;
		char count = 0;
		if(count = Vector2::linesItersection(intersectionA, intersectionB, edgeA.first, edgeA.second, edgeB.first, edgeB.second)) {
			interect = true;
			intersectionPoints[intersectionsCounter++] = intersectionA;
			if(count == 2)	intersectionPoints[intersectionsCounter++] = intersectionB;
		}
		
		if (edgeA.second < edgeB.second) {
			++i;
		} else {
			++j;
		}
	}
	
	return interect;
}

unsigned char BooleanOperation(std::vector<Shapes::Polygon>& polygons, char booleanOperation=0) {
	return 0;
}



