#version 430

// pipeline-b�l bej�v� per-fragment attrib�tumok 
in vec3 color;

// kimen� �rt�k - a fragment sz�ne 
out vec4 outputColor;

void main()
{
	outputColor = vec4( color, 1.0 );
}
