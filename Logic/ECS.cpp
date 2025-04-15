#include "ECS.h"

ECS* ECS::ecs = new ECS();
Transform defaultTransform;

Object::Object(): id(-1), transform(defaultTransform) { };
