#include "camera.h"
#include <vector> // 虽然当前实现未使用，但未来处理输入可能需要

// --- 构造函数实现 ---
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), // 初始朝向 -Z
      MovementSpeed(2.5f),                 // 默认移动速度
      MouseSensitivity(0.1f),              // 默认鼠标灵敏度
      Zoom(45.0f)                          // 默认视场角 (FOV)
{
  Position = position;
  WorldUp = up;
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors(); // 初始化 Front, Right, Up 向量
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f),
      MouseSensitivity(0.1f), Zoom(45.0f) {
  Position = glm::vec3(posX, posY, posZ);
  WorldUp = glm::vec3(upX, upY, upZ);
  Yaw = yaw;
  Pitch = pitch;
  updateCameraVectors();
}

// --- 获取矩阵实现 ---
glm::mat4 Camera::getViewMatrix() {
  // 使用 glm::lookAt 计算视图矩阵
  // 参数：相机位置, 目标点 (位置 + 朝向), 上向量
  return glm::lookAt(Position, Position + Front, Up);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio, float nearPlane,
                                      float farPlane) {
  // 使用 glm::perspective 计算透视投影矩阵
  // 参数：视场角 (弧度), 宽高比, 近裁剪面, 远裁剪面
  return glm::perspective(glm::radians(Zoom), aspectRatio, nearPlane, farPlane);
}

// --- 私有辅助函数实现 ---
void Camera::updateCameraVectors() {
  // 计算新的 Front 向量
  glm::vec3 front;
  front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  front.y = sin(glm::radians(Pitch));
  front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
  Front = glm::normalize(front); // 标准化向量

  // 重新计算 Right 和 Up 向量
  // Right 向量 = Front 向量与世界 Up 向量的叉积 (标准化)
  Right = glm::normalize(glm::cross(Front, WorldUp));
  // Up 向量 = Right 向量与 Front 向量的叉积 (标准化)
  Up = glm::normalize(glm::cross(Right, Front));
}