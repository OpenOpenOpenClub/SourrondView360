#include "mesh.h"
#include <glad/glad.h>
#include <iostream> // For potential error messages

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<unsigned int> &indices,
           const std::vector<TextureInfo> &textures)
    : vertices(vertices), indices(indices), textures(textures), VAO(0), VBO(0),
      EBO(0) {
  setupMesh();
}

Mesh::~Mesh() {
  // cleanup(); // 析构函数可以调用 cleanup，或者依赖外部调用
}

void Mesh::setupMesh() {
  // 1. 创建缓冲和数组对象
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // 2. 绑定 VAO
  glBindVertexArray(VAO);

  // 3. 绑定 VBO 并加载顶点数据
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0],
               GL_STATIC_DRAW);

  // 4. 绑定 EBO 并加载索引数据
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               &indices[0], GL_STATIC_DRAW);

  // 5. 设置顶点属性指针
  // a. 位置属性 (location = 0)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Position));
  // b. 法线属性 (location = 1) - 如果你的着色器使用法线
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, Normal));
  // c. 纹理坐标属性 (location = 2)
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, TexCoords));

  // 6. 解绑 VAO (重要！)
  glBindVertexArray(0);
  // 解绑 VBO 和 EBO 是可选的，因为 VAO 已经记录了 EBO 的绑定
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // 不要解绑 EBO 当 VAO 仍绑定时
}

void Mesh::draw() {
  // 假设 Renderer 已经绑定了正确的着色器和激活/绑定了纹理单元

  // 绑定此网格的 VAO
  glBindVertexArray(VAO);
  // 执行绘制调用
  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()),
                 GL_UNSIGNED_INT, 0);
  // 解绑 VAO
  glBindVertexArray(0);

  // 恢复 OpenGL 状态 (例如，取消激活纹理单元) 通常由 Renderer 完成
  // glActiveTexture(GL_TEXTURE0);
}

void Mesh::cleanup() {
  std::cout << "Cleaning up Mesh resources..." << std::endl;
  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
    VBO = 0;
  }
  if (EBO != 0) {
    glDeleteBuffers(1, &EBO);
    EBO = 0;
  }
  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
    VAO = 0;
  }
  std::cout << "Mesh cleanup complete." << std::endl;
}