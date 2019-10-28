#version 330 core
layout (location = 0) out vec4 VPLpositions;
layout (location = 1) out vec4 VPLcolors;
uniform vec3 baseColor;

in vec3 fragPos;
in vec3 fragNor;

uniform vec3 lightPos;

void main()
{
	float diffuseCoefficient = pow(dot(normalize(lightPos - fragPos), normalize(fragNor)), 0.5);
	float distanceScalar = 1.0 / distance(lightPos, fragPos);

	VPLpositions = vec4((abs(fragPos/100)) + vec3(.5,.5,.5), 1);
	VPLcolors = vec4(diffuseCoefficient * distanceScalar * baseColor, 1);
}
