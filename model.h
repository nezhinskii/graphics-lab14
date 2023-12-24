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

#include "lib/stb_image.h";

#include <iostream>;
#include <vector>

struct ObjVertex {
	glm::vec3 coords;
	glm::vec2 textCoords;

	ObjVertex(aiVector3D aiCoords, aiVector3D aiTextCoords) :
		coords(aiCoords.x, aiCoords.y, aiCoords.z),
		textCoords(aiTextCoords.x, aiTextCoords.y) 
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

		// textCoords
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ObjVertex), (GLvoid*)offsetof(ObjVertex, textCoords));
		glEnableVertexAttribArray(1);

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
			
		for (GLuint i = 0; i < mesh->mNumVertices; ++i) {
			ObjVertex vertex(mesh->mVertices[i], mesh->mTextureCoords[0][i]);
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


	void Draw(const GLuint& shaderId, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
		glUniform1i(glGetUniformLocation(shaderId, "numTextures"), textures.size());

		for (int i = 0; i < textures.size(); ++i) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glUniform1i(glGetUniformLocation(shaderId, ("textures" + std::to_string(i)).c_str()), i);
		}

		glBindVertexArray(VAO);
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shaderId, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

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

	void Draw(const GLuint& shaderId, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) {
		for (int i = 0; i < meshes.size(); ++i) {
			meshes[i].Draw(shaderId, model, view, projection);
		}
	}
};