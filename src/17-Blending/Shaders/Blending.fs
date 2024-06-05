#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
    // // 1. 丢弃片段的方式，不太推荐（虽然直接丢弃片段很好，但它不能让我们渲染半透明的图像。）
    // vec4 texColor = texture(texture1, TexCoords);
    // if (texColor.a < 0.1f)
    //     discard;

    // FragColor = texColor;

    FragColor = texture(texture1, TexCoords);
}