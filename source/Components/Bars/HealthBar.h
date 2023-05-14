#pragma once

#include <UnigineComponentSystem.h>
#include <UnigineObjects.h>

class TankController;

class HealthBar : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(HealthBar, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_POST_UPDATE(postUpdate);

	PROP_PARAM(Node, tank_node);
	PROP_PARAM(Float, max_width, 4.0f);

protected:
	void init();
	void postUpdate();

private:
	TankController *tank_controller_ = nullptr;
	Unigine::ObjectBillboardsPtr billboard_ = nullptr;
};
