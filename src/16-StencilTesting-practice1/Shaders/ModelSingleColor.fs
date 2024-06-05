#version 330 core
out vec4 FragColor;

in vec2 varingTexCoords;;

uniform sampler2D uTextureDiffuse1;

void main()
{    
    FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}