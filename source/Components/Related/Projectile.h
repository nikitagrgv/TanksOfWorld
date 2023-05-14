#pragma once

#include <UnigineMathLibVec4.h>

class TankController;

class Projectile
{
public:
	Projectile(const Unigine::Math::Vec3 &fire_point, const Unigine::Math::Vec3 &direction,
		float distance, int damage, int shoot_intersection_mask, TankController *shooter_tank);

	void update();
	bool isExpired() const;

private:
	void destroy_projectile();

private:
	Unigine::Math::Vec3 fire_point_;
	Unigine::Math::Vec3 direction_;
	int damage_;
	int shoot_intersection_mask_;

	float lifetime_;
	float current_time_ = 0;

	TankController *shooter_tank_;

	Unigine::Math::Vec4 color_;
};
