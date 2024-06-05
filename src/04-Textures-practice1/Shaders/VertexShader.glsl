#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 varingColor;
out vec2 varingTexCoord;

void main()
{
   gl_Position = vec4(aPos, 1.0f);
   varingColor = aColor;
   // can't varingTexCoord = aTexCoord
   varingTexCoord = vec2(aTexCoord.x, aTexCoord.y);
}