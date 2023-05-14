#pragma once

#include "Components/TankBot/BotState.h"

#include <UnigineNode.h>

class TankController;

class BotStateKillEnemy : public BotState
{
public:
	BotStateKillEnemy(TankBot *bot, const Unigine::NodePtr &enemy_node_);

	void update() override;
	void onSwitchToState() override;

private:
	float get_distance_to_enemy() const;
	bool is_enemy_killed();
	void dodge_shots();
	void rotate_perpendicular_to_enemy();
	void go_back_and_forth_periodically();
	void aim_at_enemy();
	bool is_aimed_at_enemy();
	bool is_enemy_out_of_sight();
	bool is_enemy_within_firing_distance();
	bool is_enemy_within_keeping_distance();
	void go_closer_to_enemy();
	void set_random_moving_direction();
	void rotate_front_to_enemy();

private:
	TankController *tank_controller_ = nullptr;
	TankController *enemy_tank_controller_ = nullptr;
	float remaining_time_to_change_move_dir = 0.f;
	Unigine::NodePtr enemy_node_;
};
