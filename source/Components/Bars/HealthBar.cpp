#include "Components/Bars/HealthBar.h"

#include "Components/TankController.h"

using Unigine::ComponentSystem;

REGISTER_COMPONENT(HealthBar);

void HealthBar::init()
{
	tank_controller_ = ComponentSystem::get()->getComponent<TankController>(tank_node);
	billboard_ = Unigine::checked_ptr_cast<Unigine::ObjectBillboards>(node);

	assert(tank_controller_ != nullptr);
	assert(billboard_ != nullptr);
}

void HealthBar::postUpdate()
{
	float billboard_width = (float)tank_controller_->getHealth() / (float)tank_controller_->max_health * max_width;
	billboard_->setWidth(0, billboard_width);
}
