#version 330 core
layout (location = 0) out vec4 VPLpositions;
layout (location = 1) out vec4 VPLcolors;
uniform vec3 baseColor;

in vec3 fragPos;
in vec3 fragNor;

uniform vec3 lightPos;

#define VPLdistanceFalloffScalar 0.25
#define VPLdistanceFalloffExponent 0.70
#define VPLintensityScalar 5.0

void main()
{
	float diffuseCoefficient = clamp(dot(normalize(lightPos - fragPos), normalize(fragNor)), 0, 1);
	float distanceCoefficient = VPLintensityScalar * clamp((1.0 / pow(distance(lightPos, fragPos), VPLdistanceFalloffExponent)), 0, 1);

	// TODO plz fix
	VPLpositions = vec4((fragPos/50.0) + vec3(0.5), 1);
	VPLcolors = vec4(diffuseCoefficient * distanceCoefficient * baseColor, 1);
}
