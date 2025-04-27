#include "cameracontroller.h"
#include "camera.h"  // Include Camera header
#include <algorithm> // For std::clamp

// --- Constructor ---
CameraController::CameraController(Camera &camera)
    : m_camera(camera), m_firstMouse(true),
      m_lastX(0.0),                // Initialize properly later
      m_lastY(0.0),                // Initialize properly later
      m_mouseControlEnabled(false) // Start with mouse control disabled
{}

// --- Mouse Movement Processing ---
void CameraController::processMouseMovement(GLFWwindow *window, double xpos,
                                            double ypos) {
  if (!m_mouseControlEnabled) {
    m_firstMouse = true; // Reset if control is disabled
    return;
  }

  if (m_firstMouse) {
    m_lastX = xpos;
    m_lastY = ypos;
    m_firstMouse = false;
  }

  double xoffset = xpos - m_lastX;
  double yoffset =
      m_lastY - ypos; // Reversed since y-coordinates go from bottom to top

  m_lastX = xpos;
  m_lastY = ypos;

  updateCameraRotation(static_cast<float>(xoffset),
                       static_cast<float>(yoffset));
}

// --- Mouse Scroll Processing ---
void CameraController::processMouseScroll(GLFWwindow *window, double xoffset,
                                          double yoffset) {
  if (!m_mouseControlEnabled) {
    return;
  }
  // updateCameraZoom(static_cast<float>(yoffset));
  updateCameraDistance(static_cast<float>(yoffset));
}

// --- Enable/Disable Mouse Control ---
void CameraController::enableMouseControl(GLFWwindow *window) {
  if (!m_mouseControlEnabled) {
    glfwSetInputMode(window, GLFW_CURSOR,
                     GLFW_CURSOR_DISABLED); // Capture cursor
    // Get current cursor position to avoid jump AND initialize lastX/Y
    glfwGetCursorPos(window, &m_lastX, &m_lastY); // <--- 这里会设置初始值
    m_firstMouse = true; // Treat next movement as the first
    m_mouseControlEnabled = true;
  }
}

void CameraController::disableMouseControl(GLFWwindow *window) {
  if (m_mouseControlEnabled) {
    glfwSetInputMode(window, GLFW_CURSOR,
                     GLFW_CURSOR_NORMAL); // Release cursor
    m_mouseControlEnabled = false;
    m_firstMouse = true; // Reset first mouse flag
  }
}

// --- Set Sensitivity ---
void CameraController::setMouseSensitivity(float sensitivity) {
  m_camera.MouseSensitivity = sensitivity;
}

// --- Private Update Logic ---
void CameraController::updateCameraRotation(float xoffset, float yoffset) {
  xoffset *= m_camera.MouseSensitivity;
  yoffset *= m_camera.MouseSensitivity;

  m_camera.Yaw += xoffset;
  m_camera.Pitch += yoffset;

  // Constrain pitch to avoid flipping
  m_camera.Pitch = std::clamp(m_camera.Pitch, -89.0f, 89.0f);

  // Update Front, Right and Up Vectors using the updated Euler angles
  m_camera.updateCameraVectors();
}

void CameraController::updateCameraZoom(float yoffset) {
  m_camera.Zoom -= yoffset; // Adjust FOV based on scroll
  // Constrain zoom/FOV
  m_camera.Zoom = std::clamp(m_camera.Zoom, 1.0f, 60.0f);
}

void CameraController::updateCameraDistance(float yoffset) {
  float distanceSensitivity = 5.0f; // 控制移动速度
  glm::vec3 displacement =
      m_camera.Front * yoffset * distanceSensitivity; // 计算位移量
  m_camera.Position += displacement;                  // 修改相机位置
}