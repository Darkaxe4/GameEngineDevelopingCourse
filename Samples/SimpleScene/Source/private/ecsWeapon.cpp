#include <ecsWeapon.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <ecsMesh.h>
#include <DefaultGeometry.h>
#include <RenderObject.h>

using namespace GameEngine;

void RegisterEcsWeaponSystems(flecs::world& world)
{


	world.component<ReadyToShoot>();
	world.component<IsBullet>();

	world.system<Weapon>()
		.each([&](flecs::entity e, Weapon& weapon)
	{
		if (weapon.cooldownTimer > 0.f)
			weapon.cooldownTimer -= world.delta_time();
		else if (e.has<ReadyToShoot>())
			return;
		else
			e.add<ReadyToShoot>();
	});

	world.system<Weapon, const WeaponInfo,const ReadyToShoot, const ShootEvent>()
		.each([&](flecs::entity e, Weapon& weapon, const WeaponInfo& info, const ReadyToShoot& ready, const ShootEvent& event)
			{
				CreateBullet(world, e, event.pos, event.dir);
				weapon.bulletsLeft -= 1;
				e.remove<ReadyToShoot>();
				e.remove<ShootEvent>();
				if (weapon.bulletsLeft > 0)
					weapon.cooldownTimer = info.shootTimer;
				else
				{
					weapon.cooldownTimer = info.reloadTimer;
					weapon.bulletsLeft = info.magSize;
				}
					
			});

	world.system<const Collider, const IsBullet, const CollidedToSurface>()
		.each([&](flecs::entity e, const Collider& collider, const IsBullet& bullet, const CollidedToSurface& event)
		{
			if (e.has<DestroyAfterCollision>())
				return;
			else
				e.set<DestroyAfterCollision>({ 5.f });
		});
	world.system<CollisionEvent, const Collider, const IsBullet>()
		.each([&](flecs::entity e,  CollisionEvent& event, const Collider& collider, const IsBullet& bullet)
		{
				event.first.set<DestroyAfterCollision>({ 0.f });
				event.second.set<DestroyAfterCollision>({ 0.f });
				if (const AmmoRefiller* r = event.second.get<AmmoRefiller>())
				{
					if (Weapon* w = e.parent().get_mut<Weapon>())
					{
						w->bulletsLeft += r->count;
						w->cooldownTimer = 0.f;
					}
				}
		});
}

void CreateBullet(flecs::world& world, flecs::entity weapon, Math::Vector3f pos, Math::Vector3f dir)
{
	world.entity()
		.child_of(weapon)
		.set(Position{ pos })
		.set(Velocity{ dir.Normalized() *  50.f })
		.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
		.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
		.set(Bounciness{ 0.1f })
		.set(FrictionAmount{ 1.f })
		.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(Collider{ 1.f })
		.set(RenderObjectPtr{ new Render::RenderObject() })
		.add<IsBullet>();
}
