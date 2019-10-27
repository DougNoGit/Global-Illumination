#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 fragNor;
out vec3 fragPos;

void main()
{
	gl_Position = P * V * M * vertPos;
	fragPos = (M * vec4(vertNor, 0.0)).xyz;
	fragNor = (M * vec4(vertNor, 0.0)).xyz;
}