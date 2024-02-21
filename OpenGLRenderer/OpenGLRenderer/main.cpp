
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "stb_image.h"

#include "Camera.h"
#include "Entity.h"
#include "ImguiLayer.h"
#include "Light.h"
#include "Shader.h"
#include "Skybox.h"


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

void DrawGeometry(Entity& soldier, Entity& floor, Shader& litShader);
void DrawVegetation(Entity& grass, Shader& vegetationShader);


int main()
{
	constexpr  int width = 1920;
	constexpr int height = 1080;

	float rectangleVertices[] =
	{
		//Coords		//Tex coords
		1.0f, -1.0f,	1.0f, 0.0f,
		-1.0f, -1.0f,	0.0f, 0.0f,
		-1.0f, 1.0f,	0.0f, 1.0f,

		1.0f, 1.0f,		1.0f, 1.0f,
		1.0f, -1.0f,	1.0f, 0.0f,
		-1.0f, 1.0f,	0.0f, 1.0f
	};

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
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Compile shaders
	Shader litShader("Shaders/ShadowBlinnPhong.vs", "Shaders/ShadowBlinnPhong.fs");
	Shader vegetationShader("Shaders/VegetationTransparent.vs", "Shaders/VegetationTransparent.fs");
	Shader lightSrcShader("Shaders/LightSource.vs", "Shaders/LightSource.fs");
	Shader skyboxShader("Shaders/Skybox.vs", "Shaders/Skybox.fs");
	Shader envMappingShader("Shaders/EnvironmentMapping.vs", "Shaders/EnvironmentMapping.fs");
	Shader framebufferShader("Shaders/Framebuffer.vs", "Shaders/Framebuffer.fs");
	Shader depthShader("Shaders/SimpleDepthShader.vs", "Shaders/SimpleDepthShader.fs");

	framebufferShader.use();
	framebufferShader.setBool("screenTex", 0);


	//Light properties
	glm::vec3 lightPos = glm::vec3(1.2f, -4.0f, 2.0f);
	glm::vec3 lightAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	float lightIntensity = 1.0f;
	float pointLightIntensity = 2.0f;

	glm::vec3 pointLightPositions[] = {
	glm::vec3(-1.0f,  0.5f,  0.0f),
	glm::vec3(1.0f, 0.5f, 0.0f),
	glm::vec3(-4.0f,  0.5f, 0.0f),
	glm::vec3(-6.0f,  0.5f, 0.0f)
	};

	const std::filesystem::path workDir = std::filesystem::current_path();

	 std::filesystem::path modelPath = workDir / "resources" / "models" / "soldier" / "CloneDC15sWhite.obj";
	//Model soldier(modelPath.generic_string().c_str());
	Entity soldier(modelPath.generic_string().c_str());
	soldier.addChild(modelPath.generic_string().c_str());
	soldier.getChild(0)->transform.setLocalPos(glm::vec3(5.0f, 0.1f, 0.0f));
	soldier.updateSelfAndChild();

	modelPath = workDir / "resources" / "models" / "terrain" / "terrain.obj";
	Entity floor(modelPath.generic_string().c_str());

	modelPath = workDir / "resources" / "models" / "grass" / "plane.obj";
	Entity grass(modelPath.generic_string().c_str());

	//Rectangle VAO
	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);

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
	litShader.setFloat("_PointLights[0].constant", 1.0f);
	litShader.setFloat("_PointLights[0].linear", 0.09f);
	litShader.setFloat("_PointLights[0].quadratic", 0.032f);
	litShader.setVec3("_PointLights[0].position", pointLightPositions[0]);
	litShader.setVec3("_PointLights[0].ambient", lightAmbient);
	litShader.setVec3("_PointLights[0].diffuse", glm::vec3(0.0f, 1.0f, 1.0f));
	litShader.setVec3("_PointLights[0].specular", glm::vec3(0.0f, 1.0f, 1.0f));
	litShader.setFloat("_PointLights[0].intensity", pointLightIntensity);

	litShader.setFloat("_PointLights[1].constant", 1.0f);
	litShader.setFloat("_PointLights[1].linear", 0.09f);
	litShader.setFloat("_PointLights[1].quadratic", 0.032f);
	litShader.setVec3("_PointLights[1].position", pointLightPositions[1]);
	litShader.setVec3("_PointLights[1].ambient", lightAmbient);
	litShader.setVec3("_PointLights[1].diffuse", glm::vec3(0.0f, 0.0f, 1.0f));
	litShader.setVec3("_PointLights[1].specular", glm::vec3(0.0f, 0.0f, 1.0f));
	litShader.setFloat("_PointLights[1].intensity", pointLightIntensity);

	litShader.setFloat("_PointLights[2].constant", 1.0f);
	litShader.setFloat("_PointLights[2].linear", 0.09f);
	litShader.setFloat("_PointLights[2].quadratic", 0.032f);
	litShader.setVec3("_PointLights[2].position", pointLightPositions[2]);
	litShader.setVec3("_PointLights[2].ambient", lightAmbient);
	litShader.setVec3("_PointLights[2].diffuse", glm::vec3(1.0f, 0.0f, 1.0f));
	litShader.setVec3("_PointLights[2].specular", glm::vec3(1.0f, 0.0f, 1.0f));
	litShader.setFloat("_PointLights[2].intensity", pointLightIntensity);

	litShader.setFloat("_PointLights[3].constant", 1.0f);
	litShader.setFloat("_PointLights[3].linear", 0.09f);
	litShader.setFloat("_PointLights[3].quadratic", 0.032f);
	litShader.setVec3("_PointLights[3].position", pointLightPositions[3]);
	litShader.setVec3("_PointLights[3].ambient", lightAmbient);
	litShader.setVec3("_PointLights[3].diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
	litShader.setVec3("_PointLights[3].specular", glm::vec3(1.0f, 0.0f, 0.0f));
	litShader.setFloat("_PointLights[3].intensity", pointLightIntensity);
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

	//Skybox
	std::unique_ptr<Skybox> skybox = std::make_unique<Skybox>();

	//Camera stuff
	glm::vec3 up = glm::vec3(0.0, 1.0f, 0.0f);
	
	//Timer stuff
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	//Imgui setup
	ImguiLayer imgui;
	imgui.init(wnd);

	//Perfomance metrics
	int frameCount = 0;
	int prevFPS = 0;
	float prevTime = glfwGetTime();

	//Framebuffers
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	unsigned int framebufferTexture;
	glGenTextures(1, &framebufferTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer error: " << fboStatus << std::endl;
	}

	//Depth map framebuffer
	unsigned int depthFBO;
	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//Init depth cubemap
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Lights
	Light dirLight(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 8.5f, lightPos);

	//Game loop
	while(!glfwWindowShouldClose(wnd))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(wnd, &camera, deltaTime);

		//first pass
		//render depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0); 
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		//configure shaders
		depthShader.use();
		depthShader.setMat4("lightSpace", dirLight.getWorldToClip());

		//directional light pass
		glCullFace(GL_FRONT);
		DrawGeometry(soldier, floor, depthShader);
		glCullFace(GL_BACK);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//point light pass
		glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
		glClear(GL_DEPTH_BUFFER_BIT); 

		glCullFace(GL_FRONT); 
		//DrawGeometry(soldier, floor, depthShader);
		glCullFace(GL_BACK); 

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//render normal scene
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//Rendering here
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//Imgui
		imgui.newFrame();

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		//Render light source
		/*lightSrcShader.use();
		lightSrcShader.setMat4("view", camera.GetViewMatrix());
		lightSrcShader.setMat4("projection", projection);
		lightSrcShader.setVec3("_LightColor", lightDiffuse);*/

		/*
		for (int i = 0; i < 4; i++)
		{
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, pointLightPositions[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.5f, 0.5f, 0.5f));

			lightSrcShader.setMat4("model", lightModel);
			pointLight.Draw(lightSrcShader);
		}
		*/
		
		//Render models
		litShader.use();

		litShader.setMat4("view", camera.GetViewMatrix());
		litShader.setMat4("projection", projection); 
		litShader.setMat4("lightSpaceMatrix", dirLight.getWorldToClip());
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
		litShader.setInt("shadowMap", 4);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		// 4. use our shader program when we want to render an object
		DrawGeometry(soldier, floor, litShader);

		vegetationShader.use();

		vegetationShader.setMat4("view", camera.GetViewMatrix());
		vegetationShader.setMat4("projection", projection);
		vegetationShader.setVec3("_ViewPos", camera.cameraPos);

		DrawVegetation(grass, vegetationShader);

		//Render skybox
		skybox->Draw(projection, glm::mat4(glm::mat3(camera.GetViewMatrix())));

		//ImGui
		frameCount++;
		if (currentFrame - prevTime >= 1.0)
		{
			prevFPS = frameCount;
			frameCount = 0;
			prevTime = currentFrame;
		}

		imgui.drawPerfomance(deltaTime, prevFPS);
		imgui.drawDirLightProperties(lightAmbient, lightDiffuse, lightSpecular, lightPos);

		imgui.render();

		//Post processing step
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		framebufferShader.use();
		glBindVertexArray(rectVAO);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	imgui.shutdown();
	glfwTerminate();
	return 0;
}

void DrawGeometry(Entity& soldier, Entity& floor, Shader& shader)
{
	soldier.transform.setLocalRotation(glm::vec3(180.0f, 180.0f, 0.0f));
	soldier.updateSelfAndChild();
	shader.setMat4("model", soldier.transform.getModelMatrix());
	soldier.Draw(shader);
	shader.setMat4("model", soldier.getChild(0)->transform.getModelMatrix());
	soldier.getChild(0)->Draw(shader);

	shader.setMat4("model", floor.transform.getModelMatrix());
	floor.Draw(shader);
}

void DrawVegetation(Entity& grass, Shader& shader)
{
	grass.transform.setLocalRotation(glm::vec3(0.0f, 270.0f, 0.0f));
	for (int i = 0; i < 10; i++)
	{
		grass.transform.setLocalPos(glm::vec3(-i + 5, -1.0f, -i));
		grass.updateSelfAndChild();
		shader.setMat4("model", grass.transform.getModelMatrix());
		grass.Draw(shader);
	}
}
