// filepath: /home/wind/Projects/AdasSourrondView/src/rendering/renderer.h
#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h> // NOLINT
////
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <memory> // 用于 unique_ptr
#include <string>

// 前向声明 Shader 类，避免在头文件中包含 shader.h (如果不需要完整定义)
class Shader;

class Renderer {
public:
  // 构造函数，可以接收数据路径等初始化参数
  Renderer(const std::string &dataPath);
  ~Renderer(); // 析构函数用于清理

  // 初始化 Renderer (加载着色器、设置 VAO/VBO 等)
  bool init();

  // 执行绘制操作
  void draw();

  // 清理 OpenGL 资源
  void cleanup();

private:
  std::string m_dataPath;                // 存储数据路径
  std::unique_ptr<Shader> m_basicShader; // 使用智能指针管理 Shader
  unsigned int m_triangleVAO;
  unsigned int m_triangleVBO;

  // 私有辅助函数 (可选)
  bool setupTriangle(); // 用于设置三角形的 VAO/VBO
};

#endif // RENDERER_H