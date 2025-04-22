// filepath: /home/wind/Projects/AdasSourrondView/src/rendering/renderer.cpp
#include "renderer.h"
#include "shader.h"                     // 需要包含 Shader 的完整定义
#include <glm/gtc/matrix_transform.hpp> // 用于 glm::perspective 等
#include <iostream>

Renderer::Renderer(const std::string &dataPath)
    : m_dataPath(dataPath), m_triangleVAO(0), m_triangleVBO(0) {}

Renderer::~Renderer() {
  // cleanup() 函数会处理资源释放，这里通常为空
  // 或者确保 cleanup() 在对象销毁前被调用
}

bool Renderer::init() {
  // 1. 加载着色器
  // 注意调整着色器文件的相对路径！
  // 假设 data 目录与 build 目录同级
  std::string shaderBasePath =
      m_dataPath + "/../data/shaders/"; // 调整路径构造方式
  try {
    m_basicShader =
        std::make_unique<Shader>((shaderBasePath + "basic.vert").c_str(),
                                 (shaderBasePath + "basic.frag").c_str());
  } catch (const std::exception &e) {
    std::cerr << "Failed to create shader: " << e.what() << std::endl;
    return false;
  }

  if (!m_basicShader || m_basicShader->ID == 0) { // 检查着色器是否加载成功
    std::cerr << "Failed to load shaders in Renderer." << std::endl;
    return false;
  }
  std::cout << "Shaders loaded successfully in Renderer." << std::endl;

  // 2. 设置几何体 (调用私有辅助函数)
  if (!setupTriangle()) {
    std::cerr << "Failed to set up triangle geometry in Renderer." << std::endl;
    return false;
  }
  std::cout << "Triangle geometry set up successfully." << std::endl;

  return true; // 初始化成功
}

void Renderer::draw() {
  // 1. 清除屏幕
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 2. 激活着色器
  if (!m_basicShader)
    return; // 安全检查
  m_basicShader->use();

  // 3. 设置变换矩阵 (后续应由 CameraController 提供 view 和 projection)
  glm::mat4 model = glm::mat4(1.0f); // 模型矩阵 (单位矩阵，物体在原点)
  glm::mat4 view = glm::translate(
      glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f)); // 视图矩阵 (相机向后移动)
  // 获取当前窗口大小用于计算投影矩阵的宽高比
  int width, height;
  // 注意：这里直接访问全局 window 指针不太好，后续应通过参数传递或成员变量存储
  extern GLFWwindow *window; // 引用 main.cpp 中的全局 window (临时方案)
  if (window) {
    glfwGetFramebufferSize(window, &width, &height);
  } else {
    width = 1280; // 默认值
    height = 720;
  }
  glm::mat4 projection = glm::perspective(
      glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

  m_basicShader->setMat4("model", model);
  m_basicShader->setMat4("view", view);
  m_basicShader->setMat4("projection", projection);

  // 4. 绑定 VAO 并绘制
  glBindVertexArray(m_triangleVAO);
  glDrawArrays(GL_TRIANGLES, 0, 3); // 绘制三角形
  glBindVertexArray(0);             // 解绑 VAO
}

void Renderer::cleanup() {
  std::cout << "Cleaning up Renderer resources..." << std::endl;
  // 删除 VAO 和 VBO
  glDeleteVertexArrays(1, &m_triangleVAO);
  glDeleteBuffers(1, &m_triangleVBO);
  m_triangleVAO = 0;
  m_triangleVBO = 0;

  // Shader 由 unique_ptr 自动管理，无需手动删除
  m_basicShader.reset(); // 显式释放 unique_ptr 管理的资源

  std::cout << "Renderer resources cleaned up." << std::endl;
}

// --- 私有辅助函数 ---
bool Renderer::setupTriangle() {
  // 定义三角形顶点
  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // 左下
      0.5f,  -0.5f, 0.0f, // 右下
      0.0f,  0.5f,  0.0f  // 顶部
  };

  glGenVertexArrays(1, &m_triangleVAO);
  glGenBuffers(1, &m_triangleVBO);

  glBindVertexArray(m_triangleVAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0); // 每次配置完 VAO 后解绑是个好习惯

  return true;
}