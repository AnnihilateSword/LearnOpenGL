#version 330 core
out vec4 FragColor;

in vec3 varingNormal;
// 片段在世界空间中的位置（进行所有的光照计算）
in vec3 varingFragPos;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

void main()
{
   // 1. ambient
   float ambientStrength = 0.1f;
   vec3 ambient = ambientStrength * uLightColor;

   // 2. diffuse
   vec3 norm = normalize(varingNormal);
   vec3 lightDir = normalize(uLightPos - varingFragPos);
   float diff = max(dot(norm, lightDir), 0.0f);
   vec3 diffuse = diff * uLightColor;

   // 3. Specular
   float specularStrength = 0.5f;
   vec3 viewDir = normalize(uViewPos - varingFragPos);
   vec3 reflectDir = reflect(-lightDir, norm);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);
   vec3 specular = specularStrength * spec * uLightColor;

   vec3 result = (ambient + diffuse + specular) * uObjectColor;

   FragColor = vec4(result, 1.0f);
}