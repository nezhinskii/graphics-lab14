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
		yAngle += 0.005;
		baseOrbitDeegre += 1;
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f));
		rotationMatrix = glm::rotate(glm::mat4(1.0f), yAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 centralModel = scaleMatrix * rotationMatrix * glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
		if (state.centralModel != nullptr) {
			(state.centralModel->Draw(Programs[0], centralModel, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
		}
		glm::vec3 satelitePosition(orbitRadius, 0.0f, 0.0f);
		if (state.satelliteModel != nullptr) {
			glm::vec3 position(orbitRadius, 0.0f, 0.0f);
			GLfloat deegreeStep = 360 / sateliteNum;

			for (int i = 0; i < sateliteNum; ++i)
			{
				glm::mat4 sateliteModel = scaleMatrix * rotationMatrix * glm::rotate(glm::mat4(1.0f), deegressToRadians(90), glm::vec3(-1.0f, 0.0f, 0.0f));
				glm::mat4 orbitMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(baseOrbitDeegre + i * deegreeStep), glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), position);
				sateliteModel = orbitMatrix * translateMatrix * sateliteModel;
				(state.satelliteModel->Draw(Programs[0], sateliteModel, state.camera.getViewMatrix(), state.camera.getProjectionMatrix()));
			}
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
