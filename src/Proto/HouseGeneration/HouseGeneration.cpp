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
using namespace Shapes;

#define ImageWidth 	260
#define ImageHeight 500
#define ImageOffset 10


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
	Vector2 center = {0, 0};
	for(auto it = begin; it != end; ++it) { center += *it; }
	int n = std::distance(begin, end);
	center /= n;
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

template <typename Iterator, typename T = Vector2>
std::vector<Vector2> getPointsOfRects(Iterator begin, Iterator end) { 
	int n = std::distance(begin , end);
	std::vector<Vector2> points(n * 4);
	int k = 0;
	for(auto it = begin; it != end; ++it) {
		points[k++] = {it->left,  it->top};
		points[k++] = {it->left,  it->bottom};
		points[k++] = {it->right, it->top};
		points[k++] = {it->right, it->bottom};
	}
	return std::move(points);
}



void generateBuilding(Layer& layer, int seed) {	
	//srand(time(NULL));
	Shapes::Polygon polygon;
	polygon = generatePlatonicSolid(30000, 100000);
    //std::vector<Vector2> points; srand(time(NULL)); for(int i = 0; i < 10; i++) { points.push_back({rand() % 100, rand() % 100}); }
	//polygon = convexHull(points);
	//srand(time(NULL)); polygon = generateRandomConvexHull(1000);
	//polygon = generateRect(100, 200);
	//polygon = generateRandomRect(10, 0.123f);
	//polygon = Shapes::Polygon(std::move(generateBSPPolygon({0, 0, 10, 10}, 10)));
	
	int n = 4;
	int state = rand() % 4; // 0 1 2 3
	Rect<float> rect = {0, 0, 10, 10};
	std::vector<Vector2> points(n);
	generateRandomArrayVector2(points.begin(), points.end(), {rect.left, rect.bottom}, {rect.right, rect.top});
	
	int c = n;
	if(state & 1) { c *= 2; }
	if(state & 2) { c *= 2; }
	points.resize(c);
	for(int i = 0; i < n; i++) {
		if (state & 1) {
			points[i + 1 * n] = { points[i].x, -points[i].y };
		}
		if (state & 2) {
			points[i + ((state & 1) ? 2 : 1) * n] = { -points[i].x, points[i].y };
		}
		if ((state & 1) && (state & 2)) {
			points[i + 3 * n] = { -points[i].x, -points[i].y };
		}
	}
	
	
	polygon = Shapes::Polygon(std::move(points));
	std::vector<Rect<float>> rects = calculateBSP(points.begin(), points.end(), polygon.AABB());
	
	
	int cn = 3;
	std::vector<Vector2> center_points(cn);
	generateRandomArrayVector2(center_points.begin(), center_points.end(), {rect.left, rect.bottom}, {rect.right, rect.top});
	std::sort(rects.begin(), rects.end(), [&center_points](const Rect<float>& a, const Rect<float>& b) { 
		float wa = a.right - a.left;
		float ha = a.bottom - a.top;
		if(wa < ha) {std::swap(wa, ha);}
		float Sa = wa * ha;
		float aspectA = wa / ha;
		float wb = b.right - b.left;
		float hb = b.bottom - b.top;
		if(wb < hb) {std::swap(wb, hb);}
		float Sb = wb * hb;
		float aspectB = wb / hb;
		
		float altSquareDist = 0x7F'FF'FF'FF;
		float bltSquareDist = 0x7F'FF'FF'FF;
		for (const auto& point : center_points) {
			// Найти ближайшую точку на прямоугольнике 'a'
			float left = (std::abs(a.left - point.x) > std::abs(a.right - point.x)) ? a.right : a.left;
			float top  = (std::abs(a.top - point.y) > std::abs(a.bottom - point.y)) ? a.bottom : a.top;
			float altcManhattanDist = std::abs(left - point.x) + std::abs(top - point.y);
			altSquareDist = std::min(altSquareDist, altcManhattanDist);

			// Найти ближайшую точку на прямоугольнике 'b'
			left = (std::abs(b.left - point.x) > std::abs(b.right - point.x)) ? b.right : b.left;
			top  = (std::abs(b.top - point.y) > std::abs(b.bottom - point.y)) ? b.bottom : b.top;
			float bltcManhattanDist = std::abs(left - point.x) + std::abs(top - point.y);
			bltSquareDist = std::min(bltSquareDist, bltcManhattanDist);
		}
		
		float scoreA = 0.5 * Sa + 0.25 * aspectA + 0.15 * altSquareDist;
		float scoreB = 0.5 * Sb + 0.25 * aspectB + 0.15 * bltSquareDist;
		
		return Sb > Sa;
		
	} );
	
	//remove
	int n1 =(int)(0.25f * (float)c);
	std::vector<int> removed(rects.size()); int k1 = 0;
	for(int i = n1; i < n1 * 2; i++) {removed[i] = 1; if(removed[i]) {k1++; if(k1 == n1) {break;}}}
	
	k1 = rects.size() - 1;
	for(int i = 0; i < rects.size(); i++) {
		if(removed[i]) { std::swap(rects[i], rects[k1--]); }
	}
	
	
	std::vector<Vector2> poi = getPointsOfRects(rects.begin(), rects.begin() + k1);
	std::vector<Shapes::Polygon> polygons(k1);
	for(int i = 0; i < k1; ++i) { polygons[i] = rects[i]; }
	k1++;
	
	polygons.resize(2);
	polygons = connectPolygons(polygons);
	
	
	
	
	int purple 	= RGB(255, 0, 255);
	int red 	= RGB(0, 0, 255);
	int blue 	= RGB(255, 0, 0);
	int white 	= RGB(255, 255, 255);
	int black 	= RGB(0, 0, 0);
	Rect<float> box;
	Rect<float> miniBox = layer.getBox(box, polygon, ImageOffset);
	Matrix3x3 	mat;
	mat.setIdentity();
	mat.setScale(Vector2(miniBox.right, miniBox.right));
	mat.setTranslation(Vector2(miniBox.left, miniBox.top));
	for(int i = 0; i < points.size(); i++) { Vector2 cp = points[i] * mat; layer.drawPixel(cp.x, cp.y, purple);}
	
	for(int i = k1; i < rects.size(); i++) { Rect cr = rects[i] * mat; layer.drawRectangle(cr, black);}
	for(int i = 0; i < k1; i++) { Rect cr = rects[i] * mat; layer.drawRectangle(cr, blue);}	
	//for(int i = 0; i < polygon.size(); i++) { Vector2 cp1 = 1.1f * polygon[i] * mat; Vector2 cp2 = 1.1f * polygon[(i + 1) % polygon.size()] * mat;  layer.drawLine2D(cp1, cp2, blue); layer.drawPixel(cp1.x, cp1.y, purple);}	
}


int main() {
	srand(time(NULL));
	//generator
	Image image(ImageWidth, ImageHeight);	
	int nw = 3;
	int nh = 3;
	Layer* layers = new Layer[nw * nh];
	int layerWidth = ImageWidth / nw;
	int layerHeight = ImageHeight / nh;
	for(int i = 0; i < nw * nh; i++) {
		
		layers[i].init(layerWidth * (i % nw), layerHeight * (i / nw), layerWidth, layerHeight);
		generateBuilding(layers[i], time(NULL));
		image.addLayer(&layers[i]);
	}
	
	//save and show
	image.saveImage("lol.bmp");
	ImageWindow win = image.showImage();
	char c = getch();
	return 0;
};