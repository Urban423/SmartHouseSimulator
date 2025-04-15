#pragma once
#include "IOSystem.h"
#include "GraphicsEngine.h"
#include <umath.h>

#include <thread>
#include <vector>
#include "Scene.h"
#include "RenderManager.h"

class App: IOSystem, public AppI
{
public:
	void onCreate();
	void onUpdate();
	void onUpdateIO();
	void Move();
private:
	void GraphicInit();
	void enterResizingMode();
	void  exitResizingMode();
public:
	std::vector<Scene> scenes;
	std::thread resizeThread;
	bool mode;
};