#include <ecsPhys.h>
#include <flecs.h>
#include <ecsMesh.h>

namespace
{
	inline float rand_flt(float from, float to)
	{
		return from + (float(rand()) / RAND_MAX) * (to - from);
	}
}

void RegisterEcsPhysSystems(flecs::world& world)
{
	world.component<CollidedToSurface>();

	world.system<Velocity, const Gravity, BouncePlane*, Position*>()
		.each([&](flecs::entity e, Velocity& vel, const Gravity& grav, BouncePlane* plane, Position* pos)
	{
		if (plane && pos)
		{
			constexpr float planeEpsilon = 0.1f;
			if (plane->value.x * pos->value.x + plane->value.y * pos->value.y + plane->value.z * pos->value.z < plane->value.w + planeEpsilon)
			{
				if (not e.has<CollidedToSurface>())
					e.add<CollidedToSurface>();
				return;
			}
		}
		vel.value.x += grav.value.x * world.delta_time();
		vel.value.y += grav.value.y * world.delta_time();
		vel.value.z += grav.value.z * world.delta_time();
	});


	world.system<Velocity, Position, const BouncePlane, const Bounciness>()
		.each([&](Velocity& vel, Position& pos, const BouncePlane& plane, const Bounciness& bounciness)
	{
		float dotPos = plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z;
		float dotVel = plane.value.x * vel.value.x + plane.value.y * vel.value.y + plane.value.z * vel.value.z;
		if (dotPos < plane.value.w)
		{
			pos.value.x -= (dotPos - plane.value.w) * plane.value.x;
			pos.value.y -= (dotPos - plane.value.w) * plane.value.y;
			pos.value.z -= (dotPos - plane.value.w) * plane.value.z;

			vel.value.x -= (1.f + bounciness.value) * plane.value.x * dotVel;
			vel.value.y -= (1.f + bounciness.value) * plane.value.y * dotVel;
			vel.value.z -= (1.f + bounciness.value) * plane.value.z * dotVel;
		}
	});


	world.system<Velocity, const FrictionAmount>()
		.each([&](flecs::entity e, Velocity& vel, const FrictionAmount& friction)
	{
		vel.value.x -= vel.value.x * friction.value * world.delta_time();
		vel.value.y -= vel.value.y * friction.value * world.delta_time();
		vel.value.z -= vel.value.z * friction.value * world.delta_time();
	});


	world.system<Position, const Velocity>()
		.each([&](flecs::entity e, Position& pos, const Velocity& vel)
	{
		pos.value.x += vel.value.x * world.delta_time();
		pos.value.y += vel.value.y * world.delta_time();
		pos.value.z += vel.value.z * world.delta_time();
	});


	world.system<Position, const ShiverAmount>()
		.each([&](flecs::entity e, Position& pos, const ShiverAmount& shiver)
	{
		pos.value.x += rand_flt(-shiver.value, shiver.value);
		pos.value.y += rand_flt(-shiver.value, shiver.value);
		pos.value.z += rand_flt(-shiver.value, shiver.value);
	});

	flecs::query colliderQuery = world.query<Position, const Collider>();

	world.system<Position, const Collider>()
		.each([&, colliderQuery](flecs::entity e1, Position& p1, const Collider& c1)
	{
		colliderQuery.each(
			[&](flecs::entity e2,
				Position& p2,
				const Collider& c2)
			{

				if ((e1 == e2) || (e1.id() >= e2.id()))
					return;

				if (CheckCollision(p1, c1, p2, c2))
				{
					e1.set<CollisionEvent>({ e1, e2 });
					e2.set<CollisionEvent>({ e2, e1 });
				}
			});
	});

	world.system<DestroyAfterCollision>()
		.write(flecs::Wildcard)
		.each([&](flecs::entity e, DestroyAfterCollision& marker)
	{
		if (e.has<MarkedToDestroy>())
			return;
		marker.timer -= world.delta_time();
		if (marker.timer <= 0.f)
			e.add<MarkedToDestroy>();
	});

}

bool CheckCollision(Position& posFirst, const Collider& colFirst, Position& posSecond, const Collider& colSecond)
{
	GameEngine::Math::Vector3f delta = posFirst.value - posSecond.value;

	float distance = delta.GetLength();
	float radiusSum = colFirst.radius + colSecond.radius;

	return distance <= radiusSum;
}
