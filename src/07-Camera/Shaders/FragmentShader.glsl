#version 330 core
out vec4 FragColor;

in vec2 varingTexCoord;

// texture sampler2D
uniform sampler2D uTexture;
uniform sampler2D uTexture2;

void main()
{
   FragColor = mix(texture(uTexture, varingTexCoord), texture(uTexture2, varingTexCoord), 0.2f);
}