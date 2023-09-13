#include "ImguiLayer.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>

#include <gtc/type_ptr.hpp>

ImguiLayer::ImguiLayer()
{
}

void ImguiLayer::init(GLFWwindow* wnd) noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(wnd, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void ImguiLayer::newFrame() noexcept
{
	ImGui_ImplOpenGL3_NewFrame(); 
	ImGui_ImplGlfw_NewFrame(); 
	ImGui::NewFrame(); 
}

void ImguiLayer::drawPerfomance(float delta, int fps) noexcept
{
	ImGui::Begin("Perfomance stats");
	ImGui::Text((std::string("Delta Time: ") + std::to_string(delta)).c_str());
	ImGui::Text((std::string("FPS: ") + std::to_string(fps)).c_str());
	ImGui::End(); 
}

void ImguiLayer::drawDirLightProperties(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular, glm::vec3& pos)
{
	ImGui::Begin("Directional light");
	ImGui::ColorEdit3("Ambient", glm::value_ptr(ambient));
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(diffuse));
	ImGui::ColorEdit3("Specular", glm::value_ptr(specular));
	ImGui::InputFloat3("Position", glm::value_ptr(pos));
	ImGui::End();
}

void ImguiLayer::render() const noexcept
{
	ImGui::Render(); 
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 
}

void ImguiLayer::shutdown() const noexcept
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
