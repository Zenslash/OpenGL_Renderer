#pragma once

#include <memory>
#include <vector>
#include <string>
#include "Shader.h"

//TODO Reconsider later
class Skybox
{
public:
	Skybox();
	Skybox(Skybox& other) = delete;
	Skybox(Skybox&& other) noexcept;

	~Skybox() = default;

	void Draw(const glm::mat4& proj, const glm::mat4& view);

private:
	std::unique_ptr<Shader>		m_shader;
	uint32_t					m_VAO;
	uint32_t					m_VBO;
	uint32_t					m_texture;

	std::vector<std::string>	m_cubeFaces;
};

