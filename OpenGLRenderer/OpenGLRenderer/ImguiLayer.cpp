#include "ImguiLayer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <string>
#include <gtc/type_ptr.hpp>

ImguiLayer::ImguiLayer()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
}

void ImguiLayer::Init(GLFWwindow* wnd) const
{
	ImGui_ImplGlfw_InitForOpenGL(wnd, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void ImguiLayer::NewFrame() const
{
	ImGui_ImplOpenGL3_NewFrame(); 
	ImGui_ImplGlfw_NewFrame(); 
	ImGui::NewFrame(); 
}

void ImguiLayer::Render() const noexcept
{
	ImGui::Render(); 
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
}

void ImguiLayer::Shutdown() const noexcept
{
	ImGui_ImplOpenGL3_Shutdown(); 
	ImGui_ImplGlfw_Shutdown(); 
	ImGui::DestroyContext();
}

void ImguiLayer::ShowFPS(float deltaTime, int prevFPS) const noexcept
{
	ImGui::Begin("Perfomance stats");
	ImGui::Text((std::string("Delta Time: ") + std::to_string(deltaTime)).c_str());
	ImGui::Text((std::string("FPS: ") + std::to_string(prevFPS)).c_str());
	ImGui::End();
}

void ImguiLayer::ShowDirLightProperties(glm::vec3& lightAmbient, glm::vec3& lightDiffuse,
	glm::vec3& lightSpecular, glm::vec3& lightPos) noexcept
{
	ImGui::Begin("Directional light");
	ImGui::ColorEdit3("Ambient", glm::value_ptr(lightAmbient)); 
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(lightDiffuse)); 
	ImGui::ColorEdit3("Specular", glm::value_ptr(lightSpecular)); 
	ImGui::InputFloat3("Position", glm::value_ptr(lightPos)); 
	ImGui::End();
}
