#pragma once
#include <unordered_map>
#include <GL/glew.h>
#include "camera.h"
#include "model.h"
#include "light_sources.h"

class PainterState {
public:
	PainterState(Camera camera) : camera(camera) {}

	Camera camera;

	Model* platform = nullptr;
	Model* lizardMk = nullptr;
	Model* warrior = nullptr;
	Model* gun = nullptr;
	Model* table = nullptr;
	Model* coffee = nullptr;

	Model* test = nullptr;

	PointSource pointSource = PointSource(1.0f, glm::vec3(0,0,0));
	DirectionalSource directionalSource = DirectionalSource(1.0f, glm::vec3(0, 100, 100));
	SpotlightSource spotlightSource = SpotlightSource(0.0f, glm::vec3(0, 5, 0), glm::vec3(0, 0, 0), 30.0f);
};
