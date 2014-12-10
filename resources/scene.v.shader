#version 150

uniform mat4 camera;
uniform mat4 model;

in vec3 vert;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec3 fragNormal;
out vec3 fragVert;
out vec2 fragTexCoord;
out vec4 viewCoord;

void main() {
    fragVert=vert;
    viewCoord=model*vec4(vert,1);
    fragNormal=vertNormal;
    fragTexCoord=vertTexCoord;
    gl_Position = camera*model*vec4(vert, 1); //order multiplication : right to left
}