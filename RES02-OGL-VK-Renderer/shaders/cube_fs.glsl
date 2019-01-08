#version 330 core

in vec3 VertexColor;
in vec3 Normals;
in vec3 FragPos;

out vec4 FragmentColor;

uniform float	time;
uniform vec3	light_pos;

void main()
{
	float lerp		= cos(time) + 1.0 * 0.5;
	vec3 obj_color	= mix(vec3(VertexColor.y, VertexColor.z, VertexColor.x), 
						  vec3(0.3, 0.5, VertexColor.y), lerp);
	vec3 light_col	= vec3(0.9f, 0.9f, 0.3f);

	vec3 ambient	= vec3(0.2, 0.2, 0.8);

	vec3 norm		= normalize(Normals);
	vec3 lightDir	= normalize(light_pos - FragPos);
	float diff_fac	= max(dot(norm, lightDir), 0.0);

	vec3 diffuse	= vec3(1.0) * diff_fac * light_col;

	float constant	= 1.0f;
	float linear	= 0.0014f;
	float quadratic = 0.000007f;

	const float strength = 2.0f;

	float dist		= length(light_pos - FragPos);
	float attenuate = 1.0 / (constant + linear * dist + quadratic * (dist * dist));

	ambient *= attenuate * strength;
	diffuse *= attenuate * strength;

	vec3 result		= (ambient + diffuse) * obj_color;
	FragmentColor = vec4(result, 1.0f);
}