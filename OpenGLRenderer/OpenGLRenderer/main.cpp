#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include <glad/glad.h>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "stb_image.h"
#include "Camera.h"

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

	glEnable(GL_DEPTH_TEST);

	//TODO Load custom models
	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	//Gen buffers
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VAO, LightVAO;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &LightVAO);

	//unsigned int EBO;
	//glGenBuffers(1, &EBO);

	//Compile shaders
	Shader litShader("Shaders/SimpleLit.vs", "Shaders/SimpleLit.fs");
	Shader lightSrcShader("Shaders/LightSource.vs", "Shaders/LightSource.fs");

	//Bind buffers
	// 1. bind VAO
	glBindVertexArray(VAO);

	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 3. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Configuring light
	glBindVertexArray(LightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Configure textures
	unsigned int texID;

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int tWidth, tHeight, nrChannels;
	unsigned char* texData = stbi_load("E:/CG/Projects/OpenGL_Renderer/OpenGLRenderer/Resources/Textures/stone_albedo.jpg",
		&tWidth, &tHeight, &nrChannels, 0);

	if(texData != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load texture!" << std::endl;
	}

	stbi_image_free(texData);

	//Light properties
	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	litShader.use();
	litShader.setVec3("_ObjectColor", 1.0f, 0.5f, 0.31f);
	litShader.setVec3("_LightColor", lightColor);
	litShader.setInt("albedo", 0);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Render light source
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		glBindVertexArray(LightVAO);

		lightSrcShader.use();
		lightSrcShader.setMat4("view", camera.GetViewMatrix());
		lightSrcShader.setMat4("projection", projection);
		lightSrcShader.setVec3("_LightColor", lightColor);

		glm::mat4 lightModel = glm::mat4(1.0f);
		lightModel = glm::rotate(lightModel, glm::radians((float)sin(glfwGetTime()) * 120.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		lightModel = glm::translate(lightModel, lightPos);
		

		lightSrcShader.setMat4("model", lightModel);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		//Render cube
		litShader.use();
		
		litShader.setMat4("view", camera.GetViewMatrix());
		litShader.setMat4("projection", projection);
		litShader.setVec3("_LightPos", lightModel * glm::vec4(lightPos, 1.0));
		litShader.setVec3("_ViewPos", camera.cameraPos);
		litShader.setVec3("_LightColor", lightColor);

		// 4. use our shader program when we want to render an object
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		glBindVertexArray(VAO);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		litShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		////////////

		//ImGui
		ImGui::Begin("Perfomance stats");
		ImGui::Text((std::string("Delta Time: ") + std::to_string(deltaTime)).c_str());
		ImGui::End();

		ImGui::Begin("Directional light");
		ImGui::ColorEdit3("Color", glm::value_ptr(lightColor));
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &LightVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}