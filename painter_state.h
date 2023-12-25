#pragma once
#include <unordered_map>
#include <GL/glew.h>
#include "camera.h"
#include "model.h"
#include "light_sources.h"

enum Shading : int {
	Phong,
	Toon,
	Other
};

class PainterState {
public:
	PainterState(Camera camera) : camera(camera) {
		//TODO: ������� ������ ���� �������, � ������ �������� ���� title, �������� � ������� ��� ��������� ��� ���� ���
		shadings["Cossak"] = Shading::Phong;
		shadings["LizardMK1"] = Shading::Toon;
	}
	std::string path = "";

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
	SpotlightSource spotlightSource = SpotlightSource(1.0f, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 45.0f);

	std::unordered_map<std::string, int> shadings = std::unordered_map<std::string, int>();
};
