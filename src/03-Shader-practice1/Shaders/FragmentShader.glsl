#version 330 core
out vec4 FragColor;

in vec3 varingColor;

void main()
{
   FragColor = vec4(varingColor, 1.0f);
}