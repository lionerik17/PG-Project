#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormal;
out vec4 fPosEye;
out vec2 fragTexCoords; // Add this line

uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

uniform mat4 airportModel;  // Airport transformation matrix
uniform mat4 airplaneModel; // Airplane transformation matrix
uniform int objectID; // 0 for airport, 1 for airplane

void main() 
{
    // compute eye space coordinates
    mat4 model;
    if (objectID == 0) {
        model = airportModel;
    } else if (objectID == 1) {
        model = airplaneModel;
    }
    fPosEye = view * model * vec4(vPosition, 1.0f);
    fNormal = normalize(normalMatrix * vNormal);
    fragTexCoords = vTexCoords; // Add this line
    gl_Position = projection * view * model * vec4(vPosition, 1.0f);
}
