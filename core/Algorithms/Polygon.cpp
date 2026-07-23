#include "Polygon.h"
#include "Random.h"
#include "Triangulation.h"
#include <algorithm>
#include <math.h>
namespace Shapes {

Polygon::Polygon(const Rect& rect) {
	points.resize(4);
	points[0] = {rect.left,  rect.top};
	points[1] = {rect.left,  rect.bottom};
	points[2] = {rect.right, rect.bottom};
	points[3] = {rect.right, rect.top};
	init(std::move(points));
}


void Polygon::init(std::vector<Vector2>&& points) {
	int n = points.size();
	int size = n;
	edges.resize(n);
	for(int i = 0; i < n; ++i) {
		edges[i] = i; 
	}
	
	
	std::sort(edges.begin(), edges.end(), [&n, &points](const int a, const int& b) {
		int next;
		
		const Vector2& A1 = points[a];
		if(a == n){ next = 0; }
		else { next = a + 1;}
		const Vector2& A2 = points[next];
		
		const Vector2& B1 = points[b];
		if(b == n){ next = 0; }
		else { next = b + 1;}
		const Vector2& B2 = points[next];
		
		float minAx = std::min(A1.x, A2.x);
		float minAy = std::min(A1.y, A2.y);
		float minBx = std::min(B1.x, B2.x);
		float minBy = std::min(B1.y, B2.y);
		
		return (minAx == minBx) ? (minAy < minBy) : (minAx < minBx);
	});	
}

void Polygon::rotate(float angle) {
	for(int i = 0; i < points.size(); i++) {
		points[i].rotate(angle);
	}
}

bool Polygon::checkPointInside(const Vector2& point) {
	int crossings = 0;
	int n = points.size() - 1;
	
	for(int i = 0; i < n; i++) {
		Vector2 a = points[i];
		Vector2 b = points[i + 1];
		
		if((a.y > point.y) != (b.y > point.y)) {
			float intersectionX = a.x + (b.x - a.x) * (point.y - a.y) / (b.y - a.y);
			if(point.x < intersectionX) ++crossings;
		}
	}
	return crossings & 0x1;
}

Polygon generatePlatonicSolid(int n, float radius) {
	std::vector<Vector2> points;
    points.resize(n);
    float angle = 0;
    float delta_angle = 2 * 3.14159265f / n;

    for (int i = 0; i < n; i++) {
        points[i] = radius * Vector2(sin(angle), -cos(angle));
        angle += delta_angle;
    }
    
    return Polygon(std::move(points));
}


Rect Polygon::AABB() {
	Vector2 minBox{(float)0x7F7FFFFF, (float)0x7F7FFFFF};
	Vector2 maxBox{0, 0};
	for(int i = 0; i < points.size(); i++) {
		if(points[i].x < minBox.x) {
			minBox.x = points[i].x;
		}
		else if(points[i].x > maxBox.x) {
			maxBox.x = points[i].x;
		}
		
		if(points[i].y < minBox.y) {
			minBox.y = points[i].y;
		}
		else if(points[i].y > maxBox.y) {
			maxBox.y = points[i].y;
		}
	}
	return {minBox.x, minBox.y, maxBox.x, maxBox.y};
}

Polygon convexHull(std::vector<Vector2>& points) {
	if(points.size() < 4) {return Polygon(std::move(points));}
	
	std::sort(points.begin(), points.end());
	
	std::vector<Vector2> a;
	int n = points.size();
	a.resize(n << 1);
	int k = 0;
	for(int i = 0; i < n; ++i) {
		while (k >= 2 && clockwise(a[k - 2], a[k - 1], points[i])) --k;
		a[k++] = points[i];
	}
	
	int t = k + 1;
	for(int i = n - 2; i >= 0; --i) {
		while(k >= t && clockwise(a[k - 2], a[k - 1], points[i])) --k;
		a[k++] = points[i];
	}
	
	a.resize(k - 1);
	return Polygon(std::move(a));
}

Polygon generateRandomConvexHull(unsigned int n) {
	std::vector<float> xPool(n - 1), yPool(n - 1);
	generateSortedRandomArray(xPool.begin(), xPool.end(), 0.0f, 1.0f);
	generateSortedRandomArray(yPool.begin(), yPool.end(), 0.0f, 1.0f);
	
	float minX = xPool.front(), maxX = xPool.back();
	float minY = yPool.front(), maxY = yPool.back();
	
	std::vector<float> xVec, yVec;
    float lastTop = minX, lastBot = minX;
    float lastLeft = minY, lastRight = minY;

    for (int i = 1; i < n - 1; i++) {
		int r = rand();
        float x = xPool[i];
        if (r % 2) {
            xVec.push_back(x - lastTop);
            lastTop = x;
        } else {
            xVec.push_back(lastBot - x);
            lastBot = x;
        }
		
		float y = yPool[i];
        if (r % 4) {
            yVec.push_back(y - lastLeft);
            lastLeft = y;
        } else {
            yVec.push_back(lastRight - y);
            lastRight = y;
        }
    }

    xVec.push_back(maxX - lastTop);
    xVec.push_back(lastBot - maxX);
    yVec.push_back(maxY - lastLeft);
    yVec.push_back(lastRight - maxY);
	//std::random_shuffle(yVec.begin(), yVec.end());
	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(yVec.begin(), yVec.end(), g);
    
	
	std::vector<Vector2> vec(n);
    for (int i = 0; i < n; i++) {
        vec[i] = Vector2{xVec[i], yVec[i]};
    }
	
    std::sort(vec.begin(), vec.end(), [](const Vector2& a, const Vector2& b) { return Vector2::angle(a) < Vector2::angle(b); });
	
    std::vector<Vector2> points(n);
	float x = 0, y = 0;
    for (int i = 0; i < n; i++) {
        points[i] = {x, y};
		x += vec[i].x;
        y += vec[i].y;
    }
	
	return Shapes::Polygon(std::move(points));
}


Polygon generateRect(float width, float height) {
	std::vector<Vector2> points(4);
	points[0] = {0, 0};
	points[1] = {width, 0.0f};
	points[2] = {width, height};
	points[3] = {0.0f, height};
	return Polygon(std::move(points));
}

Polygon generateRandomRect(float scale, float ratio) {
	float width  = (float)noise(1.0f, scale);
	float height = width * ratio;
	std::vector<Vector2> points(4);
	points[0] = {0, 0};
	points[1] = {width, 0.0f};
	points[2] = {width, height};
	points[3] = {0.0f, height};
	return Polygon(std::move(points));
}

void bspSplit(float xMin, float xMax, float yMin, float yMax, int depth, std::vector<Vector2>& points) {
    if (depth == 0) {
        float x = noise(xMin, xMax);
        float y = noise(yMin, yMax);
        points.push_back({x, y});
        return;
    }
	
    bool vertical = noise(0, 1) > 0.5;
    float split = vertical ? noise(xMin, xMax) : noise(yMin, yMax);
	
    if (vertical) {
        bspSplit(xMin, split, yMin, yMax, depth - 1, points);
        bspSplit(split, xMax, yMin, yMax, depth - 1, points);
    } else {
        bspSplit(xMin, xMax, yMin, split, depth - 1, points);
        bspSplit(xMin, xMax, split, yMax, depth - 1, points);
    }
}


std::vector<Vector2> sortPointsIntoPolygon(std::vector<Vector2>& points) {
    Vector2 centroid = {0, 0};
    for (const auto& p : points) centroid = centroid + p;
    centroid.x /= points.size();
    centroid.y /= points.size();
    std::sort(points.begin(), points.end(), [&centroid](const Vector2& a, const Vector2& b) {
        return atan2(a.y - centroid.y, a.x - centroid.x) < atan2(b.y - centroid.y, b.x - centroid.x);
    });
    return points;
}

#include "Polygon.h"
#include "Triangulation.h"

Mesh Shapes::Polygon::convertToMesh() {
	Mesh mesh;
	
	int n = points.size();
	if (n < 3) return mesh;

	mesh.vertices.resize(n);
	for (int i = 0; i < n; i++) {
		mesh.vertices[i].pos[0] = points[i].x;
		mesh.vertices[i].pos[1] = points[i].y;
		mesh.vertices[i].pos[2] = 0.0f;
	}
	
	mesh.indices.resize((n - 2) * 3);
	std::vector<int> index_array(n);
	for (int i = 0; i < n; i++) index_array[i] = n - i - 1;
	
	std::vector<char> removed(n, false);
	
	TriangulatePolygon2D(
		points.data(),
		(bool*)removed.data(),
		index_array.data(),
		n,
		(int*)mesh.indices.data()
	);

	mesh.materials.resize(1, mesh.indices.size());
	mesh.syncWithGPU();
	return mesh;
}

bool PointInQuadXZ(float x, float z, Vector3 quad[4]) {
	Vector3 p(x, 0, z);
    bool positive = false;
    bool negative = false;
    for(int i = 0; i < 4; i++) {
        Vector3 a = quad[i];
        Vector3 b = quad[(i + 1) % 4];
        Vector3 edge(b.x - a.x, 0, b.z - a.z);
        Vector3 toPoint(p.x - a.x, 0, p.z - a.z);
        float cross = edge.z * toPoint.x - edge.x * toPoint.z;
        if(cross > 0) positive = true;
        if(cross < 0) negative = true;
        if(positive && negative) return false;
    }
    return true;
}
};