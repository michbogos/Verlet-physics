# version 100

precision mediump float;

uniform vec3 viewPos;


// Input vertex attributes (from vertex shader)
varying vec3 fragPosition;
varying vec4 fragColor;
varying vec3 fragNormal;
varying vec2 fragTexCoord;

varying vec4 finalColor;

void main()
{
    vec4 res = vec4(fragTexCoord, 1.0, 1.0) * dot(fragNormal, normalize(viewPos));
    gl_FragColor = res;
}
