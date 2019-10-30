#version 330 core
layout (location = 0) out vec4 VPLpositions;
layout (location = 1) out vec4 VPLcolors;
uniform vec3 baseColor;

in vec3 fragPos;
in vec3 fragNor;

uniform vec3 lightPos;

void main()
{
	float diffuseCoefficient = clamp(dot(normalize(lightPos - fragPos), normalize(fragNor)), 0, 1);
	float distanceScalar = 5.0 * clamp((1 / distance(lightPos, fragPos)), 0, 1);

	vec4 diffuse = vec4(diffuseCoefficient * distanceScalar * baseColor, 1);
	vec4 ambient = diffuse * 0.8;

	// TODO plz fix
	VPLpositions = vec4((fragPos/50.0) + vec3(.5,.5,.5), 1);
	VPLcolors = vec4(diffuseCoefficient * distanceScalar * baseColor, 1);
}
