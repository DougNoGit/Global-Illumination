#version 330 core
uniform sampler2D VPLpositions;
uniform sampler2D VPLcolors;
uniform int VPLresolution;
uniform vec3 baseColor;
uniform vec3 lightPos;
in vec3 fragNor;
in vec3 fragPos;
layout(location = 0) out vec4 color;

void main()
{
	float distanceScalar, diffuseScalar;
	vec4 currentVPLPos, currentVPLColor;
	vec2 texCoords;

	color = vec4(0,0,0,1);
	vec3 normal = normalize(fragNor);

	// regular diffuse lighting
	//diffuseScalar = (dot(normal, normalize(vec3(0,-3,0) - fragPos)) + 0.5);
	//color = vec4(diffuseScalar * vec3(1.0/distance(vec3(0,-3,0), fragPos)), 1);

	 //light based on VPLs
	for(int i = 0; i < VPLresolution; i++)
	{
		for(int j = 0; j < VPLresolution; j++)
		{
			texCoords = vec2(float(i)/float(VPLresolution),float(j)/float(VPLresolution));
			currentVPLPos = texture(VPLpositions, texCoords);
			currentVPLColor = texture(VPLcolors, texCoords);

			distanceScalar = clamp(0.25 / (pow(distance(fragPos, currentVPLPos.xyz), 2.5)), 0, 2);
			diffuseScalar = 1;clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal) + 1,0,1);
			color += vec4(((0.85*currentVPLColor.xyz + 0.15*baseColor)),1) * diffuseScalar * distanceScalar;
			
		}
	}
	color = 60*(color / pow(float(VPLresolution),2));
	//vec4 ambient = color * 0.5;
	//color += ambient;
	//color += 0.15 * vec4(baseColor * clamp(dot(normalize(lightPos - fragPos), normal),0,1),1);
}
