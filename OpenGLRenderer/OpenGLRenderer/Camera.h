#pragma once

#include <glm.hpp>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	float pitch = 0.0f;
	float yaw = -90.0f;
	float mouseSensitivity;
	float speed;

	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;
	glm::vec3 worldUp;
	glm::mat4 view;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f)
	{
		cameraPos = position;
		worldUp = up;
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		mouseSensitivity = 0.1f;
		speed = 5.0f;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	{
		cameraPos = glm::vec3(posX, posY, posZ);
		worldUp = glm::vec3(upX, upY, upZ);
		cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		mouseSensitivity = 0.1f;
		speed = 5.0f;
		this->yaw = yaw;
		this->pitch = pitch;
		updateCameraVectors();
	}

	void ProcessMovementInput(Camera_Movement direction, float deltaTime)
	{
		switch (direction)
		{
		case FORWARD:
			cameraPos += speed * deltaTime * cameraFront;
			break;
		case BACKWARD:
			cameraPos -= speed * deltaTime * cameraFront;
			break;
		case LEFT:
			cameraPos += speed * deltaTime * cameraRight;
			break;
		case RIGHT:
			cameraPos -= speed * deltaTime * cameraRight;
			break;
		default:
			break;
		}

		updateCameraVectors();
	}
	void ProcessMouseMovement(float xoffset, float yoffset)
	{
		xoffset *= mouseSensitivity;
		yoffset *= mouseSensitivity;

		yaw -= xoffset;
		pitch += yoffset;

		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		else if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}

		updateCameraVectors();
	}

	const glm::mat4x4& GetViewMatrix() const
	{
		return view;
	}

private:
	void updateCameraVectors()
	{
		cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront.y = sin(glm::radians(pitch));
		cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(cameraFront);

		cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
		cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}
};