#include "Components/TankBot/BotStateFindEnemy.h"

#include "Components/GameController.h"
#include "Components/TankBot/TankBot.h"
#include "Components/TankController.h"

#include <UnigineGame.h>

// Bot check frustum intersection with this period. For optimization
static const float ENEMY_FINDING_UPDATE_TIME = 0.4f;

// Distance to the move position, when the bot consider that it has been reached
static const float CAME_DISTANCE = 12.f;

BotStateFindEnemy::BotStateFindEnemy(TankBot *bot)
	: BotState(bot)
{
	// Randomly distribute times when bots check frustum intersections. For optimization
	remaining_time_to_find_enemy = Unigine::Math::Random::getRandom().getFloat(0,
		ENEMY_FINDING_UPDATE_TIME);
}

void BotStateFindEnemy::onSwitchToState()
{
	tank_controller_ = bot_->getTankController();

	tank_controller_->setUseAutoAiming(true);
	tank_controller_->setGoingToFire(false);
	tank_controller_->setHullRotating(TankController::HULL_ROTATING_AUTO);
	tank_controller_->setHullMoving(TankController::HULL_MOVING_FORWARD);
	set_random_move_position();
}

void BotStateFindEnemy::update()
{
	if (is_came_to_move_position())
	{
		set_random_move_position();
		return;
	}

	rotate_to_move_position();

	find_enemy_periodically();
}

void BotStateFindEnemy::find_enemy_periodically()
{
	remaining_time_to_find_enemy -= Unigine::Game::getIFps();
	if (remaining_time_to_find_enemy <= 0.f)
	{
		remaining_time_to_find_enemy = ENEMY_FINDING_UPDATE_TIME;
		find_enemy();
	}
}

void BotStateFindEnemy::find_enemy()
{
	Unigine::NodePtr enemy = get_enemy_from_frustum();
	if (enemy.isValid())
		bot_->setState(new BotStateKillEnemy(bot_, enemy));
}

Unigine::NodePtr BotStateFindEnemy::get_enemy_from_frustum()
{
	auto intersected_objects = get_frustum_intersections();

	for (const auto &obj : intersected_objects)
	{
		auto *other_tank = Unigine::ComponentSystem::get()->getComponentInParent<TankController>(obj);
		if (other_tank == nullptr)
			continue;

		if (!tank_controller_->isFriend(other_tank))
			return other_tank->getNode();
	}

	return nullptr;
}

Unigine::Vector<Unigine::ObjectPtr> BotStateFindEnemy::get_frustum_intersections()
{
	auto transform = tank_controller_->node_tower->getWorldTransform() * Unigine::Math::rotateX(90.f);
	auto frustum = Unigine::Math::BoundFrustum(bot_->getFrustumProjection(), inverse(transform));

	Unigine::Vector<Unigine::ObjectPtr> intersected_objects;
	Unigine::World::getIntersection(frustum, intersected_objects);

	return intersected_objects;
}

void BotStateFindEnemy::set_random_move_position()
{
	Unigine::Math::Vec3 new_position = get_random_field_position();
	move_position_ = new_position;
	tank_controller_->setAimPosition(new_position);
}

Unigine::Math::Vec3 BotStateFindEnemy::get_random_field_position() const
{
	auto *game_controller = bot_->getGameController();

	Unigine::Math::Vec3 move_pos;
	move_pos.x = Unigine::Math::Random::getRandom().getFloat(
		game_controller->getFieldStartPoint().x, game_controller->getFieldEndPoint().x);
	move_pos.y = Unigine::Math::Random::getRandom().getFloat(
		game_controller->getFieldStartPoint().y, game_controller->getFieldEndPoint().y);

	return move_pos;
}

bool BotStateFindEnemy::is_came_to_move_position() const
{
	return (move_position_ - tank_controller_->getPosition()).length() < CAME_DISTANCE;
}

void BotStateFindEnemy::rotate_to_move_position()
{
	tank_controller_->setNeededHullAngle(tank_controller_->getAngleWorldWithAim());
}
