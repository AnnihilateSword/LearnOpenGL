#version 330 core
out vec4 FragColor;

in vec3 varingColor;
in vec2 varingTexCoord;

// texture sampler2D
uniform sampler2D uTexture;
uniform sampler2D uTexture2;

void main()
{
   FragColor = mix(texture(uTexture, varingTexCoord), texture(uTexture2, vec2(1.0f-varingTexCoord.x, varingTexCoord.y)), 0.2f);
}