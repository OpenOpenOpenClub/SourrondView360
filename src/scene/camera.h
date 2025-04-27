#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  // --- 相机属性 ---
  glm::vec3 Position; // 相机位置
  glm::vec3 Front;    // 相机朝向 (方向向量)
  glm::vec3 Up;       // 相机的上向量
  glm::vec3 Right;    // 相机的右向量
  glm::vec3 WorldUp;  // 世界坐标系的上方向 (用于计算 Right 和 Up)

  // --- 欧拉角 (用于方便旋转控制) ---
  float Yaw;   // 偏航角
  float Pitch; // 俯仰角

  // --- 相机选项 ---
  float MovementSpeed;    // 移动速度
  float MouseSensitivity; // 鼠标灵敏度
  float Zoom;             // 视场角 (FOV)

  // --- 构造函数 ---
  // 使用向量初始化
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         float yaw = -90.0f, // 默认指向 -Z 方向
         float pitch = 0.0f);
  // 使用标量值初始化
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch);

  // --- 获取矩阵 ---
  // 返回使用欧拉角和 LookAt 矩阵计算的视图矩阵
  glm::mat4 getViewMatrix();
  // 返回投影矩阵 (需要窗口宽高比)
  glm::mat4 getProjectionMatrix(float aspectRatio, float nearPlane = 0.1f,
                                float farPlane = 100.0f);

  // --- 处理输入 (后续由 CameraController 调用) ---
  // 处理键盘输入以移动相机
  // void processKeyboard(Camera_Movement direction, float deltaTime);
  // 处理鼠标移动以旋转相机
  // void processMouseMovement(float xoffset, float yoffset, bool constrainPitch
  // = true); 处理鼠标滚轮以缩放 (调整 FOV) void processMouseScroll(float
  // yoffset);

  // 根据相机的(更新后)欧拉角计算 Front, Right 和 Up 向量
  void updateCameraVectors();
};

#endif // CAMERA_H