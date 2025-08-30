#pragma once 


class Layer {
public:
	Layer();
	Layer(int width, int height);
	Layer(int x, int y, int width, int height);
	Layer(const char* filename);
	~Layer();
	
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
	template<class T>
	char drawRectangle(const Rect<T>& rect, int color);
	Rect<float> getBox(Rect<float>& rbox, Shapes::Polygon& polygon, int imageOffset);
private:
	int x;
	int y;
	int   width;
	int   height;
	int   *buffer;
	float *z_buffer;
};

Layer::Layer(): width(100), height(100), x(0), y(0) {
	buffer = new int[width * height];
	memset(buffer, 0, sizeof(int) * height * width);
	z_buffer = new float[width * height];
	memset(z_buffer, 0, sizeof(float) * height * width);
}

Layer::Layer(int width, int height): width(width), height(height), x(0), y(0) {
	buffer = new int[width * height];
	memset(buffer, 0, sizeof(int) * height * width);
	z_buffer = new float[width * height];
	memset(z_buffer, 0, sizeof(float) * height * width);
}

Layer::Layer(int x, int y, int width, int height): width(width), height(height), x(x), y(y) {
	buffer = new int[width * height];
	memset(buffer, 0, sizeof(int) * height * width);
	z_buffer = new float[width * height];
	memset(z_buffer, 0, sizeof(float) * height * width);
}

Layer::Layer(const char* filename) {
	x = 0;
	y = 0;
	TextureStruct texture = readBMP(filename);
	width  = texture.width;
	height = texture.height;
	buffer = texture.pixels;
	z_buffer = new float[width * height];
	memset(z_buffer, 0, sizeof(float) * height * width);
}

Layer::~Layer() {
	if(buffer != nullptr) {
		delete[] buffer;
	}
	if(z_buffer != nullptr) {
		delete[] z_buffer;
	}
}

void Layer::init(int x, int y, int width, int height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	if(buffer != nullptr) {delete[] buffer;}
	buffer = new int[width * height];
	memset(buffer, 0, sizeof(int) * height * width);
	if(z_buffer != nullptr) {delete[] z_buffer;}
	z_buffer = new float[width * height];
	memset(z_buffer, 0, sizeof(float) * height * width);
}

char Layer::drawLine2D(int x1, int y1, int x2, int y2, int color) {
	drawLine(x1, y1, 1, x2, y2, 1, color, buffer, z_buffer, width, height);
	return 0;
}

char Layer::drawLine2D(Vector2 a, Vector2 b, int color) {
	drawLine(a.x, a.y, 1, b.x, b.y, 1, color, buffer, z_buffer, width, height);
	return 0;
}


Rect<float> Layer::getBox(Rect<float>& rbox, Shapes::Polygon& polygon, int imageOffset) {
	Rect box = polygon.AABB();
	float imageWidth  = static_cast<float>(width  - (imageOffset << 1));
	float imageHeight = static_cast<float>(height - (imageOffset << 1));
	float dx = abs(box.right - box.left);
    float dy = abs(box.top - box.bottom);
    
    float scale = std::min(imageWidth / dx, imageHeight / dy); // Use the smallest scale

    float offsetX = imageOffset + (imageWidth  - dx * scale) / 2 - box.left * scale;
    float offsetY = imageOffset + (imageHeight - dy * scale) / 2 - box.top  * scale;

    rbox = {box.left * scale + offsetX, box.top * scale + offsetY,
            box.right * scale + offsetX, box.bottom * scale + offsetY};

    printf("Scale: %f %f\n", imageWidth, imageHeight);
    //printf("Bounding Box X: %f %f\n", rbox.left, rbox.right);
    //printf("Bounding Box Y: %f %f\n", rbox.top, rbox.bottom);
   // printf("Bounding Box Y1: %f %f\n", offsetX, offsetY);
	
	return {offsetX, offsetY, scale, scale};
}

template<class T>
char Layer::drawRectangle(const Rect<T>& rect, int color) {
	
	int xStart 	= rect.left;
	int xEnd 	= rect.right;
	int yStart 	= rect.top;
	int yEnd 	= rect.bottom;
	
	if(xStart 	< 0) 		{ xStart 	= 0; }
	if(xEnd	 	> width) 	{ xEnd 		= width; }
	if(yStart 	< 0) 		{ yStart 	= 0; }
	if(yEnd	 	> height) 	{ yEnd 		= height; }
	
	for(int y = yStart; y < yEnd; y++) {
		int* ptr = buffer + y * width + (int)xStart;
		for(int x = xStart; x < xEnd; x++) {
			*(ptr++) = color;
		}
	}
	
	// drawLine2D(rect.left,  rect.bottom, rect.left,  rect.top,    color);
	// drawLine2D(rect.right, rect.bottom, rect.right, rect.top,    color);
	// drawLine2D(rect.left,  rect.top,    rect.right, rect.top,    color);
	// drawLine2D(rect.left,  rect.bottom, rect.right, rect.bottom, color);
	return 0;
}