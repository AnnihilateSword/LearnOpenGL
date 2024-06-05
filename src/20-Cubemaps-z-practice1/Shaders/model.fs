#version 330 core
out vec4 FragColor;

struct Material
{
   sampler2D TextureDiffuse1;
   sampler2D TextureSpecular1;
   sampler2D TextureNormal1;
   sampler2D TextureHeight1;
};

in vec3 varingNormal;
in vec3 varingFragPos;
in vec2 varingTexCoords;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform Material uMaterial;

void main()
{
    vec3 viewDir = normalize(cameraPos - varingFragPos);
    vec3 normal = normalize(varingNormal);

    // 漫反射
    float diff = max(normalize(dot(normal, viewDir)), 0.0f);
    vec3 diffuse = diff * texture(uMaterial.TextureDiffuse1, varingTexCoords).rgb;
    // 环境反射
    vec3 R = reflect(-viewDir, normal);
    vec3 reflectMap = texture(uMaterial.TextureHeight1, varingTexCoords).rgb;
    vec3 reflection = texture(skybox, R).rgb * reflectMap;

    vec3 result = diffuse + reflection;

    FragColor = vec4(result, 1.0f);
}