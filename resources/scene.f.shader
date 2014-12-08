#version 150

uniform mat4 model;
uniform vec3 cameraPosition;
uniform sampler2D tex;
uniform float materialShininess;
uniform vec4 materialSpecularColor;
uniform vec4 materialDiffuseColor;
uniform vec4 materialAmbientColor;
uniform float useTexture;

#define MAX_LIGHTS 10
uniform int numLights;
uniform struct Light {
   vec4 position;
   vec3 intensities; //a.k.a the color of the light
   float attenuation;
   float ambientCoefficient;
   float coneAngle;
   vec3 coneDirection;
} lights[MAX_LIGHTS];

struct Material {
    vec4 specularColor;
    vec4 diffuseColor;
    vec4 ambientColor;
    float shininess;
} material;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 finalColor;

vec3 ApplyLight(Light light, vec3 surfaceColor, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera, Material material) {
    vec3 surfaceToLight;
    float attenuation = 1.0;
    if(light.position.w == 0.0) {
        //directional light
        surfaceToLight = normalize(light.position.xyz);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        surfaceToLight = normalize(light.position.xyz - surfacePos);
        float distanceToLight = length(light.position.xyz - surfacePos);
        attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

        //cone restrictions (affects attenuation)
        float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.coneDirection))));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.0;
        }
    }

    //ambient
    vec3 ambient = light.ambientCoefficient * material.ambientColor.rgb * light.intensities;

    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;

    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), material.shininess);
    vec3 specular = specularCoefficient * material.specularColor.rgb * light.intensities;

    //linear color (color before gamma correction)
    return ambient + attenuation*(diffuse + specular);
}

void main() {

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 normal=normalize(transpose(inverse(mat3(model)))*fragNormal);
    vec3 surfacePos=vec3(model*vec4(fragVert,1));
    vec4 surfaceColor=vec4(materialDiffuseColor);
    vec3 surfaceToCamera=normalize(cameraPosition-surfacePos);

    material.specularColor=materialSpecularColor;
    material.ambientColor=materialAmbientColor;
    material.diffuseColor=materialDiffuseColor;
    material.shininess=materialShininess;

    if (useTexture==1.0) {
        surfaceColor=texture(tex,fragTexCoord);
        material.specularColor=surfaceColor;
        material.ambientColor=surfaceColor;
        material.diffuseColor=surfaceColor;
        material.shininess=materialShininess+10;
    }

    vec3 linearColor = vec3(0);
    for (int i=0; i < numLights; ++i) {
        linearColor += ApplyLight(lights[i], surfaceColor.rgb, normal, surfacePos, surfaceToCamera, material);
    }

    //final color (after gamma correction)
    vec3 gamma = vec3(1.0/2.2);
    finalColor = vec4(pow(linearColor, gamma), surfaceColor.a);

}