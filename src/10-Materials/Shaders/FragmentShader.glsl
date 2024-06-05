#version 330 core

// 材质
struct Material
{
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
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

uniform vec3 uViewPos;
uniform Material uMaterial;
uniform Light uLight;

void main()
{
   // 1. ambient
   vec3 ambient = uLight.ambient * uMaterial.ambient;

   // 2. diffuse
   vec3 norm = normalize(varingNormal);
   vec3 lightDir = normalize(uLight.position - varingFragPos);
   float diff = max(dot(norm, lightDir), 0.0f);
   vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse);

   // 3. Specular
   vec3 viewDir = normalize(uViewPos - varingFragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
   vec3 specular = uLight.specular * (spec * uMaterial.specular);

   vec3 result = (ambient + diffuse + specular);

   FragColor = vec4(result, 1.0f);
}