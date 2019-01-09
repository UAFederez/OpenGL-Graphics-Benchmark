#version 330 core

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec3 vert_offs;
layout (location = 3) in vec3 vert_norm;
layout (location = 4) in mat4 mod_mat;

out vec3 VertexColor;
out vec3 FragPos;
out vec3 Normals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	VertexColor = vertexColor;
	mat4 model_matrices = model;

	FragPos = vec3(model_matrices * vec4(vertexPosition + vert_offs, 1.0f));
	Normals = vec3(model_matrices * vec4(vert_norm + vert_offs, 1.0f));

	gl_Position = projection * view * vec4(FragPos, 1.0);;
}