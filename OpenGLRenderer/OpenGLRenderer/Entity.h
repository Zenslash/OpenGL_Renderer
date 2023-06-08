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

	bool isDirty = true;

protected:
	glm::mat4 getLocalModelMatrix();

public:
	void computeModelMatrix();
	void computeModelMatrix(const glm::mat4& parentModel);

	void setLocalPos(glm::vec3 pos)
	{
		this->pos = pos;
		isDirty = true;
	}
	void setLocalRotation(glm::vec3 rot)
	{
		this->eulerRot = rot;
		isDirty = true;
	}
	void setLocalScale(glm::vec3 scale)
	{
		this->scale = scale;
		isDirty = true;
	}

	const glm::mat4 getModelMatrix()
	{
		return modelMatrix;
	}

	bool IsDirty() const
	{
		return isDirty;
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

	void updateSelfAndChild();

	const std::unique_ptr<Entity>& getChild(int index) const;
private:
	void forceUpdateSelfAndChild();

private:
	std::vector<std::unique_ptr<Entity>> childrens;
	Entity* parent = nullptr;
};