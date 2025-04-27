#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // For GLuint
#include <glm/glm.hpp>
#include <string>
#include <vector>

// 顶点数据结构
struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;    // 法线 (暂时可能不用，但保留结构)
  glm::vec2 TexCoords; // 纹理坐标
};

// 纹理信息结构 (可选，但有助于管理)
struct TextureInfo {
  GLuint id;        // OpenGL 纹理 ID
  std::string type; // 纹理类型 (e.g., "texture_diffuse")
  std::string path; // 纹理文件路径 (用于调试或记录)
};

class Shader; // 前向声明

class Mesh {
public:
  // 网格数据
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<TextureInfo> textures; // 可以支持多个纹理
  GLuint VAO;

  // 构造函数
  Mesh(const std::vector<Vertex> &vertices,
       const std::vector<unsigned int> &indices,
       const std::vector<TextureInfo> &textures);
  ~Mesh(); // 添加析构函数声明

  // 渲染网格 (由 Renderer 调用)
  // 注意：Shader 设置和纹理激活由 Renderer 处理
  void draw();

  // 清理资源
  void cleanup();

private:
  // 渲染对象
  GLuint VBO, EBO;

  // 初始化和设置缓冲
  void setupMesh();
};

#endif // MESH_H