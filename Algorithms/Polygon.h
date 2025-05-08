#pragma once
#include <vector>
#include <Vector2.h>
#include <Rect.h>
#include <umath.h>

namespace Shapes {

class Polygon {
public:
	Polygon() { };
	Polygon(std::vector<Vector2>&& points): points(points)  { init(std::move(points)); };
	Polygon(const std::vector<Vector2>& points): points(points) { init(std::move(this->points)); }
	Polygon(const Polygon& polygon): points(polygon.points) { init(std::move(points)); }
	Polygon(Polygon&& polygon): points(polygon.points), edges(polygon.edges) { }
	Polygon(const Rect& rect);
	
	void init(std::vector<Vector2>&& points);
	Rect AABB();
	bool checkPointInside(const Vector2& point);
	
	
	inline std::pair<Vector2, Vector2> 	getEdge(int i) 					{ return std::pair(points[i], points[(i + 1) % points.size()]); } 
	inline void 						addPoint(const Vector2& point) 	{  points.push_back(point); }
	inline unsigned int 				size() 							{ return points.size(); };
public:
	inline Polygon& operator=(const Polygon& polygon) 		{ points = polygon.points;  return *this;}
	inline Polygon& operator=(const Polygon&& polygon)  	{ points = polygon.points; return *this;}
	inline Vector2& operator[](const unsigned int index) 	{ return points[index]; }
private:
	std::vector<Vector2> points;
	std::vector<int> edges;
};

Polygon generateRandomConvexHull(unsigned int n);
Polygon generatePlatonicSolid(int n, float radius);
Polygon convexHull(std::vector<Vector2>& points);
Polygon generateRect(float width, float height);
Polygon generateRandomRect(float scale, float ratio);

unsigned char getIntersectionPoints(std::vector<Vector2>& intersectionPoints, unsigned char& intersectionsCounter, Shapes::Polygon& a, Shapes::Polygon& b);
unsigned char BooleanOperation(std::vector<Shapes::Polygon>& polygons, char booleanOperation=0);
};


bool inTrinagle(Vector2 a, Vector2  b, Vector2 c, Vector2 checkPoint);
void TriangulatePolygon(
	Vertex* vertex_array, Vector3 normal,
	Vector2* vector2_buffer, bool* removed_vertexes,
	int* index_array, int number_of_points,
	int* result_indices);