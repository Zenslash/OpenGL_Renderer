#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Shader.h"
#include "Mesh.h"
#include <vector>

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
	Model(const char* path)
	{
		loadModel(path);
	}
	
	void Draw(Shader& shader);
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> loaded_textures;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
												std::string typeName);
};