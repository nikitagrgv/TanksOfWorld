#pragma once

#include <UnigineComponentSystem.h>
#include <UniginePlayers.h>

class TankController;

class PlayerController : public Unigine::ComponentBase
{
public:
	COMPONENT_DEFINE(PlayerController, ComponentBase);

	COMPONENT_INIT(init);
	COMPONENT_UPDATE(update);
	COMPONENT_POST_UPDATE(postUpdate);
	COMPONENT_SHUTDOWN(shutdown);

	PROP_GROUP("Player camera settings");
	PROP_PARAM(Node, followed_node);
	PROP_PARAM(Float, min_camera_angle, -20.f);
	PROP_PARAM(Float, max_camera_angle, 89.f);
	PROP_PARAM(Float, camera_distance, 9.f);

	PROP_GROUP("Player control settings");
	PROP_PARAM(File, cursor_node_file);
	PROP_PARAM(Float, cursor_max_distance, 800.f);
	PROP_PARAM(Mask, cursor_intersections_mask, 0b1);

protected:
	void init();
	void update();
	void postUpdate();
	void shutdown();

private:
	void find_all_children(Unigine::NodePtr &node);
	void init_cursor();
	void delete_cursor();
	void move_cursor_to_mouse_position();
	void init_player_camera();
	void switch_to_free_camera();
	void handle_control();
	void zoom_in();
	void zoom_out();

private:
	TankController *tank_controller_ = nullptr;
	Unigine::PlayerPersecutorPtr camera_node_ = nullptr;
	Unigine::NodePtr cursor_node_ = nullptr;
	Unigine::Vector<Unigine::NodePtr> tank_children_;
};
