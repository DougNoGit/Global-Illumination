#version 330 core
layout (location = 0) out vec4 gPositions;
layout (location = 1) out vec4 gNormals;

in vec3 fragPos;
in vec3 fragNor;

void main()
{
    gPositions = vec4(fragPos,1);
    gNormals = vec4(normalize(fragNor),1);
}
