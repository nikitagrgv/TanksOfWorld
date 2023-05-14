#include "Components/TankBot/BotStateKillEnemy.h"

#include "Components/GameController.h"
#include "Components/TankBot/TankBot.h"
#include "Components/TankController.h"

#include <UnigineGame.h>

#include <cmath>

// Period of updating moving direction of bot. Uses for dodging
static const float CHANGE_MOVE_DIR_UPDATE_TIME = 1.f;

// When the bot has caught an enemy, it will lose it at another distance,
// which is calculated by multiplying the max visibility distance by this coefficient
static const float LOOSING_ENEMY_DISTANCE_MULTIPLIER = 2.f;

// The bot thinks to get closer when the distance to the target is less than the
// fire distance multiplied by this coefficient
static const float KEEPING_DISTANCE_MULTIPLIER = 0.6;

BotStateKillEnemy::BotStateKillEnemy(TankBot *bot, const Unigine::NodePtr &enemy_node)
	: BotState(bot)
{
	enemy_node_ = enemy_node;
}

void BotStateKillEnemy::onSwitchToState()
{
	tank_controller_ = bot_->getTankController();
	enemy_tank_controller_ = Unigine::ComponentSystem::get()->getComponent<TankController>(enemy_node_);

	tank_controller_->setUseAutoAiming(true);
	tank_controller_->setGoingToFire(false);
	tank_controller_->setHullRotating(TankController::HULL_ROTATING_AUTO);
	set_random_moving_direction();
}

void BotStateKillEnemy::update()
{
	if (is_enemy_killed() || is_enemy_out_of_sight())
	{
		bot_->setState(new BotStateFindEnemy(bot_));
		return;
	}

	aim_at_enemy();

	if (is_enemy_within_keeping_distance())
		dodge_shots();
	else
		go_closer_to_enemy();

	if (is_enemy_within_firing_distance())
		tank_controller_->setGoingToFire(is_aimed_at_enemy());
	else
		tank_controller_->setGoingToFire(false);
}

void BotStateKillEnemy::set_random_moving_direction()
{
	int choice = Unigine::Math::Random::getRandom().getInt(0, 3);

	switch (choice)
	{
	case 0: tank_controller_->setHullMoving(TankController::HULL_MOVING_NONE); break;
	case 1: tank_controller_->setHullMoving(TankController::HULL_MOVING_FORWARD); break;
	default: tank_controller_->setHullMoving(TankController::HULL_MOVING_BACK); break;
	}
}

bool BotStateKillEnemy::is_enemy_killed()
{
	return enemy_node_.isDeleted();
}

bool BotStateKillEnemy::is_enemy_out_of_sight()
{
	return get_distance_to_enemy() > bot_->max_visible_distance * LOOSING_ENEMY_DISTANCE_MULTIPLIER;
}

float BotStateKillEnemy::get_distance_to_enemy() const
{
	return (tank_controller_->getPosition() - enemy_node_->getPosition()).length();
}

void BotStateKillEnemy::aim_at_enemy()
{
	tank_controller_->setAimPosition(enemy_tank_controller_->node_tower->getWorldPosition());
}

bool BotStateKillEnemy::is_enemy_within_keeping_distance()
{
	return get_distance_to_enemy() < (tank_controller_->max_firing_distance * KEEPING_DISTANCE_MULTIPLIER);
}

void BotStateKillEnemy::dodge_shots()
{
	rotate_perpendicular_to_enemy();
	go_back_and_forth_periodically();
}

void BotStateKillEnemy::rotate_perpendicular_to_enemy()
{
	float angle_to_enemy = tank_controller_->getAngleWorldWithAim();

	// Rotate to nearest perpendicular angle

	if (tank_controller_->getAngleHullWithAim() < 0)
	{
		tank_controller_->setNeededHullAngle(angle_to_enemy + 90.f);
	}
	else
	{
		tank_controller_->setNeededHullAngle(angle_to_enemy - 90.f);
	}
}

void BotStateKillEnemy::go_back_and_forth_periodically()
{
	remaining_time_to_change_move_dir -= Unigine::Game::getIFps();
	if (remaining_time_to_change_move_dir <= 0.f)
	{
		remaining_time_to_change_move_dir = CHANGE_MOVE_DIR_UPDATE_TIME;
		set_random_moving_direction();
	}
}

bool BotStateKillEnemy::is_aimed_at_enemy()
{
	auto fire_point = tank_controller_->getFirePoint();
	auto fire_dir = tank_controller_->getFireDirection();
	auto fire_end = fire_point + fire_dir * tank_controller_->max_firing_distance;

	auto intersected = Unigine::World::getIntersection(fire_point, fire_end,
		bot_->getGameController()->shot_intersections_mask.get());

	if (intersected.isNull())
		return false;

	auto *other_tank = Unigine::ComponentSystem::get()->getComponentInParent<TankController>(intersected);

	if (other_tank == nullptr)
		return false;

	if (!tank_controller_->isFriend(other_tank))
		return true;

	return false;
}

bool BotStateKillEnemy::is_enemy_within_firing_distance()
{
	return get_distance_to_enemy() < tank_controller_->max_firing_distance;
}

void BotStateKillEnemy::go_closer_to_enemy()
{
	rotate_front_to_enemy();
	tank_controller_->setHullMoving(TankController::HULL_MOVING_FORWARD);
}

void BotStateKillEnemy::rotate_front_to_enemy()
{
	tank_controller_->setNeededHullAngle(tank_controller_->getAngleWorldWithAim());
}
