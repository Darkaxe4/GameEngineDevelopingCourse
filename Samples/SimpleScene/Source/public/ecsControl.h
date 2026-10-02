#pragma once

#include <flecs.h>
#include <Vector.h>

namespace GameEngine::Core
{
	class Camera;
	class Controller;
}

struct ControllerPtr
{
	GameEngine::Core::Controller* ptr;
};

struct JumpSpeed
{
	float value;
};

struct CameraPtr
{
	GameEngine::Core::Camera* ptr;
};


void RegisterEcsControlSystems(flecs::world& world);