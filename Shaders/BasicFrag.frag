#version 430

// pipeline-ból bejövõ per-fragment attribútumok 
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 textureCoords;

// kimenõ érték - a fragment színe 
out vec4 outputColor;

// textúra mintavételezõ objektum 
uniform sampler2D textureImage;


void main()
{
	outputColor = texture(textureImage, textureCoords);
}
