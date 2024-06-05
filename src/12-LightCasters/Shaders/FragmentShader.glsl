#version 330 core

// 材质
struct Material
{
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

// 聚光灯
struct Light
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
uniform Light uLight;

void main()
{
   // 1. ambient
   vec3 ambient = uLight.ambient * texture(uMaterial.diffuse, varingTexCoords).rgb;

   // 2. diffuse
   vec3 norm = normalize(varingNormal);
   vec3 lightDir = normalize(uLight.position - varingFragPos);
   // 目前使用的光照计算需求一个从片段至光源的光线方向
   float diff = max(dot(norm, lightDir), 0.0f);
   vec3 diffuse = uLight.diffuse * diff * texture(uMaterial.diffuse, varingTexCoords).rgb;

   // 3. Specular
   vec3 viewDir = normalize(uViewPos - varingFragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), uMaterial.shininess);
   vec3 specular = uLight.specular * spec * texture(uMaterial.specular, varingTexCoords).rgb;

   // 4. spotlight（软边缘）
   float theta = dot(lightDir, normalize(-uLight.direction));
   float epsilon = uLight.cutOff - uLight.outerCutOff;
   float intensity = clamp((theta - uLight.outerCutOff) / epsilon, 0.0f, 1.0f);
   diffuse *= intensity;
   specular *= intensity;

   // 5. Attenuation（衰减）
   float distance = length(uLight.position - varingFragPos);
   float attenuation = 1.0f / (uLight.constant + uLight.linear*distance + uLight.quadratic*(distance*distance));
   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;

   vec3 result = ambient + diffuse + specular;
   FragColor = vec4(result, 1.0f);
}