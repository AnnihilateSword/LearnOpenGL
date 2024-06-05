#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 varingNormal;
out vec3 varingFragPos;
out vec2 varingTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
   // 片段在世界空间中的位置（进行所有的光照计算）
   varingFragPos = vec3(model * vec4(aPos, 1.0f));
   varingNormal = mat3(transpose(inverse(model))) * aNormal;
   varingTexCoords = aTexCoords;
   
   // 顶点在经过 MVP 后的位置
   gl_Position = projection * view * model * vec4(aPos, 1.0f);
}