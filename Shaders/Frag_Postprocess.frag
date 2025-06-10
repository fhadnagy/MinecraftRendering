#version 430

// Per fragment variables coming from the pipeline
in vec2 textureCoords;

// Outgoing values - fragment color
out vec4 outputColor;

// External parameters of the shader
uniform sampler2D frameTex;
uniform float filter_weight;
uniform mat3 color_matrix = mat3( 0.393f, 0.349f, 0.272f, 
								  0.769f, 0.686f, 0.534f,
								  0.189f, 0.168f, 0.131f );

void main()
{
	vec4 originalColor = texture(frameTex, textureCoords);
	vec4 sepiaColored = vec4(
								color_matrix*originalColor.xyz, 
								1
							);
	outputColor = mix(originalColor, sepiaColored, filter_weight);
}
