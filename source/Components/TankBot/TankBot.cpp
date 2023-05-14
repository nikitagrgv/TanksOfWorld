#include "TankBot.h"

#include "Components/GameController.h"
#include "Components/TankController.h"

#include <UnigineGame.h>

REGISTER_COMPONENT(TankBot);

void TankBot::init()
{
	tank_controller_ = Unigine::ComponentSystem::get()->getComponent<TankController>(node);
	assert(tank_controller_ != nullptr);

	game_controller_ = tank_controller_->getGameController();
	assert(game_controller_ != nullptr);

	frustum_projection_ = Unigine::Math::perspective(fov.get(), 2, 2, max_visible_distance);

	state_->onSwitchToState();
}

void TankBot::update()
{
	state_->update();
}

void TankBot::setState(BotState *new_state)
{
	state_.reset(new_state);
	state_->onSwitchToState();
}
