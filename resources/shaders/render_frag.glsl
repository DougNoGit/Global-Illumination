#version 330 core
uniform sampler2D VPLcolors[6];
uniform sampler2D VPLpositions[6];
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

            for(int k = 0; k < 6; k++)
            {
			    currentVPLPos = (texture(VPLpositions[k], texCoords) - vec4(0.5)) * 50.0;
			    currentVPLColor = texture(VPLcolors[k], texCoords);

			    distanceCoefficient = clamp(VPLdistanceFalloffScalar / (pow(distance(fragPos, currentVPLPos.xyz), VPLdistanceFalloffExponent)), 0, 1);
			    diffuseCoefficient = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1.5 ,0,1);
			    color += vec4(((VPLcolorScalar*currentVPLColor.xyz + VPLbaseColorScalar*baseColor)),1) * diffuseCoefficient * distanceCoefficient;
            }
		}
	}
	color = VPLintensityScalar*(color / pow(float(VPLresolution),2));
	//color += 0.15 * vec4(baseColor * clamp(dot(normalize(lightPos - fragPos), normal),0,1),1);
}
