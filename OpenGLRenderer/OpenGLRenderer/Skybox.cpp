#include "Skybox.h"

#include "ResourceHelpers.h"

Skybox::Skybox()
{
	m_shader = std::make_unique<Shader>("Shaders/Skybox.vs", "Shaders/Skybox.fs");

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	m_cubeFaces = {
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/front.jpg",
		"resources/textures/skybox/back.jpg"
	};
	m_texture = loadCubemap(m_cubeFaces);
}

Skybox::Skybox(Skybox&& other) noexcept
{
	m_shader = std::move(other.m_shader);
	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
	m_texture = other.m_texture;
}

void Skybox::Draw(const glm::mat4& proj, const glm::mat4& view)
{
	glDepthMask(GL_FALSE); 
	m_shader->use(); 
	m_shader->setMat4("projection", proj);
	m_shader->setMat4("view", view); 

	glBindVertexArray(m_VAO); 
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture); 
	glDrawArrays(GL_TRIANGLES, 0, 36); 
	glDepthMask(GL_TRUE); 
}
