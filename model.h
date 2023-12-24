#pragma once
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "lib/stb_image.h";

#include <iostream>;
#include <vector>

#include "camera.h";
#include "light_sources.h"

struct ObjVertex {
	glm::vec3 coords;
	glm::vec3 color;
	glm::vec2 textCoords;
	glm::vec3 normal;

	ObjVertex(aiVector3D aiCoords, aiColor3D color, aiVector3D aiTextCoords, aiVector3D normal) :
		coords(aiCoords.x, aiCoords.y, aiCoords.z),
		color(color.r, color.g, color.b),
		textCoords(aiTextCoords.x, aiTextCoords.y),
		normal(normal.x, normal.y, normal.z)
	{}
};

class Mesh {
	std::vector<ObjVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<GLuint> textures;
	GLuint VBO, EBO;

	void loadTexture(const char* texturePath, GLuint& textureID) {
		int width, height, channels;
		unsigned char* image = stbi_load(texturePath, &width, &height, &channels, STBI_rgb);

		if (!image) {
			std::cerr << "Failed to load texture: " << texturePath << std::endl;
			return;
		}

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(image);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void setupBuffers() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ObjVertex), &vertices[0], GL_STATIC_DRAW);

		// coords
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)offsetof(ObjVertex, color));
		glEnableVertexAttribArray(1);

		// textCoords
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)offsetof(ObjVertex, textCoords));
		glEnableVertexAttribArray(2);

		// normals
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)offsetof(ObjVertex, normal));
		glEnableVertexAttribArray(3);

		glBindVertexArray(0);
	}

public:
	GLuint VAO;

	Mesh(aiMesh* mesh, aiMaterial*  material, const std::string& modelDirectory) {
		material->mProperties;
		for (GLuint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j) {
			aiTextureType textureType = static_cast<aiTextureType>(j);
			aiString texturePath;
			if (material->GetTexture(textureType, 0, &texturePath) == AI_SUCCESS) {
				std::string fullPath = modelDirectory + '\\' + texturePath.C_Str();
				GLuint textureID;
				loadTexture(fullPath.c_str(), textureID);
				textures.push_back(textureID);
			}
		}

		aiColor3D color(1.f, 1.f, 1.f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		for (GLuint i = 0; i < mesh->mNumVertices; ++i) {

			ObjVertex vertex(
				mesh->mVertices[i],
				color,
				mesh->mTextureCoords[0][i],
				mesh->mNormals[i]
			);
			vertices.push_back(vertex);
		}

		for (unsigned int j = 0; j < mesh->mNumFaces; ++j) {
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; ++k) {
				indices.push_back(face.mIndices[k]);
			}
		}

		setupBuffers();
	}


	void Draw(const GLuint& shaderId, const glm::mat4& model, const Camera& camera, const PointSource& pSource, const SpotlightSource& sSource, const DirectionalSource& dSource) {
		glUniform1i(glGetUniformLocation(shaderId, "numTextures"), textures.size());

		for (int i = 0; i < textures.size(); ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glUniform1i(glGetUniformLocation(shaderId, ("textures" + std::to_string(i)).c_str()), i);
		}

		glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		glBindVertexArray(VAO);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));

		glUniform4f(glGetUniformLocation(shaderId, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(shaderId, "camPos"), camera.position.x, camera.position.y, camera.position.z);

		glUniform1f(glGetUniformLocation(shaderId, "pSource.intensity"), pSource.intensity);
		glUniform3fv(glGetUniformLocation(shaderId, "pSource.pos"), 1, glm::value_ptr(pSource.pos));

		glUniform1f(glGetUniformLocation(shaderId, "sSource.intensity"), sSource.intensity);
		glUniform3fv(glGetUniformLocation(shaderId, "sSource.pos"), 1, glm::value_ptr(sSource.pos));
		glUniform3fv(glGetUniformLocation(shaderId, "sSource.direction"), 1, glm::value_ptr(sSource.direction));
		glUniform1f(glGetUniformLocation(shaderId, "sSource.cone"), sSource.cone);

		glUniform1f(glGetUniformLocation(shaderId, "dSource.intensity"), dSource.intensity);
		glUniform3fv(glGetUniformLocation(shaderId, "dSource.direction"), 1, glm::value_ptr(dSource.direction));

		glDrawElements(GL_TRIANGLES, static_cast<GLuint>(indices.size()), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
};

class Model {
	std::vector<Mesh> meshes;
public:
	Model(const std::string& path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
			return;
		}

		std::string modelDirectory = path;
		modelDirectory = modelDirectory.substr(0, modelDirectory.find_last_of('\\'));

		for (GLuint i = 0; i < scene->mNumMeshes; ++i) {
			Mesh mesh(scene->mMeshes[i], scene->mMaterials[scene->mMeshes[i]->mMaterialIndex], modelDirectory);
			meshes.push_back(mesh);
		}
	}

	void Draw(const GLuint& shaderId, const glm::mat4& model, const Camera& camera, const PointSource& pSource, const SpotlightSource& sSource, const DirectionalSource& dSource) {
		for (int i = 0; i < meshes.size(); ++i) {
			meshes[i].Draw(shaderId, model, camera, pSource, sSource, dSource);
		}
	}
};