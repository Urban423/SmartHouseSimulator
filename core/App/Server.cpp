#include "Server.h"
#include "Color.h"
#include "Rect.h"
#include "Window.h"
#include "TimeSystem.h"
#include "Scene.h"
#include "WindowsPlatform.h"

void Server::create() {
	Scene::Start();
	Time::start();
}

void Server::update() {
    Time::update();
    accumulator += Time::realDeltaTime;
    while(accumulator >= Time::fixedStep) {
        Time::fixedUpdate();
        Scene::FixedUpdate();
        accumulator -= Time::fixedStep;
    }
}