#pragma once
#include <Vector2.h>
#include <Rect.h>
#include <Mesh.h>

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
	Mesh convertToMesh();
	void rotate(float angle);
	
	inline std::pair<Vector2, Vector2> 	getEdge(int i) 					{ return std::pair(points[i], points[(i + 1) % points.size()]); } 
	inline void 						addPoint(const Vector2& point) 	{  points.push_back(point); }
	inline unsigned int 				size() 							{ return points.size(); };
public:
	inline Polygon& operator=(const Polygon& polygon) 		{ points = polygon.points;  return *this;}
	inline Polygon& operator=(const Polygon&& polygon)  	{ points = polygon.points; return *this;}
	inline Vector2& operator[](const unsigned int index) 	{ return points[index]; }
	inline Polygon& operator+(const Vector2 point) {
		for(int i = 0; i < points.size(); i++) {
			points[i] += point;
		}
		return *this;
	}
private:
	std::vector<Vector2> points;
	std::vector<int> edges;
};

Polygon generateRandomConvexHull(unsigned int n);
Polygon generatePlatonicSolid(int n, float radius);
Polygon convexHull(std::vector<Vector2>& points);
Polygon generateRect(float width, float height);
Polygon generateRandomRect(float scale, float ratio);
bool PointInQuadXZ(float x, float z, Vector3 quad[4]);

unsigned char getIntersectionPoints(std::vector<Vector2>& intersectionPoints, unsigned char& intersectionsCounter, Shapes::Polygon& a, Shapes::Polygon& b);
unsigned char BooleanOperation(std::vector<Shapes::Polygon>& polygons, char booleanOperation=0);
};
