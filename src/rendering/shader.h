#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // 包含glad来获取所有的必须OpenGL头文件
#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  // 程序ID
  unsigned int ID;

  // 构造器读取并构建着色器
  Shader(const char *vertexPath, const char *fragmentPath);
  // 析构
  ~Shader();

  // 激活程序
  void use();
  // uniform工具函数
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, const glm::mat4 &mat) const;
  void setVec3(const std::string &name, const glm::vec3 &value) const;
  void setVec3(const std::string &name, float x, float y, float z) const;

private:
  // 检查编译/链接错误的工具函数
  void checkCompileErrors(unsigned int shader, std::string type);
};

#endif