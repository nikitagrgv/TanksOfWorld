#include "Components/GameController.h"

#include "Components/TankController.h"

#include <UnigineGame.h>

static const int MAX_SPAWN_TRIES = 100000;

REGISTER_COMPONENT(GameController);

void GameController::init()
{
	spawn_tanks_randomly();
}

void GameController::update()
{
	for (int i = 0; i < projectiles_.size(); ++i)
	{
		if (projectiles_[i].isExpired())
		{
			projectiles_.removeFast(i);
			// removeFast places last element in position of removed element
			// So we need to decrement 'i', to not miss it
			i--;
		}
		else
		{
			projectiles_[i].update();
		}
	}
}

void GameController::shutdown() {}

void GameController::processShot(const Unigine::Math::Vec3 &gun_point, const Unigine::Math::Vec3 &direction, float max_distance,
	int damage, TankController *shooter)
{
	projectiles_.append(Projectile(gun_point, direction, max_distance, damage, shot_intersections_mask.get(), shooter));
}

// Divides all game field into grid. Places the tanks in cells of the grid.
// To prevent tanks from placing in the same position, hashset of cells is used.
void GameController::spawn_tanks_randomly()
{
	Unigine::HashSet<Unigine::Math::ivec2> engaged_cells;
	engaged_cells.reserve(team1_tanks_count + team2_tanks_count + 1);

	{
		spawn_tank_randomly_uniquely_to_grid(player_tank, engaged_cells);
	}

	for (int i = 0; i < team1_tanks_count; i++)
	{
		spawn_tank_randomly_uniquely_to_grid(team1_tank, engaged_cells);
	}

	for (int i = 0; i < team2_tanks_count; i++)
	{
		spawn_tank_randomly_uniquely_to_grid(team2_tank, engaged_cells);
	}
}

void GameController::spawn_tank_randomly_uniquely_to_grid(const char *tank_file,
	Unigine::HashSet<Unigine::Math::ivec2> &engaged_cells)
{
	auto tank_node = Unigine::World::loadNode(tank_file);
	Unigine::ComponentSystem::get()->getComponent<TankController>(tank_node)->game_controller_node = node;

	Unigine::Math::ivec2 cells_count;
	cells_count.x = int(field_size.get().x / grid_cell_size.get());
	cells_count.y = int(field_size.get().y / grid_cell_size.get());

	Unigine::Math::ivec2 tank_cell;
	int i;
	for (i = 0; i < MAX_SPAWN_TRIES; i++)
	{
		tank_cell.x = Unigine::Math::Random::getRandom().getInt(0, cells_count.x);
		tank_cell.y = Unigine::Math::Random::getRandom().getInt(0, cells_count.y);
		if (!engaged_cells.contains(tank_cell))
		{
			break;
		}
	}
	if (i == MAX_SPAWN_TRIES)
	{
		Unigine::Log::error("Failed to spawn a tank\n");
		return;
	}

	engaged_cells.insert(tank_cell);

	Unigine::Math::Vec3 tank_position(getFieldStartPoint());
	tank_position.x += tank_cell.x * grid_cell_size;
	tank_position.y += tank_cell.y * grid_cell_size;

	tank_node->setWorldPosition(tank_position);
	tank_node->rotate(0.f, 0.f, Unigine::Math::Random::getRandom().getFloat(0, 360.f));
}

Unigine::Math::Vec2 GameController::getFieldStartPoint()
{
	return {node->getPosition().x, node->getPosition().y};
}

Unigine::Math::Vec2 GameController::getFieldEndPoint()
{
	Unigine::Math::Vec2 end_point = getFieldStartPoint();
	end_point.x += field_size.get().x;
	end_point.y += field_size.get().y;
	return end_point;
}
