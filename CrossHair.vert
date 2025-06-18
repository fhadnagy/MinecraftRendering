#version 430

// Variables going forward through the pipeline
out vec3 color;

// External parameters of the shader
uniform mat4 world;
uniform mat4 viewProj;

const vec4 positions[6] = vec4[6](
	// 1. segment (X)
	vec4( 0,  0, 0, 1),
	vec4( 1,  0, 0, 1),
	// 2. segment (Y)
	vec4( 0,  0, 0, 1),
	vec4( 0,  1, 0, 1),
	// 3. segment (Z)
	vec4( 0,  0, 0, 1),
	vec4( 0,  0, 1, 1)
);

const vec3 colors[6] = vec3[6](
	vec3(1, 0, 0),
	vec3(1, 0, 0),
	vec3(0, 1, 0),
	vec3(0, 1, 0),
	vec3(0, 0, 1),
	vec3(0, 0, 1)
);

void main()
{
	// https://registry.khronos.org/OpenGL-Refpages/gl4/html/gl_VertexID.xhtml
	gl_Position = viewProj * world * positions[gl_VertexID];
	color = colors[gl_VertexID];
}

