#version 330 core
out vec4 FragColor;

in vec2 TexCoord; // 从顶点着色器接收纹理坐标
// in vec3 FragPos; // 从顶点着色器接收世界坐标 (用于光照)
// in vec3 Normal;  // 从顶点着色器接收法线 (用于光照)

uniform sampler2D texture1; // 纹理采样器

// 可选：用于指示是否使用纹理
// uniform bool useTexture;

void main()
{
    // if (useTexture) {
         FragColor = texture(texture1, TexCoord); // 从纹理采样颜色
    // } else {
    //     FragColor = vec4(0.8, 0.8, 0.8, 1.0); // 默认灰色
    // }

    // 如果纹理有 Alpha 通道，可以考虑混合
    // if(FragColor.a < 0.1) discard; // 丢弃透明像素 (如果需要)
}