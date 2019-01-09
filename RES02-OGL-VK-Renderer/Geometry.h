/* Geometry functions */
#pragma once

#include <iostream>
#include <vector>
#include <random>
#include <glm/glm.hpp>

constexpr float PI = 3.1415926f;

namespace geom
{
	std::vector<glm::vec3> InitPositions(const unsigned&);
	std::vector<glm::vec3> InitSpherePositions(const unsigned&);
	std::vector<glm::vec3> GenerateCube(const float&, std::vector<unsigned>&);
	std::vector<glm::vec3> GenerateSphere(const float&, const unsigned&, std::vector<unsigned>&);
};