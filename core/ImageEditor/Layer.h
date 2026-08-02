#pragma once
#include <Rect.h>
#include <Polygon.h>

class Layer {
public:
	Layer();
	Layer(int width, int height);
	Layer(int x, int y, int width, int height);
	Layer(const char* filename);
	//~Layer();
	
	void init(int x, int y, int width, int height);
	
	inline void setOffsetX(int offset) {x = offset;}
	inline void setOffsetY(int offset) {y = offset;}
	inline void setWidth  (int width)  {this->width  = width;}
	inline void setHeight (int height) {this->height = height;}
	
	inline int  getWidth()  {return width;}
	inline int  getHeight() {return height;}
	inline int* getImage()  {return buffer;}
	inline int  getOffsetX(){return x;}
	inline int  getOffsetY(){return y;}
	inline int* getPtr()	{return buffer;}
	
	inline void drawPixel(int x, int y, int color) { buffer[x + width * y] = color; }
	inline void drawPixel(const Vector2& p, int color) { if(p.x < 0) {return;} if(p.x > width- 2) {return;} if(p.y < 0) {return;} if(p.y > height- 2) {return;} drawPixel(p.x, p.y,color);   drawPixel(p.x + 1, p.y,color);  drawPixel(p.x, p.y + 1,color);  drawPixel(p.x + 1, p.y + 1,color); }
	char drawLine2D(int x1, int y1, int x2, int y2, int color);
	char drawLine2D(Vector2 a, Vector2 b, int color);
	char drawBezier(Vector2 a, Vector2 b, Vector2 c, int color);
	char drawRectangle(const Rect& rect, int color);
	Rect getBox(Rect& rbox, Shapes::Polygon& polygon, int imageOffset);
private:
	int x;
	int y;
	int   width;
	int   height;
	int   *buffer;
	float *z_buffer;
};
