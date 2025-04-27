#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;   // 接收法线 (即使暂时不用)
layout (location = 2) in vec2 aTexCoord; // 接收纹理坐标

out vec2 TexCoord; // 传递给片段着色器的纹理坐标
// out vec3 FragPos;  // 传递给片段着色器的世界坐标 (用于光照)
// out vec3 Normal;   // 传递给片段着色器的法线 (用于光照)

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord; // 直接传递纹理坐标

    // 如果需要光照，取消以下注释
    // FragPos = vec3(model * vec4(aPos, 1.0));
    // Normal = mat3(transpose(inverse(model))) * aNormal; // 法线矩阵变换
}