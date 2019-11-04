#version 330 core
layout (location = 0) out vec4 gPositions;
layout (location = 1) out vec4 gNormals;
layout (location = 2) out vec4 baseColors;

uniform vec3 baseColor;

in vec3 fragPos;
in vec3 fragNor;

void main()
{
    // map down to 0,1 for positions with max value 50 or -50
    gPositions = vec4((fragPos),1);

    // map normals down to 0,1 from -1,1
    gNormals = vec4((normalize(fragNor) + vec3(1)) / 2.0,1);

    baseColors = vec4(baseColor, 1);
}
