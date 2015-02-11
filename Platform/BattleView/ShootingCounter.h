// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef ShootingMarker_H
#define ShootingMarker_H

#include "BattleModel/BattleSimulator.h"
#include "Shapes/VertexShape.h"
#include "Audio/SoundPlayer.h"


class ShootingCounter
{
public:
	struct Projectile
	{
		glm::vec3 position1{};
		glm::vec3 position2{};
		float time{};
		float duration{};
	};

	MissileType _missileType{};
	std::vector<Projectile> _projectiles{};
	SoundCookieID _soundCookie{};

public:
	ShootingCounter(MissileType missileType);
	~ShootingCounter();

	bool Animate(float seconds);
	void AddProjectile(glm::vec3 position1, glm::vec3 position2, float delay, float duration);

	void Render(VertexShape_3f_4f* vertices);

private:
	void RenderArrow(VertexShape_3f_4f* vertices, glm::vec3 p1, glm::vec3 p2, float t);
	void RenderBullet(VertexShape_3f_4f* vertices, glm::vec3 p1, glm::vec3 p2, float t);
};


#endif
