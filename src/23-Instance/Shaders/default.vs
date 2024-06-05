#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec3 Color;

void main()
{
    // 使用实例化渲染时可以使用 gl_InstanceID，从 0 开始索引
    vec2 pos = aPos * (gl_InstanceID / 100.0f);
    Color = aColor;
    gl_Position = vec4(pos + aOffset, 0.0f, 1.0f);
}