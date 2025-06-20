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
	float lightintese = 0.8 + dot(normalize(worldNormal),vec3(0.1,1,0.1))*0.2;

	outputColor = texture(textureImage, textureCoords)*lightintese;
}
