#include "renderer.h"
#include "common.h"
#include "mesh.h"
#include "model_loader.h"
#include "shader.h"
#include "texture_utils.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

Renderer::Renderer(const std::string &dataPath)
    : m_dataPath(dataPath), m_sampleTextureId(0) {} // 初始化 texture ID

Renderer::~Renderer() {
  cleanup(); // 确保 cleanup 被调用
}
bool Renderer::init() {
  // 1. 加载着色器 (保持不变)
  std::string shaderBasePath = m_dataPath + "/shaders/";
  m_basicShader =
      std::make_unique<Shader>((shaderBasePath + "basic.vert").c_str(),
                               (shaderBasePath + "basic.frag").c_str());
  if (!m_basicShader || m_basicShader->ID == 0) {
    std::cerr << "Renderer Error: Failed to load basic shader." << std::endl;
    return false;
  }
  std::cout << "Renderer: Basic shader loaded successfully." << std::endl;

  // 2. 加载纹理 (保持不变)
  std::string texturePath = m_dataPath + "/textures/sample_front.png";
  m_sampleTextureId = loadTexture(texturePath.c_str());
  if (m_sampleTextureId == 0) {
    std::cerr << "Renderer Warning: Failed to load sample texture. Continuing "
                 "without texture."
              << std::endl;
    // 不再因为纹理失败而返回 false
  } else {
    std::cout << "Renderer: Sample texture loaded ID: " << m_sampleTextureId
              << std::endl;
  }

  // 3. 加载模型数据
  std::vector<Vertex> loadedVertices;
  std::vector<unsigned int> loadedIndices;
  std::string modelPath =
      m_dataPath + "/models/car.obj"; // <--- 修改为你的模型路径
  if (!loadModel(modelPath, loadedVertices, loadedIndices)) {
    std::cerr << "Renderer Error: Failed to load model: " << modelPath
              << std::endl;
    // 如果模型加载失败，可以选择创建一个默认的平面或返回错误
    // 这里我们创建一个简单的平面作为后备
    std::cerr << "Renderer Info: Creating fallback plane." << std::endl;
    loadedVertices = {{{1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                      {{-1.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
                      {{-1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
                      {{1.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}};
    loadedIndices = {0, 1, 2, 0, 2, 3};
    // return false; // 或者直接退出
  }

  // 4. 准备纹理信息 (即使模型加载失败，也尝试使用加载的纹理)
  std::vector<TextureInfo> textures;
  if (m_sampleTextureId != 0) {
    textures.push_back({m_sampleTextureId, "texture_diffuse", texturePath});
  }

  // 5. 创建 Mesh 对象 (使用加载的数据或后备数据)
  m_surroundMesh =
      std::make_unique<Mesh>(loadedVertices, loadedIndices, textures);
  if (!m_surroundMesh || m_surroundMesh->VAO == 0) {
    std::cerr << "Renderer Error: Failed to create surround mesh." << std::endl;
    return false;
  }
  std::cout << "Renderer: Surround mesh created successfully." << std::endl;

  return true;
}
void Renderer::draw(const glm::mat4 &view, const glm::mat4 &projection) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (!m_basicShader || !m_surroundMesh) {
    std::cerr << "Renderer Error: Shader or Mesh not ready for drawing."
              << std::endl;
    return;
  }

  m_basicShader->use();

  // 设置变换矩阵
  glm::mat4 model = glm::mat4(1.0f); // 模型矩阵 (单位矩阵，模型已经在世界坐标)
  m_basicShader->setMat4("model", model);
  m_basicShader->setMat4("view", view);
  m_basicShader->setMat4("projection", projection);

  // 绑定纹理
  if (m_sampleTextureId != 0) {
    glActiveTexture(GL_TEXTURE0); // 激活纹理单元 0
    glBindTexture(GL_TEXTURE_2D, m_sampleTextureId);
    // 告诉着色器 texture1 采样器使用纹理单元 0
    m_basicShader->setInt("texture1", 0);
  } else {
    // 可选：如果纹理加载失败，可以设置一个默认颜色或解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0); // 解绑任何可能绑定的纹理
    // 可以设置一个 uniform 变量告诉着色器不要采样纹理
    // m_basicShader->setBool("useTexture", false);
  }

  // 调用 Mesh 的 draw 方法
  m_surroundMesh->draw();

  // 解绑纹理 (可选，良好的实践)
  // glActiveTexture(GL_TEXTURE0); // 确保在正确的单元上操作
  // glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::cleanup() {
  std::cout << "Renderer: Cleaning up resources..." << std::endl;
  // 清理 Mesh (unique_ptr 会自动调用析构函数，析构函数可以调用 Mesh::cleanup)
  if (m_surroundMesh) {
    m_surroundMesh->cleanup(); // 或者让 unique_ptr 管理
    m_surroundMesh.reset();
  }

  // 删除纹理
  if (m_sampleTextureId != 0) {
    glDeleteTextures(1, &m_sampleTextureId);
    m_sampleTextureId = 0;
    std::cout << "Renderer: Sample texture deleted." << std::endl;
  }

  // 清理 Shader (由 unique_ptr 自动管理)
  m_basicShader.reset();

  // 移除旧的 VBO/VAO 清理
  // glDeleteVertexArrays(1, &m_triangleVAO);
  // glDeleteBuffers(1, &m_triangleVBO);
  std::cout << "Renderer: Cleanup complete." << std::endl;
}

// 移除 setupTriangle 函数实现
// bool Renderer::setupTriangle() { ... }