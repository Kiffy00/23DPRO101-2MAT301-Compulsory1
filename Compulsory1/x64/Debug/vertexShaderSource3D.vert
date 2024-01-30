#version 330 core
layout (location = 0) in vec3 vertexPosition; // Position
layout (location = 1) in vec3 vertexColor;    // Color
out vec3 fragmentColor;
uniform float rotationAngle; // Uniform variable for rotation
void main()
{
    float cosAngle = cos(rotationAngle);
    float sinAngle = sin(rotationAngle);
    mat4 rotationMatrix = mat4(
        1.0,      0.0,       0.0, 0.0,
        0.0, cosAngle, -sinAngle, 0.0,
        0.0, sinAngle,  cosAngle, 0.0,
        0.0,      0.0,       0.0, 1.0
    );
    gl_Position = rotationMatrix * vec4(vertexPosition, 1.0);
    fragmentColor = vertexColor; // Pass color to fragment shader
}