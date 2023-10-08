#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec4 fragColor;
in vec3 fragNormal;
uniform vec3 viewPos;

out vec4 finalColor;

void main()
{
    finalColor = vec4(1.0, 1.0, 1.0, 1.0) * dot(fragNormal, normalize(viewPos));
}