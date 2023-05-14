#include "Components/TankController.h"

#include "Components/GameController.h"

#include <cmath>
#include <UnigineGame.h>

using Unigine::Math::Vec3;

// to compare float numbers
static const float EPSILON = 0.01f;

inline static float normalize_angle(float angle)
{
	return std::remainder(angle, 360.f);
}

inline static float get_delta_angle(float first, float second)
{
	return normalize_angle(second - first);
}

REGISTER_COMPONENT(TankController);

void TankController::init()
{
	assert(game_controller_node.get().isValid());
	game_controller_ = Unigine::ComponentSystem::get()->getComponent<GameController>(
		game_controller_node.get());
	assert(game_controller_ != nullptr);

	health_ = max_health;
}

void TankController::update()
{
	float dt = Unigine::Game::getIFps();

	try_to_fire(dt);

	apply_hull_rotating(dt);
	apply_hull_moving(dt);
	apply_auto_aiming(dt);
}

void TankController::try_to_fire(float dt)
{
	if (remaining_reload_time_ > 0)
	{
		remaining_reload_time_ -= dt;
	}
	else if (going_to_fire_)
	{
		game_controller_->processShot(getFirePoint(), getFireDirection(), max_firing_distance, damage, this);
		remaining_reload_time_ = reload_time;
		going_to_fire_ = false;
	}
}

void TankController::apply_hull_rotating(float dt)
{
	if (hull_rotating_dir_ == HULL_ROTATING_AUTO)
	{
		float needed_delta_angle = get_delta_angle(hull_angle_, needed_hull_angle_);

		if (Unigine::Math::abs(needed_delta_angle) > EPSILON)
		{
			float rotate_direction = needed_delta_angle > 0 ? 1.f : -1.f;

			float delta_angle = hull_rotate_speed * rotate_direction * dt;

			if (Unigine::Math::abs(delta_angle) > Unigine::Math::abs(needed_delta_angle))
				hull_angle_ = needed_hull_angle_;
			else
				hull_angle_ = normalize_angle(hull_angle_ + delta_angle);
		}
	}
	else
	{
		float angular_velocity = hull_rotate_speed * hull_rotating_dir_;
		hull_angle_ += angular_velocity * dt;
	}

	apply_hull_angle();
}

void TankController::apply_hull_angle()
{
	auto old_pos = node_hull->getWorldPosition();
	node_hull->setWorldTransform(Unigine::Math::translate(old_pos) * Unigine::Math::rotateZ(hull_angle_));
}


void TankController::apply_hull_moving(float dt)
{
	Vec3 forward_vector = node_hull->getTransform().getAxisY();

	Vec3 velocity_vector = forward_vector * hull_move_speed * hull_moving_dir_;

	node->translate(velocity_vector * dt);
}

void TankController::apply_auto_aiming(float dt)
{
	if (!use_auto_aiming_)
		return;

	// Aiming for the tower
	{
		float needed_angle = get_needed_tower_angle();
		float needed_delta_angle = get_delta_angle(tower_angle_, needed_angle);

		if (Unigine::Math::abs(needed_delta_angle) > EPSILON)
		{
			float rotate_direction = needed_delta_angle > 0 ? 1.f : -1.f;

			float delta_angle = tower_rotate_speed * rotate_direction * dt;

			if (Unigine::Math::abs(delta_angle) > Unigine::Math::abs(needed_delta_angle))
				tower_angle_ = needed_angle;
			else
				tower_angle_ = normalize_angle(tower_angle_ + delta_angle);

			apply_tower_angle();
		}
	}

	// Aiming for the gun
	{
		float needed_angle = get_needed_gun_angle();

		float needed_delta_angle = get_delta_angle(gun_angle_, needed_angle);

		if (Unigine::Math::abs(needed_delta_angle) > EPSILON)
		{
			float rotate_direction = needed_delta_angle > 0 ? 1.f : -1.f;

			float delta_angle = gun_rotate_speed * rotate_direction * dt;

			if (Unigine::Math::abs(delta_angle) > Unigine::Math::abs(needed_delta_angle))
				gun_angle_ = needed_angle;
			else
				gun_angle_ = normalize_angle(gun_angle_ + delta_angle);

			clamp_gun_angle();
			apply_gun_angle();
		}
	}
}

float TankController::get_needed_tower_angle()
{
	Vec3 vec_from_tank_to_aim
		= (aim_position_ - node_tower->getWorldPosition()).normalize();

	return getAngle(getForwardDirection(), vec_from_tank_to_aim, Unigine::Math::Vec3_up);
}

void TankController::apply_tower_angle()
{
	auto old_pos = node_tower->getPosition();
	node_tower->setTransform(Unigine::Math::translate(old_pos) * Unigine::Math::rotateZ(tower_angle_));
}

float TankController::get_needed_gun_angle()
{
	Vec3 vec_from_tank_to_aim
		= (aim_position_ - node_gun->getWorldPosition()).normalize();

	// To calculate the angle with the plane, we need to first calculate the angle with the normal
	// Angle with the plane = 90 - angle with the normal

	float angle_with_tank_plane = 90.f - getAngle(Unigine::Math::Vec3_up, vec_from_tank_to_aim);

	return angle_with_tank_plane;
}

void TankController::clamp_gun_angle()
{
	gun_angle_ = Unigine::Math::clamp(gun_angle_, min_gun_angle, max_gun_angle);
}

void TankController::apply_gun_angle()
{
	auto old_pos = node_gun->getPosition();
	node_gun->setTransform(Unigine::Math::translate(old_pos) * Unigine::Math::rotateX(gun_angle_));
}

void TankController::setNeededHullAngle(float angle)
{
	needed_hull_angle_ = normalize_angle(angle);
}

Unigine::Math::Vec3 TankController::getFirePoint() const
{
	return node_gun_shooter.get()->getWorldPosition();
}

Unigine::Math::Vec3 TankController::getFireDirection() const
{
	return node_gun_shooter.get()->getWorldDirection(Unigine::Math::AXIS_Y);
}

float TankController::getRemainingReloadTime() const
{
	return remaining_reload_time_ > 0.f ? remaining_reload_time_ : 0.f;
}

Unigine::Math::Vec3 TankController::getForwardDirection() const
{
	// return node_hull->getWorldDirection(Unigine::Math::AXIS_Y);
	return node_hull.get()->getWorldDirection(Unigine::Math::AXIS_Y);
}

float TankController::getAngleWorldWithAim() const
{
	auto vec_to_aim = getAimPosition() - getPosition();
	return Unigine::Math::getAngle(Unigine::Math::Vec3_forward, vec_to_aim, Unigine::Math::Vec3_up);
}

float TankController::getAngleHullWithAim() const
{
	auto tank_forward = getForwardDirection();
	auto vec_to_enemy = getAimPosition() - getPosition();

	return Unigine::Math::getAngle(tank_forward, vec_to_enemy, Unigine::Math::Vec3_up);
}

// ================== Health control =================

bool TankController::isDead() const
{
	if (is_immortal)
		return false;

	return health_ <= 0;
}

void TankController::kill()
{
	node.deleteLater();
}

void TankController::takeDamage(int damage)
{
	if (is_immortal)
		return;

	health_ -= damage;

	if (isDead())
		kill();
}

// ================== Teams logic =================

bool TankController::isFriend(TankController *other_tank_controller) const
{
	return team_index == other_tank_controller->team_index;
}
