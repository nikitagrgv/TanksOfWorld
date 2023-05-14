#pragma once

#include "Components/Related/Projectile.h"

#include <UnigineComponentSystem.h>

class TankController;

class GameController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(GameController, ComponentBase);

	PROP_GROUP("Field settings");
	PROP_PARAM(Vec2, field_size);

	PROP_GROUP("Spawn settings");
	PROP_PARAM(Float, grid_cell_size);
	PROP_PARAM(File, team1_tank);
	PROP_PARAM(File, team2_tank);
	PROP_PARAM(File, player_tank);
	PROP_PARAM(Int, team1_tanks_count);
	PROP_PARAM(Int, team2_tanks_count);

	PROP_GROUP("Shooting settings");
	PROP_PARAM(Mask, shot_intersections_mask, 0b1);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_SHUTDOWN(shutdown);

	// Start point - point with minimum x and y
	Unigine::Math::Vec2 getFieldStartPoint();
	// End point - point with maximum x and y
	Unigine::Math::Vec2 getFieldEndPoint();

	void processShot(const Unigine::Math::Vec3 &gun_point, const Unigine::Math::Vec3 &direction,
		float max_distance, int damage, TankController *shooter);

protected:
	void init();
	void update();
	void shutdown();

private:
	void spawn_tanks_randomly();
	void spawn_tank_randomly_uniquely_to_grid(const char *tank_file,
		Unigine::HashSet<Unigine::Math::ivec2> &engaged_cells);

private:
	Unigine::Vector<Projectile> projectiles_;
};
