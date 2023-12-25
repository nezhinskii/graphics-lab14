#pragma once

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include "painter_state.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include<filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace sf;

class Painter {

	const static GLuint shadersNumber = 3;

	GLuint Programs[shadersNumber];


	bool readFile(const std::string& filename, std::string& content) {
		std::ifstream file(filename);

		if (!file.is_open()) {
			std::cerr << "Unable to open the file: " << filename << std::endl;
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		content = buffer.str();

		file.close();

		return true;
	}

	void ShaderLog(unsigned int shader)
	{
		int infologLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
		if (infologLen > 1)
		{
			int charsWritten = 0;
			std::vector<char> infoLog(infologLen);
			glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
			std::cout << "InfoLog: " << infoLog.data() << std::endl;
		}
	}

	void InitShader() {
		std::string content;
		const char* cstrContent;

		GLuint vShaders[shadersNumber];

		readFile("shaders/shader.vert", content);
		cstrContent = content.c_str();
		for (int i = 0; i < shadersNumber; i++) {
			vShaders[i] = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vShaders[i], 1, &cstrContent, NULL);

			glCompileShader(vShaders[i]);
			std::cout << "vertex shader" << i << std::endl;
			ShaderLog(vShaders[i]);
		}

		GLuint fShaders[shadersNumber];

		for (int i = 0; i < shadersNumber; i++) {
			readFile("shaders/shader" + std::to_string(i + 1) + ".frag", content);
			std::cout << "shaders/shader" + std::to_string(i + 1) + ".frag" << std::endl;
			cstrContent = content.c_str();
			fShaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fShaders[i], 1, &cstrContent, NULL);
			glCompileShader(fShaders[i]);
			std::cout << "fragment shader" << i << std::endl;
			ShaderLog(fShaders[i]);
		}


		for (int i = 0; i < shadersNumber; i++) {
			Programs[i] = glCreateProgram();
			glAttachShader(Programs[i], vShaders[i]);
			glAttachShader(Programs[i], fShaders[i]);
			glLinkProgram(Programs[i]);
			int link_ok;
			glGetProgramiv(Programs[i], GL_LINK_STATUS, &link_ok);
			if (!link_ok) {
				std::cout << "error attach shaders \n";
				return;
			}
		}
	}

	/*void InitShader() {
		GLuint vShaders[shadersNumber];
		std::filesystem::path currentPath = std::filesystem::current_path();
		std::cout << "Current Path: " << currentPath.string() << std::endl;

		std::string  vertShaderText = get_file_contents((currentPath.string() + "\\shaders\\shader.vert").c_str());
		const char* vertShader = vertShaderText.c_str();
		for (int i = 0; i < shadersNumber; i++) {
			vShaders[i] = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vShaders[i], 1, &vertShader, NULL);

			glCompileShader(vShaders[i]);
			std::cout << "vertex shader" << i << std::endl;
			ShaderLog(vShaders[i]);
		}

		GLuint fShaders[shadersNumber];
		std::string  fragShaderText = get_file_contents((currentPath.string() + "\\shaders\\shader.frag").c_str());
		const char* fragShader = vertShaderText.c_str();
		for (int i = 0; i < shadersNumber; i++) {
			fShaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fShaders[i], 1, &fragShader, NULL);
			glCompileShader(fShaders[i]);
			std::cout << "fragment shader" << i << std::endl;
			ShaderLog(fShaders[i]);
		}


		for (int i = 0; i < shadersNumber; i++) {
			Programs[i] = glCreateProgram();
			glAttachShader(Programs[i], vShaders[i]);
			glAttachShader(Programs[i], fShaders[i]);
			glLinkProgram(Programs[i]);
			int link_ok;
			glGetProgramiv(Programs[i], GL_LINK_STATUS, &link_ok);
			if (!link_ok) {
				std::cout << "error attach shaders \n";
				return;
			}
		}
	}*/

	void ReleaseShader() {
		glUseProgram(0);
		for (int i = 0; i < shadersNumber; i++) {
			glDeleteProgram(Programs[i]);
		}
	}

	GLfloat deegressToRadians(GLfloat deegres) {
		return deegres * 3.141592f / 180.0f;
	}

	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), yAngle, glm::vec3(1.0f, 0.5f, 0.0f));

public:
	Painter(PainterState& painterState) : state(painterState) {}

	PainterState state;

	GLuint sateliteNum = 10;
	GLfloat yAngle = 0.0f;
	GLfloat baseOrbitDeegre = 0.0f;
	GLfloat orbitRadius = 5.0f;

	void Draw() {
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		
		if (state.platform != nullptr) {
			glUseProgram(Programs[state.platform->shading]);
			glm::mat4 platformMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f));
			(state.platform->Draw(Programs[0], platformMat,  state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.lizardMk != nullptr) {
			glUseProgram(Programs[state.lizardMk->shading]);
			glm::mat4 lizardMkMat = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.15f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(15), glm::vec3(0.0f, 1.0f, 0.0f));
			(state.lizardMk->Draw(Programs[0], lizardMkMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		/*if (state.warrior != nullptr) {
			glUseProgram(Programs[state.warrior->shading]);
			glm::mat4 warriorMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.15f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(0.0f, -1.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
			(state.warrior->Draw(Programs[0], warriorMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}*/

		if (state.warrior != nullptr) {
			glUseProgram(Programs[state.warrior->shading]);
			glm::mat4 warriorMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.2f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(0.0f, -1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.28f));
			(state.warrior->Draw(Programs[0], warriorMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.gun != nullptr) {
			glUseProgram(Programs[state.gun->shading]);
			glm::mat4 gunMat = glm::translate(glm::mat4(1.0f), glm::vec3(-2.53f, 0.89f, 0.40f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(-18), glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(-15), glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(60), glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(180), glm::vec3(-1.0f, 0.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.70f));
			(state.gun->Draw(Programs[0], gunMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.table != nullptr) {
			glUseProgram(Programs[state.table->shading]);
			glm::mat4 tableMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.15f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(1.35f));
			(state.table->Draw(Programs[0], tableMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		if (state.coffee != nullptr) {
			glUseProgram(Programs[state.coffee->shading]);
			glm::mat4 coffeeMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.22f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.003f));
			(state.coffee->Draw(Programs[0], coffeeMat, state.camera, state.pointSource, state.spotlightSource, state.directionalSource));
			glUseProgram(0);
		}

		glUseProgram(0);
	}

	void Init() {
		glewInit();
		InitShader();
	}

	void Release() {
		ReleaseShader();
	}

};
