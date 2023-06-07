#pragma once

#include "Model.h"
#include <vector>

class Transform
{
protected:

	glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
	glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 modelMatrix = glm::mat4(1.0f);

protected:
	glm::mat4 getLocalModelMatrix();

public:
	void computeModelMatrix();
	void computeModelMatrix(const glm::mat4& parentModel);

	void setLocalPos(glm::vec3 pos)
	{
		this->pos = pos;
	}
	void setLocalRotation(glm::vec3 rot)
	{
		this->eulerRot = rot;
	}
	void setLocalScale(glm::vec3 scale)
	{
		this->scale = scale;
	}

	const glm::mat4 getModelMatrix()
	{
		return modelMatrix;
	}
};

class Entity : public Model
{
public:
	Transform transform;

	Entity(const char* path) : Model(path)
	{

	}

	template<typename... TArgs>
	void addChild(const TArgs&... args)
	{
		childrens.emplace_back(std::make_unique<Entity>(args...));
		childrens.back()->parent = this;
	}

	void updateSelfAndChild()
	{
		if (parent)
		{
			transform.computeModelMatrix(parent->transform.getModelMatrix());
		}
		else
		{
			transform.computeModelMatrix();
		}

		for (auto& child : childrens)
		{
			child->updateSelfAndChild();
		}
	}

	const std::unique_ptr<Entity>& getChild(int index) const;
private:
	std::vector<std::unique_ptr<Entity>> childrens;
	Entity* parent = nullptr;
};