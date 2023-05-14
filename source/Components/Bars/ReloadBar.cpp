#include "Components/Bars/ReloadBar.h"

#include "Components/TankController.h"

using Unigine::ComponentSystem;

REGISTER_COMPONENT(ReloadBar);

void ReloadBar::init()
{
	tank_controller_ = ComponentSystem::get()->getComponent<TankController>(tank_node);
	billboard_ = Unigine::checked_ptr_cast<Unigine::ObjectBillboards>(node);

	assert(tank_controller_ != nullptr);
	assert(billboard_ != nullptr);
}

void ReloadBar::postUpdate()
{
	float billboard_width = tank_controller_->getRemainingReloadTime() / tank_controller_->reload_time * max_width;
	billboard_->setWidth(0, billboard_width);
}
