#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
    FragColor = texture(texture1, TexCoords);

    // 深度缓冲的可视化
    // FragColor = vec4(vec3(gl_FragCoord.z), 1.0f);
}