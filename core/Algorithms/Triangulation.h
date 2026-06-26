#pragma once
#include <Vector2.h>
#include <Mesh.h>
#include <vector>

enum class EventType { Site, Circle };

struct Event {
    EventType type;
    Vector2 point;
    int pointIndex;
    bool valid;
};

struct VoronoiEdge {
    Vector2 start;
    Vector2 end;
    int leftSiteIndex;
    int rightSiteIndex;
};

struct Arc {
    Vector2 site;
    int siteIndex;
    Event* circleEvent;
    VoronoiEdge* leftEdge = nullptr;
    VoronoiEdge* rightEdge = nullptr;
};

struct VoronoiDiagram {
    std::vector<VoronoiEdge> edges;
    std::vector<Vector2> vertices;
};

bool inTrinagle(Vector2 a, Vector2  b, Vector2 c, Vector2 checkPoint);
bool inCircle(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& p);

void TriangulatePolygon2D(
	Vector2* vector2_array, bool* removed_vertexes, int* index_array,
	int number_of_points, int* result_indices);

std::vector<int> DelaunayTriangulation(std::vector<Vector2>& points);

VoronoiDiagram voronoi(std::vector<Vector2>& points);

void TriangulatePolygon3D(
	Vertex* vertex_array, Vector3 normal, Vector2* vector2_array,
	bool* removed_vertexes, int* index_array, int number_of_points, int* result_indices);