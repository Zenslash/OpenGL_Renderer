#pragma once

#include <GLFW/glfw3.h>
#include <glm.hpp>

class ImguiLayer
{
public:
	ImguiLayer();

	void Init(GLFWwindow* wnd) const;
	void NewFrame() const;
	void Render() const noexcept;
	void Shutdown() const noexcept;

	void ShowFPS(float deltaTime, int prevFPS) const noexcept;
	void ShowDirLightProperties(glm::vec3& lightAmbient, glm::vec3& lightDiffuse,
		glm::vec3& lightSpecular, glm::vec3& lightPos) noexcept;

private:
};