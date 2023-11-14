#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class Light
{
private:
	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::mat4 m_worldToClip;

	glm::vec3 m_pos;

	void updateMatrices(glm::vec3, glm::vec3) noexcept;

public:
	Light() = default;
	Light(const Light&);
	Light(Light&&) noexcept;
	~Light() {}

	Light(float left, float right, float bottom, float top, float near, float far, glm::vec3 pos);
	Light(float fov, float aspect, float near, float far, glm::vec3 pos);

	void setPosition(glm::vec3 pos) noexcept;
	void setProjection(float left, float right, float bottom, float top, float near, float far) noexcept;
	void setProjection(float fov, float aspect, float near, float far) noexcept; 
	void setProjection(glm::mat4 proj) noexcept;
	void setView(const glm::vec3& center, const glm::vec3& up) noexcept;

	glm::mat4 getWorldToClip() const noexcept;
};