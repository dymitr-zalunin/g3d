#version 150

uniform mat4 camera;
uniform mat4 model;

in vec3 vert;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec3 fragNormal;
out vec3 fragVert;
out vec2 fragTexCoord;

void main() {
    fragVert=vert;
    fragNormal=vertNormal;
    fragTexCoord=vertTexCoord;
    gl_Position = camera*model*vec4(vert, 1); //order multiplication : right to left
}