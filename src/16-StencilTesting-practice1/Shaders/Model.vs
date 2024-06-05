#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 varingTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform float uOutline;

void main()
{
    varingTexCoords = aTexCoords;

    vec3 pos = aPos + uOutline * normalize(aNormal);
    gl_Position = uProjection * uView * uModel * vec4(pos, 1.0f);
}