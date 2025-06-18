#version 430

// pipeline-ból bejövõ per-fragment attribútumok 
in vec3 color;

// kimenõ érték - a fragment színe 
out vec4 outputColor;

void main()
{
	outputColor = vec4( color, 1.0 );
}
