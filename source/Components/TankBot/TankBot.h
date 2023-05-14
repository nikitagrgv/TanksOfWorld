#pragma once

#include "Components/TankBot/BotState.h"
#include "Components/TankBot/BotStateFindEnemy.h"
#include "Components/TankBot/BotStateKillEnemy.h"

#include <memory>
#include <UnigineComponentSystem.h>

class TankController;
class GameController;

class TankBot : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(TankBot, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);

	PROP_GROUP("Visibility");
	PROP_PARAM(Float, fov, 60);
	PROP_PARAM(Float, max_visible_distance, 150);

public:
	void setState(BotState *new_state);

	Unigine::Math::Mat4 getFrustumProjection() const { return frustum_projection_; }

	GameController *getGameController() const { return game_controller_; };
	TankController *getTankController() const { return tank_controller_; };

protected:
	void init();
	void update();

private:
	GameController *game_controller_ = nullptr;
	TankController *tank_controller_ = nullptr;

	std::unique_ptr<BotState> state_{new BotStateFindEnemy(this)};

	Unigine::Math::Mat4 frustum_projection_;
};
