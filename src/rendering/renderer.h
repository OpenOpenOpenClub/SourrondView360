// filepath: /home/wind/Projects/SourrondView360/src/rendering/renderer.h
#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector> // For storing meshes or textures if needed

class Shader;
class Mesh; // 前向声明 Mesh

class Renderer {
public:
  Renderer(const std::string &dataPath);
  ~Renderer();

  bool init();
  void draw(const glm::mat4 &view, const glm::mat4 &projection);
  void cleanup();

private:
  std::string m_dataPath;
  std::unique_ptr<Shader> m_basicShader;
  // std::unique_ptr<Mesh> m_triangleMesh; // 移除旧的三角形 VAO/VBO
  std::unique_ptr<Mesh> m_surroundMesh; // 用于环视的网格
  GLuint m_sampleTextureId;             // 存储加载的纹理 ID

  // 移除 setupTriangle
  // bool setupTriangle();
};

#endif // RENDERER_H