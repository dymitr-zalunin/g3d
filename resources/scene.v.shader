#version 150

uniform mat4 camera;
uniform mat4 model;

in vec3 vert;
in vec3 vertNormal;

out vec3 fragNormal;
out vec3 fragVert;

void main() {
    fragVert=vert;
    fragNormal=vertNormal;
    gl_Position = camera*model*vec4(vert, 1); //order multiplication : right to left
}