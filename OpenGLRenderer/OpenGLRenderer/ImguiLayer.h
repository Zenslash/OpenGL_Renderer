#pragma once

#include <GLFW/glfw3.h>
#include <glm.hpp>

class ImguiLayer
{
public:

	ImguiLayer();

	void init(GLFWwindow* wnd) noexcept;
	void newFrame() noexcept;
	void drawPerfomance(float delta, int fps) noexcept;
	void drawDirLightProperties(glm::vec3& ambient, glm::vec3& diffuse,
								glm::vec3& specular, glm::vec3& pos);
	void render() const noexcept;
	void shutdown() const noexcept;
private:

};