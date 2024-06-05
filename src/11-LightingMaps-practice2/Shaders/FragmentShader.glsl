#version 330 core

// 材质
struct Material
{
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

// 光照
struct Light
{
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
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
   vec3 ambient = uLight.ambient * vec3(texture(uMaterial.diffuse, varingTexCoords));

   // 2. diffuse
   vec3 norm = normalize(varingNormal);
   vec3 lightDir = normalize(uLight.position - varingFragPos);
   float diff = max(dot(norm, lightDir), 0.0f);
   vec3 diffuse = uLight.diffuse * diff * vec3(texture(uMaterial.diffuse, varingTexCoords));

   // 3. Specular
   vec3 viewDir = normalize(uViewPos - varingFragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
   // 练习2：翻转镜面光
   vec3 specular = uLight.specular * spec * (vec3(1.0f) - vec3(texture(uMaterial.specular, varingTexCoords)));

   vec3 result = (ambient + diffuse + specular);

   FragColor = vec4(result, 1.0f);
}