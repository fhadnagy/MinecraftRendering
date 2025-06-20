#version 430

// pipeline-b�l bej�v� per-fragment attrib�tumok 
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 textureCoords;

// kimen� �rt�k - a fragment sz�ne 
out vec4 outputColor;

// text�ra mintav�telez� objektum 
uniform sampler2D textureImage;


void main()
{
	float lightintese = 0.8 + dot(normalize(worldNormal),vec3(0.1,1,0.1))*0.2;

	outputColor = texture(textureImage, textureCoords)*lightintese;
}
