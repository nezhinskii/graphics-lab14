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


using namespace sf;

class Painter {

	GLuint Programs[1];

	const static GLuint shadersNumber = 1;

	const char* VertexShaderSource[shadersNumber] = {
		R"(
		#version 330 core

		layout (location = 0) in vec3 position;
		layout (location = 1) in vec2 texCoord;

		out vec2 textureCoord;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		void main() {
			gl_Position = projection * view * model * vec4(position, 1.0);
			textureCoord = texCoord;
		}
		)"
	};

	const char* FragShaderSources[shadersNumber] = {
		R"(
		#version 330 core

		in vec2 textureCoord;

		out vec4 fragColor;

		uniform sampler2D textures0;
		uniform sampler2D textures1;
		uniform sampler2D textures2;
		uniform sampler2D textures3;
		uniform sampler2D textures4;
		uniform sampler2D textures5;
		uniform sampler2D textures6;
		uniform sampler2D textures7;

		uniform int numTextures;

		void main() {
			vec4 finalColor = vec4(1.0);
			
			if (numTextures > 0){
				finalColor *= texture(textures0, textureCoord);
			}
			if (numTextures > 1){
				finalColor *= texture(textures1, textureCoord);
			}
			if (numTextures > 2){
				finalColor *= texture(textures2, textureCoord);
			}
			if (numTextures > 3){
				finalColor *= texture(textures3, textureCoord);
			}
			if (numTextures > 4){
				finalColor *= texture(textures4, textureCoord);
			}
			if (numTextures > 5){
				finalColor *= texture(textures5, textureCoord);
			}
			if (numTextures > 6){
				finalColor *= texture(textures6, textureCoord);
			}
			if (numTextures > 7){
				finalColor *= texture(textures7, textureCoord);
			}

			fragColor = finalColor;
		}
		)"
	};

	std::string get_file_contents(const char* filename)
	{
		std::ifstream file(filename);

		if (!file.is_open()) {
			std::cerr << "Error opening file: " << filename << std::endl;
			return ""; 
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
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
		GLuint vShaders[shadersNumber];

		for (int i = 0; i < shadersNumber; i++) {
			vShaders[i] = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vShaders[i], 1, &(VertexShaderSource[i]), NULL);

			glCompileShader(vShaders[i]);
			std::cout << "vertex shader" << i << std::endl;
			ShaderLog(vShaders[i]);
		}

		GLuint fShaders[shadersNumber];

		for (int i = 0; i < shadersNumber; i++) {
			fShaders[i] = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fShaders[i], 1, &(FragShaderSources[i]), NULL);
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
		glUseProgram(Programs[0]);
		
		if (state.platform != nullptr) {
			glm::mat4 platformMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f));
			(state.platform->Draw(Programs[0], platformMat, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
		}

		if (state.lizardMk != nullptr) {
			glm::mat4 lizardMkMat = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.15f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(15), glm::vec3(0.0f, 1.0f, 0.0f));
			(state.lizardMk->Draw(Programs[0], lizardMkMat, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
		}

		if (state.kazak != nullptr) {
			glm::mat4 kazakMat = 
				glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 1.2f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(0.0f, -1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f)) 
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.28f));
			(state.kazak->Draw(Programs[0], kazakMat, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
		}

		if (state.gun != nullptr) {
			glm::mat4 gunMat = glm::translate(glm::mat4(1.0f), glm::vec3(-2.53f, 0.89f, 0.40f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(-18), glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(-15), glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(60), glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(180), glm::vec3(-1.0f, 0.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.70f));
			(state.gun->Draw(Programs[0], gunMat, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
		}

		if (state.table != nullptr) {
			glm::mat4 tableMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.17f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
			(state.table->Draw(Programs[0], tableMat, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
		}

		if (state.coffee != nullptr) {
			glm::mat4 coffeeMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.15f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(0.003f));
			(state.coffee->Draw(Programs[0], coffeeMat, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
		}

		if (state.test != nullptr) {
			glm::mat4 testMat = glm::mat4(1.0f);
			(state.test->Draw(Programs[0], testMat, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
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
