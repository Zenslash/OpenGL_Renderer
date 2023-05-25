#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "stb_image.h"
#include "Camera.h"
#include "Model.h"
#include <filesystem>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
}

bool cursorDisabled = false;
void process_input(GLFWwindow* wnd, Camera* camera, float deltatTime)
{
	if(glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(wnd, true);
	}
	if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->ProcessMovementInput(Camera_Movement::FORWARD, deltatTime);
	}
	if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->ProcessMovementInput(Camera_Movement::BACKWARD, deltatTime);
	}
	if (glfwGetKey(wnd, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->ProcessMovementInput(Camera_Movement::RIGHT, deltatTime);
	}
	if (glfwGetKey(wnd, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->ProcessMovementInput(Camera_Movement::LEFT, deltatTime);
	}
	if (glfwGetKey(wnd, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (cursorDisabled)
		{
			glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		cursorDisabled = !cursorDisabled;
	}
}

float lastX = 400;
float lastY = 300;
bool firstMouseInput = true;
Camera camera;

void mouse_callback(GLFWwindow* wnd, double xpos, double ypos)
{
	if (firstMouseInput)
	{
		firstMouseInput = false;
		lastX = xpos;
		lastY = ypos;
	}

	if (!cursorDisabled)
	{
		lastX = xpos;
		lastY = ypos;
		return;
	}

	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

unsigned int loadTexture(const char* path);

int main()
{
	constexpr  int width = 1024;
	constexpr int height = 720;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* wnd = glfwCreateWindow(width, height, "OpenGL_Renderer", nullptr, nullptr);
	if(wnd == nullptr)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(wnd);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialized GLAD" << std::endl;
		return -1;
	}

	//Setup viewport
	glViewport(0, 0, width, height);
	glfwSetFramebufferSizeCallback(wnd, framebuffer_size_callback);
	glfwSetCursorPosCallback(wnd, mouse_callback);

	//Configuring depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Compile shaders
	Shader litShader("Shaders/SimpleLit.vs", "Shaders/SimpleLit.fs");
	//TODO There is some problem with shader compilation
	Shader vegetationShader("Shaders/VegetationTransparent.vs", "Shaders/VegetationTransparent.fs");
	Shader lightSrcShader("Shaders/LightSource.vs", "Shaders/LightSource.fs");
	Shader postProcessShader("Shaders/PostProcess.vs", "Shaders/PostProcess.fs");


	//Light properties
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 lightAmbient = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	float lightIntensity = 1.0f;

	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
	};

	const std::filesystem::path workDir = std::filesystem::current_path();

	 std::filesystem::path modelPath = workDir / "resources" / "models" / "soldier" / "CloneDC15sWhite.obj";
	Model soldier(modelPath.generic_string().c_str());

	modelPath = workDir / "resources" / "models" / "terrain" / "terrain.obj";
	Model floor(modelPath.generic_string().c_str());

	modelPath = workDir / "resources" / "models" / "grass" / "plane.obj";
	Model grass(modelPath.generic_string().c_str());


	//Utility
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	//Uniform vars
	litShader.use();
	litShader.setVec3("_Material.ambient", 0.2f, 0.2f, 0.2f);
	litShader.setVec3("_Material.texture_diffuse1", 1.0f, 1.0f, 1.0f);
	litShader.setVec3("_Material.texture_specular1", 0.5f, 0.5f, 0.5f);
	litShader.setFloat("_Material.shiness", 32.0f);
	/*litShader.setFloat("_PointLights[0].constant", 1.0f);
	litShader.setFloat("_PointLights[0].linear", 0.09f);
	litShader.setFloat("_PointLights[0].quadratic", 0.032f);
	litShader.setVec3("_PointLights[0].position", pointLightPositions[0]);
	litShader.setVec3("_PointLights[0].ambient", lightAmbient);
	litShader.setVec3("_PointLights[0].diffuse", lightDiffuse);
	litShader.setVec3("_PointLights[0].specular", lightSpecular);
	litShader.setFloat("_PointLights[0].intensity", lightIntensity);

	litShader.setFloat("_PointLights[1].constant", 1.0f);
	litShader.setFloat("_PointLights[1].linear", 0.09f);
	litShader.setFloat("_PointLights[1].quadratic", 0.032f);
	litShader.setVec3("_PointLights[1].position", pointLightPositions[1]);
	litShader.setVec3("_PointLights[1].ambient", lightAmbient);
	litShader.setVec3("_PointLights[1].diffuse", lightDiffuse);
	litShader.setVec3("_PointLights[1].specular", lightSpecular);
	litShader.setFloat("_PointLights[1].intensity", lightIntensity);

	litShader.setFloat("_PointLights[2].constant", 1.0f);
	litShader.setFloat("_PointLights[2].linear", 0.09f);
	litShader.setFloat("_PointLights[2].quadratic", 0.032f);
	litShader.setVec3("_PointLights[2].position", pointLightPositions[2]);
	litShader.setVec3("_PointLights[2].ambient", lightAmbient);
	litShader.setVec3("_PointLights[2].diffuse", lightDiffuse);
	litShader.setVec3("_PointLights[2].specular", lightSpecular);
	litShader.setFloat("_PointLights[2].intensity", lightIntensity);

	litShader.setFloat("_PointLights[3].constant", 1.0f);
	litShader.setFloat("_PointLights[3].linear", 0.09f);
	litShader.setFloat("_PointLights[3].quadratic", 0.032f);
	litShader.setVec3("_PointLights[3].position", pointLightPositions[3]);
	litShader.setVec3("_PointLights[3].ambient", lightAmbient);
	litShader.setVec3("_PointLights[3].diffuse", lightDiffuse);
	litShader.setVec3("_PointLights[3].specular", lightSpecular);
	litShader.setFloat("_PointLights[3].intensity", lightIntensity);*/
	litShader.setInt("albedo", 0);

	//Spot light
	litShader.setVec3("_SpotLight.position", camera.cameraPos);
	litShader.setVec3("_SpotLight.direction", camera.cameraFront);
	litShader.setFloat("_SpotLight.cutOff", glm::cos(glm::radians(12.5f)));
	litShader.setFloat("_SpotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	litShader.setVec3("_SpotLight.ambient", lightAmbient);
	litShader.setVec3("_SpotLight.diffuse", lightDiffuse);
	litShader.setVec3("_SpotLight.specular", lightSpecular);
	litShader.setFloat("_SpotLight.intensity", lightIntensity);

	postProcessShader.use();
	postProcessShader.setInt("_screenTexture", 0);

	//Framebuffer goes here
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	//glBindRenderbuffer(GL_RENDERBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer object is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Camera stuff
	glm::vec3 up = glm::vec3(0.0, 1.0f, 0.0f);
	
	//Timer stuff
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	//Imgui setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(wnd, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	//Game loop
	while(!glfwWindowShouldClose(wnd))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(wnd, &camera, deltaTime);

		//Rendering here
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		////////////////////////////////////////////////////////////
		//First pass
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render light source
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		lightSrcShader.use();
		lightSrcShader.setMat4("view", camera.GetViewMatrix());
		lightSrcShader.setMat4("projection", projection);
		lightSrcShader.setVec3("_LightColor", lightDiffuse);


		/*for (int i = 0; i < 4; i++)
		{
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, pointLightPositions[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.5f, 0.5f, 0.5f));

			lightSrcShader.setMat4("model", lightModel);
			pointLight.Draw(lightSrcShader);
		}*/
		
		//Render models
		litShader.use();

		litShader.setMat4("view", camera.GetViewMatrix());
		litShader.setMat4("projection", projection);
		litShader.setVec3("_ViewPos", camera.cameraPos);
		//litShader.setVec3("_Light.position", lightModel * glm::vec4(lightPos, 1.0));
		litShader.setVec3("_Light.position", camera.cameraPos);

		//Directiona light uniforms
		litShader.setVec3("_DirLight.direction", -0.2f, -1.0f, -0.3f);
		litShader.setVec3("_DirLight.ambient", lightAmbient);
		litShader.setVec3("_DirLight.diffuse", lightDiffuse);
		litShader.setVec3("_DirLight.specular", lightSpecular);

		litShader.setVec3("_SpotLight.position", camera.cameraPos);
		litShader.setVec3("_SpotLight.direction", camera.cameraFront);

		litShader.setInt("_Material.diffuse", 0);
		litShader.setInt("_Material.specular", 1);

		// 4. use our shader program when we want to render an object

		glm::mat4 local = glm::mat4(1.0f);
		local = glm::rotate(local, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		litShader.setMat4("model", local);
		//model.Draw(litShader);

		soldier.Draw(litShader);

		local = glm::mat4(1.0f);
		litShader.setMat4("model", local);
		floor.Draw(litShader);

		vegetationShader.use();

		vegetationShader.setMat4("view", camera.GetViewMatrix());
		vegetationShader.setMat4("projection", projection);
		vegetationShader.setVec3("_ViewPos", camera.cameraPos);

		local = glm::mat4(1.0f);
		local = glm::translate(local, glm::vec3(1.0f, -1.0f, 2.0f));
		vegetationShader.setMat4("model", local);
		grass.Draw(vegetationShader);

		////////////////////////////////////////////////////////////
		//Second pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		postProcessShader.use();
		glBindVertexArray(quadVAO);
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);



		//ImGui
		ImGui::Begin("Perfomance stats");
		ImGui::Text((std::string("Delta Time: ") + std::to_string(deltaTime)).c_str());
		ImGui::End();

		ImGui::Begin("Directional light");
		ImGui::ColorEdit3("Ambient", glm::value_ptr(lightAmbient));
		ImGui::ColorEdit3("Diffuse", glm::value_ptr(lightDiffuse));
		ImGui::ColorEdit3("Specular", glm::value_ptr(lightSpecular));
		ImGui::InputFloat3("Position", glm::value_ptr(lightPos));
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &fbo);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}