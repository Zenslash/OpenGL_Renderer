
#include <glad/glad.h>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "stb_image.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* wnd, int width, int height)
{
	glViewport(0, 0, width, height);
}
void process_input(GLFWwindow* wnd)
{
	if(glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(wnd, true);
	}
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

	//TODO Load custom models
	float vertices[] = {
		// positions         // colors			//UV coords
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,	1.0f, 0.0f,   // bottom right
		 -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,	0.0f, 0.0f,   // bottom left
		 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,	1.0f, 1.0f,    // top right 
		 -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,	0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,   // first triangle
		1, 3, 2,   // second triangle
	};

	//Gen buffers
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	//Compile shaders
	Shader shader("Shaders/Colored.vs", "Shaders/Colored.fs");

	//Bind buffers
	// 1. bind VAO
	glBindVertexArray(VAO);

	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// 3. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//Configure textures
	unsigned int texID, decalID;

	glGenTextures(1, &texID);
	glGenTextures(1, &decalID);
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

	glBindTexture(GL_TEXTURE_2D, decalID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	texData = stbi_load("E:/CG/Projects/OpenGL_Renderer/OpenGLRenderer/Resources/Textures/decal.png",
		&tWidth, &tHeight, &nrChannels, 0);

	if (texData != nullptr)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load texture!" << std::endl;
	}

	stbi_image_free(texData);

	shader.use();
	shader.setInt("albedo", 0);
	shader.setInt("decal", 1);

	//Game loop
	while(!glfwWindowShouldClose(wnd))
	{
		process_input(wnd);

		//Rendering here
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.use();
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		
		
		shader.setMat4("transform", trans);

		// 4. use our shader program when we want to render an object
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, decalID);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		////////////


		glfwSwapBuffers(wnd);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}