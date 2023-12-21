#pragma once
#include <GL/glew.h>
#include "camera.h"
#include "model.h"

class PainterState {
public:
	PainterState(Camera camera) : camera(camera)
	{}
	std::string centralPath = "";
	std::string satellitePath = "";
	Camera camera;
	Model* centralModel = nullptr;
	Model* satelliteModel = nullptr;
};
