#pragma once
#include "Rect.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "IOstructures.h"
#include "FBXfile.h"
#include <math.h>
#include <vector>
#include <umath.h>
//#define epsilon 0.000000000000000000000000000000000000001f

struct VertexWithNormal
{
 int index;
 int uv_index;
 Vector3 normal;
 
 bool operator==(const VertexWithNormal second)
 {
  if(this->uv_index == second.uv_index && 
  (this->normal.x == second.normal.x) && (this->normal.y == second.normal.y) && (this->normal.z == second.normal.z))
  {
   return 1;
  }
  return 0;
 }
};


struct VertexWithoutNormal
{
 int index;
 int uv_index;
 int normal_index;
 
 bool operator==(const VertexWithoutNormal second)
 {
  if(this->uv_index == second.uv_index && this->normal_index == second.normal_index) {
   return 1;
  }
  return 0;
 }
};

#ifdef Android
	#include "AndroidInputManager.h"
	#include "AndroidFilter.h"
	#include "AndroidFileManager.h"
	#define _Window AndroidFilter 
	#define _Input AndroidInputManager
#elif windows
	#include "Window.h"
	#include "KeyBoard.h"
	#define _Window Window 
	#define _Input keyBoard
#endif

class IOSystem
{
public:
	static void onCreate(const char* windowName, int width, int height, bool fullscreen);
	static void onUpdate();
	
	//time
	static void initTime();
	static double getDeltaTime();
	
	//input
	static void setCenterCursorPos();
	inline static _Input&  getInputInstance()  { return input; };
	inline static _Window& getOutputInstance() { return window; };
	
	static Mesh readFBX(const char* filename);
	static TextureStruct readBMP(const char* filename);
private:
	static _Input input;
	static _Window window;
};

