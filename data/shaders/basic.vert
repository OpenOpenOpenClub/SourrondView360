#version 330 core
layout (location = 0) in vec3 aPos; // 顶点位置属性

// uniform mat4 model;      // 模型矩阵 (后续添加)
// uniform mat4 view;       // 视图矩阵 (后续添加)
// uniform mat4 projection; // 投影矩阵 (后续添加)

void main()
{
    // 暂时只输出顶点位置，不进行变换
    gl_Position = vec4(aPos, 1.0);
    // gl_Position = projection * view * model * vec4(aPos, 1.0); // 完整形式
}