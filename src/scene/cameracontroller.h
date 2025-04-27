#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "camera.h"     // 需要包含 Camera 类定义
#include <GLFW/glfw3.h> // 需要 GLFW 来处理窗口和输入

class CameraController {
public:
  // 构造函数，需要一个 Camera 对象的引用
  CameraController(Camera &camera);

  // 处理鼠标移动的回调函数
  void processMouseMovement(GLFWwindow *window, double xpos, double ypos);

  // 处理鼠标滚轮的回调函数
  void processMouseScroll(GLFWwindow *window, double xoffset, double yoffset);

  // 启用/禁用鼠标控制
  void enableMouseControl(GLFWwindow *window);
  void disableMouseControl(GLFWwindow *window);

  // 设置鼠标灵敏度
  void setMouseSensitivity(float sensitivity);

private:
  Camera &m_camera; // 引用所控制的相机对象

  // 鼠标状态
  bool m_firstMouse;          // 是否是第一次接收鼠标输入
  double m_lastX;             // 上一帧鼠标的 X 坐标
  double m_lastY;             // 上一帧鼠标的 Y 坐标
  bool m_mouseControlEnabled; // 鼠标控制是否启用

  // 内部更新逻辑
  void updateCameraRotation(float xoffset, float yoffset);
  void updateCameraZoom(float yoffset);
  void updateCameraDistance(float yoffset);
};

#endif // CAMERACONTROLLER_H
