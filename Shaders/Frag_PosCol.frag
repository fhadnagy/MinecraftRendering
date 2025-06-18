#version 430 core


out vec4 FragColor;

void main()
{
    vec2 uv = gl_PointCoord; // from 0 to 1 across the point
    float lineWidth = 0.05;  // thickness of cross lines

    // Draw vertical and horizontal lines intersecting at center (0.5, 0.5)
    if (abs(uv.x - 0.5) < lineWidth || abs(uv.y - 0.5) < lineWidth) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // red color
    } else {
        discard; // transparent outside crosshair
    }
}
