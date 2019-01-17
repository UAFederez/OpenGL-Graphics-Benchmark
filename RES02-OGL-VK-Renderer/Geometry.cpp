#include "Geometry.h"

std::vector<glm::vec3> geom::InitPositions(const unsigned& mass_count)
{
	std::vector<glm::vec3> positions;

	const float space = 3.0f, offset = space / 2.0f;

	for (unsigned x = 0; x < mass_count; x++)
	{
		for (unsigned y = 0; y < mass_count; y++)
		{
			for (unsigned z = 0; z < mass_count; z++)
			{
				glm::vec3 position = glm::vec3(
					float(space * x - (mass_count - offset)),
					float(space * y - (mass_count - offset)),
					float(space * z));
				positions.push_back(position);
			}
		}
	}

	return positions;
}

std::vector<glm::vec3> geom::InitSpherePositions(const unsigned& mass_count)
{
	std::vector<glm::vec3> positions;

	std::default_random_engine gen;
	auto randf = std::uniform_real_distribution<float>(0.0, 1.0);

	const unsigned segments = mass_count * 2;
	const unsigned rings = mass_count;

	const float latitude_step = PI / rings;
	const float longitude_step = (2 * PI) / segments;

	float phi, theta;
	float x_pos, y_pos, z_pos;

	const float inv_radius = 1 / float(mass_count);

	const float space = 3.0f;
	float offset = 10.0f;

	for (unsigned r = 0; r <= mass_count; r++)
	{
		for (unsigned y = 0; y <= rings; y++)
		{
			phi = (PI / 2) - y * latitude_step;

			for (unsigned x = 0; x <= segments; x++)
			{
				theta = x * longitude_step;

				x_pos = (r *  space) * sinf(theta) * sinf(phi) + randf(gen) * offset;
				y_pos = (r *  space) * cosf(theta) + randf(gen)* offset;
				z_pos = (r *  space) * sinf(theta) * cosf(phi) + randf(gen) * offset;

				glm::vec3 circle_pnt = glm::vec3(x_pos, y_pos, z_pos);

				positions.push_back(circle_pnt);
			}
		}
	}

	std::cout << "TOTAL POSITIONS: " << positions.size() << std::endl;

	return positions;
}

std::vector<glm::vec3> geom::GenerateSphere( const float& radius, const unsigned &subdivisions, std::vector<unsigned>& indices )
{
	std::vector<glm::vec3> vertices;

	assert(subdivisions > 0);

	const unsigned segments = subdivisions * 2;
	const unsigned rings = subdivisions;

	const float latitude_step = PI / rings;
	const float longitude_step = (2 * PI) / segments;

	float phi, theta;
	float x_pos, y_pos, z_pos;
	float x_nrm, y_nrm, z_nrm;
	float r_col, g_col, b_col;

	const float inv_radius = 1 / radius;

	for (unsigned y = 0; y <= rings; y++)
	{
		phi = (PI / 2) - y * latitude_step;

		for (unsigned x = 0; x <= segments; x++)
		{
			theta = x * longitude_step;

			x_pos = radius * sinf(theta) * sinf(phi);
			y_pos = radius * cosf(theta);
			z_pos = radius * sinf(theta) * cosf(phi);

			x_nrm = x_pos * inv_radius;
			y_nrm = y_pos * inv_radius;
			z_nrm = z_pos * inv_radius;

			r_col = abs(x_nrm);
			g_col = abs(y_nrm);
			b_col = abs(z_nrm);

			glm::vec3 circle_pnt = glm::vec3(x_pos, y_pos, z_pos);
			glm::vec3 circle_col = glm::vec3(r_col, g_col, b_col);
			glm::vec3 circle_nrm = glm::vec3(x_nrm, y_nrm, z_nrm);

			vertices.push_back(circle_pnt);
			vertices.push_back(circle_col);
			vertices.push_back(circle_nrm);
		}
	}

	std::cout << "unique verts: " << vertices.size() / 3 << std::endl;

	unsigned idx1, idx2;
	for (unsigned i = 0; i < rings; ++i)
	{
		idx1 = i * (segments + 1);
		idx2 = idx1 + segments + 1;

		for (unsigned j = 0; j < segments; ++j, ++idx1, ++idx2)
		{
			indices.push_back(idx1);
			indices.push_back(idx1 + 1);
			indices.push_back(idx2);

			indices.push_back(idx2);
			indices.push_back(idx2 + 1);
			indices.push_back(idx1 + 1);
		}

	}

	std::cout << "verts per sphere: " << indices.size() << std::endl;

	return vertices;
}

std::vector<glm::vec3> geom::GenerateCube(const float& length, std::vector<unsigned>& indices)
{
	const float side = length / 2.0f;

	std::vector<glm::vec3> cube_vertices = {
		glm::vec3(-side, -side, side),		glm::vec3(1.0, 0.0, 0.0),		glm::vec3(-1.0, -1.0, 1.0),
		glm::vec3( side, -side, side),		glm::vec3(0.0, 1.0, 0.0),		glm::vec3(1.0, -1.0, 1.0),
		glm::vec3( side,  side, side),		glm::vec3(0.0, 0.0, 1.0),		glm::vec3(1.0,  1.0, 1.0),
		glm::vec3(-side,  side, side),		glm::vec3(1.0, 0.0, 1.0),		glm::vec3(-1.0, 1.0, 1.0),
		glm::vec3( side,  side,-side),		glm::vec3(0.0, 1.0, 1.0),		glm::vec3(1.0,  1.0,-1.0),
		glm::vec3( side, -side,-side),		glm::vec3(1.0, 1.0, 0.0),		glm::vec3(1.0, -1.0,-1.0),
		glm::vec3(-side,  side,-side),		glm::vec3(1.0, 0.0, 0.0),		glm::vec3(-1.0, 1.0,-1.0),
		glm::vec3(-side, -side,-side),		glm::vec3(0.0, 1.0, 0.0),		glm::vec3(-1.0,-1.0,-1.0)
	};

	indices = {
		0, 1, 2, 2, 3, 0,	1, 5, 4, 4, 2, 1,	5, 7, 6, 6, 4, 5,
		7, 0, 3, 3, 6, 7,	3, 2, 4, 4, 6, 3,	7, 5, 1, 1, 0, 7
	};

	return cube_vertices;
}