#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;    
    // 透视除法是在顶点着色器运行之后执行的，将gl_Position的xyz坐标除以w分量
    // 当透视除法执行之后，z分量会变为w / w = 1.0。
    vec4 pos = projection * view * vec4(aPos, 1.0);
    // 让 z 分量永远等于 1.0
    gl_Position = pos.xyww;

    // 最终的标准化设备坐标将永远会有一个等于1.0的z值：最大的深度值。
    // 结果就是天空盒只会在没有可见物体的地方渲染了（只有这样才能通过深度测试，其它所有的东西都在天空盒前面）。
}