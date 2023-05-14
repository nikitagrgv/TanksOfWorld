#pragma once

#include "Components/TankBot/BotState.h"

#include <UnigineMathLib.h>
#include <UnigineObjects.h>

class TankController;

class BotStateFindEnemy : public BotState
{
public:
	explicit BotStateFindEnemy(TankBot *bot);

	void update() override;
	void onSwitchToState() override;

private:
	Unigine::Vector<Unigine::ObjectPtr> get_frustum_intersections();
	Unigine::NodePtr get_enemy_from_frustum();
	Unigine::Math::Vec3 get_random_field_position() const;
	bool is_came_to_move_position() const;
	void set_random_move_position();
	void rotate_to_move_position();
	void find_enemy_periodically();
	void find_enemy();

private:
	TankController *tank_controller_ = nullptr;
	Unigine::Math::Vec3 move_position_ = {0, 0, 0};
	float remaining_time_to_find_enemy;
};
