#include "Light.h"

void Light::updateMatrices(glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, -1.0f, 0.0f)) noexcept
{
	//For some reason I need use reversed world up vector
	m_view = glm::lookAt(m_pos, center, up);
	m_worldToClip = m_projection * m_view;
}

Light::Light(const Light& l)
{
	m_pos = l.m_pos;
	m_projection = l.m_projection;
	m_view = l.m_view;
	m_worldToClip = l.m_worldToClip;
}

Light::Light(Light&& l) noexcept
{
	m_pos = std::move(l.m_pos);
	m_projection = std::move(l.m_projection);
	m_view = std::move(l.m_view);
	m_worldToClip = std::move(l.m_worldToClip);
}

Light::Light(float left, float right, float bottom, float top, float near, float far, glm::vec3 pos)
{
	m_pos = pos;
	m_projection = glm::ortho(left, right, bottom, top, near, far); 
	updateMatrices();
}

Light::Light(float fov, float aspect, float near, float far, glm::vec3 pos) 
{
	m_pos = pos;
	m_projection = glm::perspective(fov, aspect, near, far);
	updateMatrices();
}

void Light::setPosition(glm::vec3 pos) noexcept
{
	m_pos = pos;
	updateMatrices();
}

void Light::setProjection(float left, float right, float bottom, float top, float near, float far) noexcept
{
	m_projection = glm::ortho(left, right, bottom, top, near, far);
	updateMatrices();
}

void Light::setProjection(float fov, float aspect, float near, float far) noexcept
{
	m_projection = glm::perspective(fov, aspect, near, far);
	updateMatrices();
}

void Light::setProjection(glm::mat4 proj) noexcept
{
	m_projection = proj;
	updateMatrices();
}

void Light::setView(const glm::vec3& center, const glm::vec3& up) noexcept
{
	updateMatrices(center, up);
}

glm::mat4 Light::getWorldToClip() const noexcept
{
	return m_worldToClip; 
}
