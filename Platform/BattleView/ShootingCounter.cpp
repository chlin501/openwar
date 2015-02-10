// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#include "ShootingCounter.h"
#include "Audio/SoundPlayer.h"



ShootingCounter::ShootingCounter(MissileType missileType) :
	_missileType{missileType}
{
	if (missileType == MissileType::Arq)
		SoundPlayer::GetSingleton()->PlayMissileMatchlock();
	else
		_soundCookie = SoundPlayer::GetSingleton()->PlayMissileArrows();
}


ShootingCounter::~ShootingCounter()
{
}


void ShootingCounter::AddProjectile(glm::vec3 position1, glm::vec3 position2, float delay, float duration)
{
	Projectile projectile;
	projectile.position1 = position1;
	projectile.position2 = position2;
	projectile.time = -delay;
	projectile.duration = duration;

	_projectiles.push_back(projectile);
}


bool ShootingCounter::Animate(float seconds)
{
	bool alive = false;

	for (Projectile& projectile : _projectiles)
	{
		if (projectile.time < 0)
		{
			projectile.time += seconds;
			alive = true;
		}
		else if (projectile.time < projectile.duration)
		{
			projectile.time += seconds;
			if (projectile.time > projectile.duration)
				projectile.time = projectile.duration;
			alive = true;
		}
		else if (projectile.time == projectile.duration)
		{
			projectile.time += 1;
			alive = true;
		}
	}

	if (!alive)
	{
		SoundPlayer::GetSingleton()->StopMissileArrows(_soundCookie);
	}

	return alive;
}


void ShootingCounter::Render(VertexShape_3f_4f* vertices)
{
	for (ShootingCounter::Projectile& projectile : _projectiles)
	{
		float t = projectile.time / projectile.duration;
		if (0 <= t && t <= 1)
		{
			if (_missileType == MissileType::Bow)
				RenderArrow(vertices, projectile.position1, projectile.position2, t);
			else
				RenderBullet(vertices, projectile.position1, projectile.position2, t);
		}
	}
}


void ShootingCounter::RenderArrow(VertexShape_3f_4f* vertices, glm::vec3 p1, glm::vec3 p2, float t)
{
	float size = 4;
	glm::vec3 diff = p2 - p1;
	float distance = glm::length(diff);
	glm::vec3 dir = diff / distance;

	float dt = size / distance;
	float t1 = t * (1 - dt);
	float t2 = t1 + dt;
	float h1 = 0.3f * distance * (t1 - t1 * t1);
	float h2 = 0.3f * distance * (t2 - t2 * t2);

	glm::vec3 pp1 = p1 + t * (distance - size) * dir;
	glm::vec3 pp2 = pp1 + size * dir;

	pp1.z += h1;
	pp2.z += h2;


	vertices->AddVertex(Vertex_3f_4f(pp1, glm::vec4(0, 0, 0, 0.2)));
	vertices->AddVertex(Vertex_3f_4f(pp2, glm::vec4(0, 0, 0, 0.2)));
}


void ShootingCounter::RenderBullet(VertexShape_3f_4f* vertices, glm::vec3 p1, glm::vec3 p2, float t)
{
	float size = 50;
	glm::vec3 diff = p2 - p1;
	glm::vec3 dir = glm::normalize(diff);

	glm::vec3 pp1 = p1 + t * (glm::length(diff) - size) * dir;
	glm::vec3 pp2 = pp1 + size * dir;

	vertices->AddVertex(Vertex_3f_4f(pp1, glm::vec4(1, 1, 0, 0.2)));
	vertices->AddVertex(Vertex_3f_4f(pp2, glm::vec4(1, 1, 1, 0.2)));
}

