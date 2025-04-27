#include "texture_utils.h"
#include <iostream>

// --- STB Image Implementation (确保只在一个 cpp 文件中定义) ---
#ifndef STB_IMAGE_IMPLEMENTATION_DEFINED // 防止重复定义
#define STB_IMAGE_IMPLEMENTATION_DEFINED
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif
// --- End STB Image ---

GLuint loadTexture(const char *path) {
  GLuint textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  // stbi_set_flip_vertically_on_load(true); // 如果纹理上下颠倒，取消注释此行
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;
    else {
      std::cerr << "Texture format not supported: " << path
                << " nrComponents: " << nrComponents << std::endl;
      stbi_image_free(data);
      glDeleteTextures(1, &textureID); // 清理已生成的纹理对象
      return 0;                        // 返回 0 表示失败
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 设置纹理环绕和过滤选项
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT); // 或者 GL_CLAMP_TO_EDGE 等
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR); // 使用 Mipmap 进行缩小过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR); // 放大过滤

    stbi_image_free(data);
    std::cout << "Texture loaded successfully: " << path << std::endl;
  } else {
    std::cerr << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data); // 即使 data 为空，调用也是安全的
    glDeleteTextures(1, &textureID); // 清理已生成的纹理对象
    return 0;                        // 返回 0 表示失败
  }

  glBindTexture(GL_TEXTURE_2D, 0); // 解绑纹理
  return textureID;
}