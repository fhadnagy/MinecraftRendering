#version 430

in vec3 color;
// kimenõ érték - a fragment színe 
out vec4 outputColor;

void main()
{
	vec2 uv = gl_PointCoord * 2.0 - 1.0; // Convert from [0,1] to [-1,1]

    float thickness = 0.15; // How thick the cross is

    // Create a cross shape using abs(x) or abs(y)
    if (!(abs(uv.x) < thickness || abs(uv.y) < thickness)) {
        discard; // outside cross shape
    }

    outputColor = vec4(1.0, 1.0, 1.0, 1.0); // white cross
}
