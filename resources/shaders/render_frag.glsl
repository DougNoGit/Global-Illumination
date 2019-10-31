#version 330 core
uniform sampler2D VPLpositions1;
uniform sampler2D VPLcolors1;
uniform sampler2D VPLpositions2;
uniform sampler2D VPLcolors2;
uniform int VPLresolution;
uniform vec3 baseColor;
uniform vec3 lightPos;
in vec3 fragNor;
in vec3 fragPos;
layout(location = 0) out vec4 color;

#define VPLdistanceFalloffScalar 0.20
#define VPLdistanceFalloffExponent 1.5
#define VPLintensityScalar 60
#define VPLbaseColorScalar 0.05
#define VPLcolorScalar 0.95

void main()
{
	float distanceCoefficient, diffuseCoefficient;
	vec4 currentVPLPos, currentVPLColor;
	vec2 texCoords;

	color = vec4(0,0,0,1);
	vec3 normal = normalize(fragNor);

	// loop through all VPL's and "accumulate" light
	for(int i = 0; i < VPLresolution; i++)
	{
		for(int j = 0; j < VPLresolution; j++)
		{
			texCoords = vec2(float(i)/float(VPLresolution),float(j)/float(VPLresolution));
			currentVPLPos = (texture(VPLpositions1, texCoords) - vec4(0.5)) * 50.0;
			currentVPLColor = texture(VPLcolors1, texCoords);

			distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
			diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
			color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
            
            texCoords = vec2(float(i)/float(VPLresolution),float(j)/float(VPLresolution));
            currentVPLPos = (texture(VPLpositions2, texCoords) - vec4(0.5)) * 50.0;
            currentVPLColor = texture(VPLcolors2, texCoords);
            
            distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
            diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
            color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
			
		}
	}
	color = VPLintensityScalar*(color / pow(float(VPLresolution),2));
	//color += 0.15 * vec4(baseColor * clamp(dot(normalize(lightPos - fragPos), normal),0,1),1);
}
