#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormal;
out vec4 fPosEye;
out vec2 fragTexCoords; // Add this line

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() 
{
    // compute eye space coordinates
    fPosEye = view * model * vec4(vPosition, 1.0f);
    fNormal = normalize(normalMatrix * vNormal);
    fragTexCoords = vTexCoords; // Add this line
    gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}
