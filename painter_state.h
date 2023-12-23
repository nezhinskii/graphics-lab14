#pragma once
#include <unordered_map>
#include <GL/glew.h>
#include "camera.h"
#include "model.h"

enum Shading : int {
	Phong,
	Toon,
	Other
};

struct LightSource {
	GLfloat intensity;

	LightSource(GLfloat intensity) : intensity(intensity) {}
};

struct PointSource: LightSource {
	glm::vec3 pos;

	PointSource(GLfloat intensity, glm::vec3 pos) : pos(pos), LightSource(intensity) {}
};

struct DirectionalSource : LightSource {
	glm::vec3 direction;

	DirectionalSource(GLfloat intensity, glm::vec3 direction) : direction(direction), LightSource(intensity) {}
};

struct SpotlightSource : LightSource {
	glm::vec3 pos;
	glm::vec3 direction;
	GLfloat cone;

	SpotlightSource(GLfloat intensity, glm::vec3 pos, glm::vec3 direction, GLfloat cone) : pos(pos), direction(direction), cone(cone), LightSource(intensity) {}
};

class PainterState {
public:
	PainterState(Camera camera) : camera(camera) {
		//TODO: сделать список всех моделей, в модель добавить поле title, записать в словарь все затенения для всех имён
		shadings["Cossak"] = Shading::Phong;
		shadings["LizardMK1"] = Shading::Toon;
	}
	std::string centralPath = "";
	std::string satellitePath = "";
	Camera camera;
	Model* centralModel = nullptr;
	Model* satelliteModel = nullptr;
	
	PointSource pointSource = PointSource(1.0f, glm::vec3(0,0,0));
	DirectionalSource directionalSource = DirectionalSource(1.0f, glm::vec3(1, 1, 1));
	SpotlightSource spotlightSource = SpotlightSource(1.0f, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 45.0f);

	std::unordered_map<std::string, int> shadings = std::unordered_map<std::string, int>();
};
