#version 330 core

// 材质
struct Material
{
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

// 1. 方向光
struct DirLight
{
   vec3 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

// 2. 点光源
struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// 3. 聚光灯
struct SpotLight
{
   vec3 position;
   vec3 direction;
   float cutOff;
   float outerCutOff;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;
};

out vec4 FragColor;

in vec3 varingNormal;
// 片段在世界空间中的位置（进行所有的光照计算）
in vec3 varingFragPos;
in vec2 varingTexCoords;

uniform vec3 uViewPos;
uniform Material uMaterial;

// 光源
uniform DirLight uDirLight;
uniform PointLight uPointLights[4];
uniform SpotLight uSpotLight;

// 1. 计算方向光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
   vec3 lightDir = normalize(-light.direction);
   // 漫反射着色
   float diff = max(dot(normal, lightDir), 0.0f);
   // 镜面光着色
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);

   // 合并结果
   vec3 ambient = light.ambient * texture(uMaterial.diffuse, varingTexCoords).rgb;
   vec3 diffuse = light.diffuse * diff * texture(uMaterial.diffuse, varingTexCoords).rgb;
   vec3 specular = light.specular * spec * texture(uMaterial.specular, varingTexCoords).rgb;

   return ambient + diffuse + specular;
}

// 2. 计算点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // 合并结果
    vec3 ambient  = light.ambient  * texture(uMaterial.diffuse, varingTexCoords).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(uMaterial.diffuse, varingTexCoords).rgb;
    vec3 specular = light.specular * spec * texture(uMaterial.specular, varingTexCoords).rgb;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// 3. 计算聚光灯
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // 聚光灯强度
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // 合并结果
    vec3 ambient = light.ambient * vec3(texture(uMaterial.diffuse, varingTexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(uMaterial.diffuse, varingTexCoords));
    vec3 specular = light.specular * spec * vec3(texture(uMaterial.specular, varingTexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{
   vec3 norm = normalize(varingNormal);
   vec3 viewDir = normalize(uViewPos - varingFragPos);

   // 1. 定向光照
   vec3 result = CalcDirLight(uDirLight, norm, viewDir);
   // 2. 点光源
   for (int i = 0; i < 4; i++)
   {
      result += CalcPointLight(uPointLights[i], norm, varingFragPos, viewDir);
   }
   // 3. 聚光灯
   result += CalcSpotLight(uSpotLight, norm, varingFragPos, viewDir);

   FragColor = vec4(result, 1.0f);
}