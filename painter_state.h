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
	Model* platform = nullptr;
	Model* lizardMk = nullptr;
	Model* kazak = nullptr;
	Model* gun = nullptr;
	Model* table = nullptr;
	Model* coffee = nullptr;

	Model* test = nullptr;
};
