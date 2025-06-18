#version 430

// Output to fragment shader
out vec3 color;

// Uniforms
uniform mat4 world;
uniform mat4 viewProj;
uniform vec3 minCorner;

// 12 edges of the cube = 24 vertices (each line has a start and end)
const vec4 positions[24] = vec4[24](
	// Bottom square
	vec4(0, 0, 0, 1), vec4(1, 0, 0, 1),
	vec4(1, 0, 0, 1), vec4(1, 0, 1, 1),
	vec4(1, 0, 1, 1), vec4(0, 0, 1, 1),
	vec4(0, 0, 1, 1), vec4(0, 0, 0, 1),

	// Top square
	vec4(0, 1, 0, 1), vec4(1, 1, 0, 1),
	vec4(1, 1, 0, 1), vec4(1, 1, 1, 1),
	vec4(1, 1, 1, 1), vec4(0, 1, 1, 1),
	vec4(0, 1, 1, 1), vec4(0, 1, 0, 1),

	// Vertical edges
	vec4(0, 0, 0, 1), vec4(0, 1, 0, 1),
	vec4(1, 0, 0, 1), vec4(1, 1, 0, 1),
	vec4(1, 0, 1, 1), vec4(1, 1, 1, 1),
	vec4(0, 0, 1, 1), vec4(0, 1, 1, 1)
);

// All lines get the same highlight color (yellow)
const vec3 lineColor = vec3(1, 1, 0); // yellow

void main()
{
	vec4 pos = positions[gl_VertexID] + vec4(minCorner, 0.0);
	gl_Position = viewProj * world * pos;
	color = lineColor;
}
