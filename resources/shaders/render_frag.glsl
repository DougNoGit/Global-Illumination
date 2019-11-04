#version 330 core
uniform sampler2D VPLpositions1;
uniform sampler2D VPLcolors1;
uniform sampler2D VPLpositions2;
uniform sampler2D VPLcolors2;
uniform sampler2D VPLpositions3;
uniform sampler2D VPLcolors3;
uniform sampler2D VPLpositions4;
uniform sampler2D VPLcolors4;
uniform sampler2D VPLpositions5;
uniform sampler2D VPLcolors5;
uniform sampler2D VPLpositions6;
uniform sampler2D VPLcolors6;
uniform sampler2D gPositions;
uniform sampler2D gNormals;
uniform sampler2D baseColors;
uniform int VPLresolution;
uniform vec3 lightPos;
in vec2 quadTexCoords;
layout(location = 0) out vec4 color;

#define VPLdistanceFalloffScalar 0.20
#define VPLdistanceFalloffExponent 1.8
#define VPLintensityScalar 20.0
#define VPLbaseColorScalar 0.05
#define VPLcolorScalar 0.95

void main()
{
    vec3 fragNor, fragPos, baseColor;
	float distanceCoefficient, diffuseCoefficient;
	vec4 currentVPLPos, currentVPLColor;
	vec2 texCoords;

	color = vec4(0,0,0,1);
    // "unpack" the mapping done in geometry_frag of the positions and normals
    fragPos = (texture(gPositions, quadTexCoords).xyz);
    fragNor = (texture(gNormals, quadTexCoords).xyz * 2.0) - vec3(1);
    baseColor = texture(baseColors, quadTexCoords).xyz;

	vec3 normal = normalize(fragNor);

	// loop through all VPL's and "accumulate" light
	for(int i = 0; i < VPLresolution; i++)
	{
		for(int j = 0; j < VPLresolution; j++)
		{
          
			texCoords = vec2(float(i)/float(VPLresolution),float(j)/float(VPLresolution));

            // 1
			currentVPLPos = (texture(VPLpositions1, texCoords) - vec4(0.5)) * 50.0;
			currentVPLColor = texture(VPLcolors1, texCoords);

			distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
			diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
			color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
            
            // 2
            currentVPLPos = (texture(VPLpositions2, texCoords) - vec4(0.5)) * 50.0;
            currentVPLColor = texture(VPLcolors2, texCoords);
            
            distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
            diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
            color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
            
            // 3
            currentVPLPos = (texture(VPLpositions3, texCoords) - vec4(0.5)) * 50.0;
            currentVPLColor = texture(VPLcolors3, texCoords);
            
            distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
            diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
            color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
            
            // 4
            currentVPLPos = (texture(VPLpositions4, texCoords) - vec4(0.5)) * 50.0;
            currentVPLColor = texture(VPLcolors4, texCoords);
            
            distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
            diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
            color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
            
            // 5
            currentVPLPos = (texture(VPLpositions5, texCoords) - vec4(0.5)) * 50.0;
            currentVPLColor = texture(VPLcolors5, texCoords);
            
            distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
            diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
            color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
            
            // 6
            currentVPLPos = (texture(VPLpositions6, texCoords) - vec4(0.5)) * 50.0;
            currentVPLColor = texture(VPLcolors6, texCoords);
            
            distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
            diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
            color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
			
		}
	}
	color = VPLintensityScalar*(color / pow(float(VPLresolution),2));
	//color += 0.15 * vec4(baseColor * clamp(dot(normalize(lightPos - fragPos), normal),0,1),1);
}
