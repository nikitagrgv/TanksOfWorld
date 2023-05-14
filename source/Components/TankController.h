#pragma once

#include <UnigineComponentSystem.h>

class GameController;

class TankController : public Unigine::ComponentBase
{
public:
	enum HullMoving
	{
		HULL_MOVING_FORWARD = 1,
		HULL_MOVING_BACK = -1,
		HULL_MOVING_NONE = 0
	};

	enum HullRotating
	{
		HULL_ROTATING_CLOCKWISE = 1,
		HULL_ROTATING_ANTICLOCKWISE = -1,
		HULL_ROTATING_NONE = 0,
		HULL_ROTATING_AUTO = 2
	};

	COMPONENT_DEFINE(TankController, Unigine::ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_GROUP("Game controller")
	PROP_PARAM(Node, game_controller_node);

	PROP_GROUP("Tank nodes")
	PROP_PARAM(Node, node_hull);
	PROP_PARAM(Node, node_tower);
	PROP_PARAM(Node, node_gun);
	PROP_PARAM(Node, node_gun_shooter);

	PROP_GROUP("Speed parameters");
	PROP_PARAM(Float, hull_move_speed, 14.f);
	PROP_PARAM(Float, hull_rotate_speed, 90.f);
	PROP_PARAM(Float, tower_rotate_speed, 120.f);
	PROP_PARAM(Float, gun_rotate_speed, 120.f);

	PROP_GROUP("Bound values");
	PROP_PARAM(Float, min_gun_angle, -15.f);
	PROP_PARAM(Float, max_gun_angle, 15.f);

	PROP_GROUP("Health parameters");
	PROP_PARAM(Toggle, is_immortal);
	PROP_PARAM(Int, max_health, 100);

	PROP_GROUP("Firing parameters");
	PROP_PARAM(Float, reload_time, 1.f);
	PROP_PARAM(Int, damage, 5);
	PROP_PARAM(Float, max_firing_distance, 240.f);

	PROP_GROUP("Game parameters");
	PROP_PARAM(Int, team_index, 0);

	GameController *getGameController() const { return game_controller_; }

	Unigine::Math::Vec3 getPosition() const { return node->getWorldPosition(); }
	Unigine::Math::Vec3 getForwardDirection() const;

	Unigine::Math::Vec3 getAimPosition() const { return aim_position_; }
	void setAimPosition(const Unigine::Math::Vec3& position) { aim_position_ = position; }
	void setUseAutoAiming(bool use) { use_auto_aiming_ = use; };
	// Returns angle between vector from tank to aim and forward vector of world
	float getAngleWorldWithAim() const;
	float getAngleHullWithAim() const;

	Unigine::Math::Vec3 getFirePoint() const;
	Unigine::Math::Vec3 getFireDirection() const;
	void setGoingToFire(bool is_firing) { going_to_fire_ = is_firing; }
	float getRemainingReloadTime() const;

	void setHullMoving(HullMoving hull_moving_dir) { hull_moving_dir_ = hull_moving_dir; }
	void setHullRotating(HullRotating hull_rotating_dir) { hull_rotating_dir_ = hull_rotating_dir; }
	void setNeededHullAngle(float angle);

	int getHealth() const { return health_; }
	bool isDead() const;
	void kill();
	void takeDamage(int damage);

	bool isFriend(TankController *other_tank_controller) const;

protected:
	void init();
	void update();

private:
	void try_to_fire(float dt);

	void apply_hull_rotating(float dt);
	void apply_hull_moving(float dt);
	void apply_auto_aiming(float dt);

	float get_needed_tower_angle();
	void apply_tower_angle();
	void apply_hull_angle();

	float get_needed_gun_angle();
	void clamp_gun_angle();
	void apply_gun_angle();

private:
	GameController *game_controller_ = nullptr;

	HullMoving hull_moving_dir_ = HULL_MOVING_NONE;
	HullRotating hull_rotating_dir_ = HULL_ROTATING_NONE;

	bool use_auto_aiming_ = false;
	Unigine::Math::Vec3 aim_position_;

	float needed_hull_angle_ = 0;
	float hull_angle_ = 0;
	float tower_angle_ = 0;
	float gun_angle_ = 0;

	bool going_to_fire_ = false;
	float remaining_reload_time_ = 0.f;

	int health_;
};
