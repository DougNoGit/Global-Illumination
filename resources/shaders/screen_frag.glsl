#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D renderTexture;

void main()
{
	color = vec4(texture( renderTexture, texCoord ).rgb, 1);
}