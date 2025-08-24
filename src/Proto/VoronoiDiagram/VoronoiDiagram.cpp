#include "Polygon.h"
#include <stdio.h>
#include <ImageEditor/ImageEditor.h>
#include <conio.h>
#include <algorithm>
#include "Random.h"
#include <random>
#include <Vector2.h>
#include <Matrix3x3.h>
#include <vector>
#include <Rect.h>
#include <ctime>

#define ImageWidth 	500
#define ImageHeight 800
#define ImageOffset 10

std::vector<int> generateColors(int n) {
    std::vector<int> colors;
    for (int i = 0; i < n; i++) {
        // Distribute hue values evenly around the color wheel (0-360 degrees)
        double hue = (360.0 / n) * i;
        
        // Convert HSV to RGB
        double C = 255;  // Max saturation & brightness
        double X = C * (1 - abs(fmod(hue / 60.0, 2) - 1));
        double m = 0;

        int r, g, b;
        if (hue < 60)      { r = C, g = X, b = 0; }
        else if (hue < 120){ r = X, g = C, b = 0; }
        else if (hue < 180){ r = 0, g = C, b = X; }
        else if (hue < 240){ r = 0, g = X, b = C; }
        else if (hue < 300){ r = X, g = 0, b = C; }
        else               { r = C, g = 0, b = X; }

        // Store the color as an integer
        colors.push_back(RGB(r + m, g + m, b + m));
    }
    return colors;
}

float ManhatanDistance(const Vector2& a, const Vector2& b) {
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

float ChebishovDistance(const Vector2& a, const Vector2& b) {
	return std::max(std::abs(a.x - b.x), std::abs(a.y - b.y));
}

float EuclidDistance(const Vector2& a, const Vector2& b) {
	float x = a.x - b.x;
	float y = a.y - b.y;
	return x * x + y * y;
}

float MultiplicativelyDistance(const Vector2& a, const Vector2& b) {
	return pow(pow(fabs(b.x - a.x), 3) + pow(fabs(b.y - a.y), 3), 0.33);
}



template <typename Iterator, typename T>
Iterator partitionVector2Array(Iterator begin, Iterator end, int dimension, const T centroid) {
	auto jt = begin;
	for(auto it = begin; it != end; ++it) {
		if((*it)[dimension] < centroid[dimension]) {
			std::swap(*jt, *it);
			++jt;
		}
	}
	return jt;
}

template <typename Iterator, typename T = Vector2>
void stepBSP(std::vector<Rect<float>>& rect, int& index, Iterator begin, Iterator end, int depth, float xStart, float yStart, float xEnd, float yEnd) {
	if (begin + 1 == end) { rect[index++] = {xStart, yStart, xEnd, yEnd}; return; }
	if (begin == end) { return; }
	Vector2 center = {0, 0};
	for(auto it = begin; it != end; ++it) { center += *it; }
	int n = std::distance(begin, end);
	center /= static_cast<float>(n);
	auto jt = partitionVector2Array(begin, end, depth % 2, center);
	
	++depth;
	if(depth % 2) {
		stepBSP(rect, index, begin, jt, depth, xStart, yStart, center.x, yEnd);
		stepBSP(rect, index, jt, end, depth, center.x, yStart, xEnd, yEnd);
	}
	else {
		stepBSP(rect, index, begin, jt, depth, xStart, yStart, xEnd, center.y);
		stepBSP(rect, index, jt, end, depth, xStart, center.y, xEnd, yEnd);
	}
}	

template <typename Iterator, typename T = Vector2>
std::vector<Rect<float>> calculateBSP(Iterator begin, Iterator end, Rect<float> rectangle) {
	std::vector<Rect<float>> rect(std::distance(begin, end));
	int index = 0;
	stepBSP(rect, index, begin, end, 0, rectangle.left, rectangle.top, rectangle.right, rectangle.bottom);
	return rect;
}


std::vector<Shapes::Polygon> connectPolygons(std::vector<Vector2>& ip, std::vector<Shapes::Polygon>& polygons) {
	int n = polygons.size();
	std::vector<Shapes::Polygon> connections(n);
	std::vector<Vector2> intersectionPoints(256);
	connections[0] = polygons[0];
	int k = 1;
	for(int i = 1; i < n; i++) {
		unsigned char intersectionsCounter = 0;
		unsigned char uniquePolygonsIntersectionCounter = 0;
		for(int j = 0; j < k; j++) {
			uniquePolygonsIntersectionCounter += getIntersectionPoints(intersectionPoints, intersectionsCounter, polygons[i], connections[j]);
			for (int m = 0; m < intersectionsCounter && m < intersectionPoints.size(); m++) {
				ip.push_back(intersectionPoints[m]);
			}
		}
		//printf("%d     ", uniquePolygonsIntersectionCounter);
		if(intersectionsCounter == 0) {
			connections[k] = polygons[i];
			++k;
		}
		else {
			k -= polygonBooleanOperation(connections, polygons[i], intersectionPoints, intersectionsCounter) - 1;
		}
	}
	connections.resize(k);
	return connections;
}


void VoronoiDiagram(Layer& layer, int voronooiResolution, int bspResolution, float minPercent, float maxPercent, float (*distFunc)(const Vector2&, const Vector2&), int seed) {
	int k1 = minPercent * voronooiResolution;
	int k2 = maxPercent * voronooiResolution;
	std::vector<Vector2> points(voronooiResolution);
	Rect<float> rect = {0, 0, 1, 1};
	std::vector<bool> removed(voronooiResolution, 1);
	std::vector<Vector2> points1(bspResolution);
	std::vector<Rect<float>> preRects;
	std::vector<Shapes::Polygon> rects(bspResolution);
	int rectsSize = 0;
	Shapes::Polygon polygon;
	
	
	srand(seed);
	generateRandomArrayVector2(points.begin(), points.end(), {rect.left, rect.bottom}, {rect.right, rect.top});
	generateRandomArrayVector2(points1.begin(), points1.end(), {rect.left, rect.bottom}, {rect.right, rect.top});
	std::sort(points.begin(), points.end(), [&](const Vector2& a, const Vector2& b) {
		float d1 = distFunc(a, Vector2(0.5, 0.5));
		float d2 = distFunc(b, Vector2(0.5, 0.5));
		return d1 < d2;
	});
	for(int i = k1;  i < k2; i++) {removed[i] = 0;}
	preRects = calculateBSP(points1.begin(), points1.end(), rect);
	printf("calculateBSP: ok\n");
	
	for(int j = 0; j < preRects.size(); j++) {
		float minDist = std::numeric_limits<float>::max();
		int index = 0;
		Vector2 middlePoint = {(preRects[j].right + preRects[j].left) / 2, (preRects[j].bottom + preRects[j].top) / 2};
		for(int i = 0; i < voronooiResolution; i++) {
			float dist = distFunc(points[i], middlePoint);
			if(minDist > dist) {minDist = dist; index = i;}
		}
		if(removed[index]) { continue; }
		std::vector<Vector2> rectsPoints(4);
		rectsPoints[0] = {preRects[j].left, preRects[j].top};
		rectsPoints[1] = {preRects[j].left, preRects[j].bottom};
		rectsPoints[2] = {preRects[j].right, preRects[j].bottom};
		rectsPoints[3] = {preRects[j].right, preRects[j].top};
		rects[rectsSize++] = Shapes::Polygon(rectsPoints);
	}
	rects.resize(rectsSize);
	printf("calculate rects: ok\n");
	
	//conect rects
	polygon = Shapes::Polygon(points);
	std::vector<Vector2> ip;
	std::vector<Shapes::Polygon> polygons = connectPolygons(ip, rects);
	printf("calculate polygone: ok\n");
	
	printf("polygons: %d\n", polygons.size());
	
	
	
	int red 	= RGB(0, 0, 255);
	int green 	= RGB(0, 255, 0);
	int blue 	= RGB(255, 0, 0);
	int purple 	= RGB(255, 0, 255);
	int yellow 	= RGB(255, 255, 0);
	int cyan 	= RGB(0, 255, 255);
	int white 	= RGB(255, 255, 255);
	int black 	= RGB(0, 0, 0);
	std::vector<int> colorArray = generateColors(voronooiResolution);
	Rect<float> box;
	Rect<float> miniBox = layer.getBox(box, polygon, ImageOffset);
	Matrix3x3 	mat;
	mat.setIdentity();
	mat.setScale(Vector2(miniBox.right, miniBox.right));
	mat.setTranslation(Vector2(miniBox.left, miniBox.top));
	miniBox = layer.getBox(box, polygon, ImageOffset);
	mat.setIdentity();
	mat.setScale(Vector2(miniBox.right, miniBox.right));
	mat.setTranslation(Vector2(miniBox.left, miniBox.top));
	
	// Vector2 a = {100, 100}; Vector2 b = {100, 500};
	// Vector2 c = {100, 200}; Vector2 d = {100, 300};
	// Vector2 intersectionPointA;
	// Vector2 intersectionPointB;
	
	// layer.drawLine2D(a, b, white);
	// layer.drawLine2D(c, d, red);
	
	// switch(Vector2::linesItersection(intersectionPointA, intersectionPointB, a, b, c, d)) {
		// case(2): {
			// layer.drawPixel(intersectionPointB, blue);
		// }
		// case(1): {
			// layer.drawPixel(intersectionPointA, blue);
		// }
	// }
	
	for(int j = 0; j < rects.size(); j++) { 
		for(int i = 0; i < rects[j].size(); i++) { rects[j][i] *= mat; } 
		for(int i = 0; i < rects[j].size(); i++) {layer.drawLine2D(rects[j][i], rects[j][(i + 1) % rects[j].size()], colorArray[j]); } 
		//layer.drawRectangle(rects[j], colorArray[j]);
	}
	for(int i = 0; i < ip.size(); i++) { ip[i] *= mat; } 
	for(int i = 0; i < ip.size(); i++) { layer.drawPixel(ip[i], red); } 
}


int main() {
	int seed = time(NULL);
	
	
	//generator
	Image image(ImageWidth, ImageHeight);	
	int nw = 2;
	int nh = 2;
	Layer* layers = new Layer[nw * nh];
	int layerWidth = ImageWidth / nw;
	int layerHeight = ImageHeight / nh;
	
	float (*distFunc[])(const Vector2&, const Vector2&) = { EuclidDistance, ManhatanDistance, MultiplicativelyDistance, ChebishovDistance};
	for(int i = 0; i < nw * nh; i++) {
		layers[i].init(layerWidth * (i % nw), layerHeight * (i / nw), layerWidth, layerHeight);
		VoronoiDiagram(layers[i], 1000, 25, 0.0f, 0.15f, distFunc[i % 4], seed);
		image.addLayer(&layers[i]);
	}
	
	//save and show
	image.saveImage("lol.bmp");
	ImageWindow win = image.showImage();
	char c = getch();
	return 0;
};