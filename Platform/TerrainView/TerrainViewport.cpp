#include "TerrainViewport.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>


TerrainViewport::TerrainViewport(GraphicsContext* gc) : Viewport(gc),
	_cameraTilt((float)M_PI_4),
	_cameraFacing(0),
	_flip(false)
{
}


TerrainViewport::~TerrainViewport()
{
}


glm::mat4 TerrainViewport::GetTransform() const
{
	return GetProjectionMatrix() * GetViewMatrix();
}


glm::mat4 TerrainViewport::GetProjectionMatrix() const
{
	float r = 2 * glm::length(_terrainBounds.size());

	glm::vec2 viewportSize = (glm::vec2)GetViewportBounds().size();
	float aspect = viewportSize.y / viewportSize.x;

	return glm::perspective(45.0f, 1.0f / aspect, 0.01f * r, r);
}


glm::mat4 TerrainViewport::GetViewMatrix() const
{
	glm::mat4 result = glm::lookAt(_cameraPosition, _cameraPosition + GetCameraDirection(), GetCameraUpVector());
	if (_flip)
		result = glm::scale(glm::mat4(), glm::vec3(-1.0f, -1.0f, 1.0f)) * result;
	return result;
}


bounds2f TerrainViewport::GetTerrainBounds() const
{
	return _terrainBounds;
}


void TerrainViewport::SetTerrainBounds(const bounds2f& value)
{
	_terrainBounds = value;
}


float TerrainViewport::GetContentRadius() const
{
	return _terrainBounds.y().size() / 2;
}


glm::vec3 TerrainViewport::GetCameraPosition() const
{
	return _cameraPosition;
}


void TerrainViewport::SetCameraPosition(const glm::vec3& value)
{
	_cameraPosition = value;
}


float TerrainViewport::GetCameraFacing() const
{
	return _cameraFacing;
}


void TerrainViewport::SetCameraFacing(float value)
{
	_cameraFacing = value;
}


glm::vec3 TerrainViewport::GetCameraDirection() const
{
	glm::quat q = glm::angleAxis(_cameraFacing, glm::vec3(0, 0, 1));
	glm::vec3 v(cosf(_cameraTilt), 0, -sinf(_cameraTilt));
	return q * v;
}



float TerrainViewport::GetCameraTilt() const
{
	return _cameraTilt;
}


void TerrainViewport::SetCameraTilt(float value)
{
	_cameraTilt = value;
}


glm::vec3 TerrainViewport::GetCameraUpVector() const
{
	glm::quat q = glm::angleAxis(_cameraFacing, glm::vec3(0, 0, 1));
	glm::vec3 v(sinf(_cameraTilt), 0, cosf(_cameraTilt));
	return q * v;
}



bool TerrainViewport::GetFlip() const
{
	return _flip;
}


void TerrainViewport::SetFlip(bool value)
{
	_flip = value;
}


glm::vec2 TerrainViewport::LocalToGlobal(glm::vec2 value) const
{
	return NormalizedToScreen(LocalToNormalized(value));
}


glm::vec2 TerrainViewport::GlobalToLocal(glm::vec2 value) const
{
	return NormalizedToLocal(ScreenToNormalized(value));
}


glm::vec2 TerrainViewport::LocalToNormalized(glm::vec2 value) const
{
	glm::ivec2 size = GetViewportBounds().size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(-1, -1);

	return 2.0f * value / (glm::vec2)size - 1.0f;
}


glm::vec2 TerrainViewport::NormalizedToLocal(glm::vec2 value) const
{
	glm::ivec2 size = GetViewportBounds().size();
	if (size.x == 0 || size.y == 0)
		return glm::vec2(0, 0);

	return (value + 1.0f) / 2.0f * (glm::vec2)size;
}
