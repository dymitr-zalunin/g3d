#version 150

uniform mat4 model;
uniform vec3 cameraPosition;
uniform int numTextures;
uniform sampler2D tex[10];
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

uniform struct Fog {
    vec4 color;
    float density;
    float start;
    float end;
    int eq;
} fog;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;
in vec4 viewCoord;

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

float fog_factor(Fog fog, float viewCoord) {
    float result=1.0;
    if (fog.eq==0) {
        result=exp(-fog.density*viewCoord);
    }else if (fog.eq==1) {
        result=exp(-pow(fog.density*viewCoord,2.0));
    }else if (fog.eq==2) {
        result=(fog.end-viewCoord)/(fog.end-fog.start);
    }
    result=1.0-clamp(result,0.0,1.0);
    return result;
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
        vec4 tcolor=vec4(0,0,0,0);
        for (int i=0; i<numTextures; i++) {
            vec4 t=texture(tex[i],fragTexCoord);
            tcolor=mix(tcolor,t,t.a);
        }
        surfaceColor=tcolor;
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
    vec4 tmpColor = vec4(pow(linearColor, gamma), surfaceColor.a);

    float view=abs(viewCoord.z/viewCoord.w);
    float factor=fog_factor(fog,view);
    finalColor = mix(tmpColor,fog.color,factor);

}