#pragma once

#include <flecs.h>
#include <Vector.h>

struct ShootEvent
{
	GameEngine::Math::Vector3f pos;
	GameEngine::Math::Vector3f dir;
};

struct WeaponInfo
{
	float shootTimer;
	float reloadTimer;
	size_t magSize;
};

struct ReadyToShoot {};
struct IsBullet {};

struct Weapon
{
	GameEngine::Math::Vector3f direction;
	float cooldownTimer;
	size_t bulletsLeft;
};

struct AmmoRefiller
{
	size_t count;
};

void RegisterEcsWeaponSystems(flecs::world& world);

void CreateBullet(flecs::world& world, flecs::entity weapon, GameEngine::Math::Vector3f pos, GameEngine::Math::Vector3f dir);
