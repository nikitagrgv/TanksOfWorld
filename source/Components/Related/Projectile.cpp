#include "Components/Related/Projectile.h"

#include "Components/GameController.h"
#include "Components/TankController.h"

#include <UnigineGame.h>
#include <UnigineVisualizer.h>

static float PROJECTILE_SPEED = 150.f;
static float PROJECTILE_VISUAL_LENGTH = 4.f;

Projectile::Projectile(const Unigine::Math::Vec3 &fire_point, const Unigine::Math::Vec3 &direction,
					   float distance, int damage, int shoot_intersection_mask, TankController *shooter_tank)
{
	fire_point_ = fire_point;
	direction_ = direction;
	damage_ = damage;
	shooter_tank_ = shooter_tank;
	shoot_intersection_mask_ = shoot_intersection_mask;

	lifetime_ = distance / PROJECTILE_SPEED;

	color_ = shooter_tank_->team_index == 0 ? Unigine::Math::vec4_blue : Unigine::Math::vec4_red;
}

void Projectile::update()
{
	if (isExpired())
		return;

	// A moving projectile can be considered as a sequence of small, infinitely fast projectiles
	// Every delta time the projectile moves at delta distance
	// At this delta distance intersections with objects are calculated
	// start_point and end_point determine this delta distance
	// This points moves with the time, so the projectile moves
	// Current position of projectile is calculated by formula: x = x0 + v * t

	Unigine::Math::Vec3 start_point = fire_point_ + direction_ * current_time_ * PROJECTILE_SPEED;
	current_time_ += Unigine::Game::getIFps();
	Unigine::Math::Vec3 end_point = fire_point_ + direction_ * current_time_ * PROJECTILE_SPEED;

	// For rendering uses start_point, but not uses end_point. The end point is calculated
	// in such a way that the projectile length is constant and not depends on FPS

	Unigine::Visualizer::renderLine3D(start_point, start_point + direction_ * PROJECTILE_VISUAL_LENGTH, color_);

	auto intersection = Unigine::WorldIntersection::create();
	auto obj_under_shot = Unigine::World::getIntersection(start_point, end_point, shoot_intersection_mask_, intersection);

	// If projectile not intersect something, go ahead moving
	if (obj_under_shot.isNull())
		return;

	auto *tank_under_shot = Unigine::ComponentSystem::get()->getComponentInParent<TankController>(obj_under_shot);

	// Ignore intersections with the tank that made a shot
	if (tank_under_shot == shooter_tank_)
		return;

	destroy_projectile();

	// If intersected with another tank, take damage on it
	if (tank_under_shot != nullptr)
	{
		tank_under_shot->takeDamage(damage_);
	}
}

void Projectile::destroy_projectile()
{
	current_time_ = lifetime_;
}

bool Projectile::isExpired() const
{
	return current_time_ >= lifetime_;
}
