#include "Components/PlayerController.h"

#include "Components/TankController.h"

#include <UnigineConsole.h>

REGISTER_COMPONENT(PlayerController);

void PlayerController::init()
{
	tank_controller_ = Unigine::ComponentSystem::get()->getComponent<TankController>(node);
	assert(tank_controller_ != nullptr);

	find_all_children(node);
	init_cursor();
	init_player_camera();
}

void PlayerController::update()
{
	handle_control();
}

void PlayerController::postUpdate()
{
	move_cursor_to_mouse_position();
}

void PlayerController::shutdown()
{
	switch_to_free_camera();
	delete_cursor();
}

void PlayerController::find_all_children(Unigine::NodePtr &node)
{
	for (int i = 0; i < node->getNumChildren(); ++i)
	{
		auto child = node->getChild(i);
		tank_children_.append(child);
		find_all_children(child);
	}
}

void PlayerController::init_cursor()
{
	cursor_node_ = Unigine::World::loadNode(cursor_node_file);
	assert(cursor_node_.isValid());

	tank_controller_->setAimPosition(cursor_node_->getWorldPosition());
	tank_controller_->setUseAutoAiming(true);
}

void PlayerController::delete_cursor()
{
	cursor_node_.deleteLater();
}

void PlayerController::move_cursor_to_mouse_position()
{
	auto intersection = Unigine::WorldIntersection::create();

	Unigine::Math::Vec3 p0 = camera_node_->getWorldPosition();
	Unigine::Math::Vec3 p1 = p0 + camera_node_->getViewDirection() * cursor_max_distance;

	auto intersected = Unigine::World::getIntersection(p0, p1, cursor_intersections_mask.get(), tank_children_, intersection);
	if (!intersected.isNull())
	{
		cursor_node_->setPosition(intersection->getPoint());
	}
	else
	{
		cursor_node_->setPosition(p1);
	}

	tank_controller_->setAimPosition(cursor_node_->getWorldPosition());
}

void PlayerController::init_player_camera()
{
	assert(followed_node->isValid());

	camera_node_ = Unigine::PlayerPersecutor::create();
	camera_node_->setParent(followed_node);
	camera_node_->setTarget(followed_node);
	camera_node_->setMinDistance(camera_distance);
	camera_node_->setMaxDistance(camera_distance);
	camera_node_->setMinThetaAngle(min_camera_angle);
	camera_node_->setMaxThetaAngle(max_camera_angle);
	camera_node_->setMainPlayer(true);

	zoom_out();
}

void PlayerController::switch_to_free_camera()
{
	auto free_camera = Unigine::PlayerSpectator::create();
	free_camera->setMinVelocity(20.f);
	free_camera->setMaxVelocity(80.f);
	free_camera->setTransform(node->getTransform());
	free_camera->translate({0, 0, 15});
	free_camera->setMainPlayer(true);
	Unigine::ControlsApp::setMouseSensitivity(1);
}

void PlayerController::handle_control()
{
	using Unigine::Input;

	bool is_console_closed = !Unigine::Console::isActive();

	bool pressed_fire = is_console_closed && Input::isMouseButtonPressed(Input::MOUSE_BUTTON::MOUSE_BUTTON_LEFT);

	bool pressed_forward = is_console_closed && Input::isKeyPressed(Input::KEY_W);
	bool pressed_back = is_console_closed && Input::isKeyPressed(Input::KEY_S);
	bool pressed_left = is_console_closed && Input::isKeyPressed(Input::KEY_A);
	bool pressed_right = is_console_closed && Input::isKeyPressed(Input::KEY_D);

	// ========================
	// Firing
	tank_controller_->setGoingToFire(pressed_fire);

	// ========================
	// Movement
	if (!(pressed_forward ^ pressed_back))
	{
		tank_controller_->setHullMoving(TankController::HULL_MOVING_NONE);
	}
	else if (pressed_forward)
	{
		tank_controller_->setHullMoving(TankController::HULL_MOVING_FORWARD);
	}
	else
	{
		tank_controller_->setHullMoving(TankController::HULL_MOVING_BACK);
	}

	if (!(pressed_left ^ pressed_right))
	{
		tank_controller_->setHullRotating(TankController::HULL_ROTATING_NONE);
	}
	else if (pressed_left)
	{
		tank_controller_->setHullRotating(TankController::HULL_ROTATING_CLOCKWISE);
	}
	else
	{
		tank_controller_->setHullRotating(TankController::HULL_ROTATING_ANTICLOCKWISE);
	}

	// ========================
	// God mode
	if (is_console_closed && Input::isKeyDown(Input::KEY_F1))
	{
		tank_controller_->is_immortal = !tank_controller_->is_immortal;
	}

	// ========================
	// Suicide
	if (is_console_closed && Input::isKeyDown(Input::KEY_F2))
	{
		tank_controller_->kill();
	}

	// ========================
	// Zoom
	if (is_console_closed && Input::isKeyPressed(Input::KEY_ANY_SHIFT))
	{
		zoom_in();
	}
	else
	{
		zoom_out();
	}

}

void PlayerController::zoom_out()
{
	camera_node_->setFov(60);
	Unigine::ControlsApp::setMouseSensitivity(1);
}

void PlayerController::zoom_in()
{
	camera_node_->setFov(10);
	Unigine::ControlsApp::setMouseSensitivity(0.1);
}
