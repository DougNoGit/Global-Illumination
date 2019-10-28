#version 330 core
uniform sampler2D VPLpositions;
uniform sampler2D VPLcolors;
uniform int VPLresolution;
uniform vec3 baseColor;
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;

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
			currentVPLPos = (texture(VPLpositions, texCoords) - vec4(0.5)) * 50.0;
			currentVPLColor = texture(VPLcolors, texCoords);

			distanceScalar = 5.0 / (distance(fragPos, currentVPLPos.xyz));
			diffuseScalar = clamp(dot(normalize(currentVPLPos.xyz - fragPos), normal),0,1);
			//if(distance(currentVPLPos.xyz, fragPos) < 1){
				color += currentVPLColor * diffuseScalar * distanceScalar;
			//}
		}
	}
	color = (color / pow(float(VPLresolution),2));
	color += vec4(baseColor * clamp(dot(normalize(vec3(0,-3,0) - fragPos), normal),0,1),1);
}
