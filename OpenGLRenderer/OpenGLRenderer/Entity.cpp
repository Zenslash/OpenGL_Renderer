#include "Entity.h"

glm::mat4 Transform::getLocalModelMatrix()
{
	const glm::mat4 transformX = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	const glm::mat4 transformY = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 transformZ = glm::rotate(glm::mat4(1.0f), glm::radians(eulerRot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	const glm::mat4 rotationMatrix = transformY * transformX * transformZ;

	return glm::translate(glm::mat4(1.0f), pos) *
		rotationMatrix *
		glm::scale(glm::mat4(1.0f), scale);
}

void Transform::computeModelMatrix()
{
	modelMatrix = getLocalModelMatrix();
}

void Transform::computeModelMatrix(const glm::mat4& parentModel)
{
	modelMatrix = parentModel * getLocalModelMatrix();
}

const std::unique_ptr<Entity>& Entity::getChild(int index) const
{
	if (index >= childrens.size())
	{
		return nullptr;
	}

	return childrens[index];
}
