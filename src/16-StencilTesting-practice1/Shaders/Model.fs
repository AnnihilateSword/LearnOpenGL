#version 330 core
out vec4 FragColor;

in vec2 varingTexCoords;;

uniform sampler2D uTextureDiffuse1;

void main()
{    
    FragColor = texture(uTextureDiffuse1, varingTexCoords);
}