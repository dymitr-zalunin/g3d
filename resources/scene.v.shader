#version 150

uniform mat4 camera;
uniform mat4 model;
uniform vec4 viewDirection;
uniform vec3 cameraPosition;

in vec3 vert;
in vec3 vertNormal;
in vec2 vertTexCoord;

out vec3 fragNormal;
out vec3 fragVert;
out vec2 fragTexCoord;
out vec4 viewCoord;

const float PI = 3.1415926535;

vec4 Distort(vec4 p)
{
    vec2 v = p.xy / p.w;
    return p;
}

void main() {
    fragVert=vert;
    fragNormal=vertNormal;
    fragTexCoord=vertTexCoord;
    viewCoord=model*vec4(vert,1);

    vec3 surfaceToCamera = normalize(cameraPosition - vec3(viewCoord));
    float theta=acos(dot(surfaceToCamera,vec3(viewDirection)));
    float r=length(surfaceToCamera)*sin(theta);
    float coef=4*sin(theta/2)/r;

    viewCoord.x=viewCoord.x*coef;
    viewCoord.y=viewCoord.y*coef;

    gl_Position = camera*viewCoord; //order multiplication : right to left

}