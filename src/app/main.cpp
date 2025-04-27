#include "common.h"
#include <iostream>
#include <memory>
#include <string> // For std::string

#include <glad/glad.h>
// --- OpenGL / Windowing ---
#include <GLFW/glfw3.h>

// --- 数学库 ---
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- 引入 Renderer ---
#include "renderer.h"

// --- 引入 Scene Components --- // <--- NEW SECTION
#include "camera.h"
#include "cameracontroller.h"

// --- 渲染相关 ---
#include "shader.h" // Keep for now, though Renderer manages its own

// --- 全局变量 ---
GLFWwindow *window = nullptr;
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
// Shader* basicShader = nullptr; // Now managed by Renderer
// unsigned int VBO, VAO; // Now managed by Renderer

// --- Scene Objects --- // <--- NEW SECTION
std::unique_ptr<Camera> g_camera;
std::unique_ptr<CameraController> g_cameraController;
bool g_mouseControlActive = false; // Track if mouse control should be active

// --- 函数声明 ---
bool initializeOpenGL();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void cleanupGLFW();
// bool setupScene(); // Now handled within Renderer::init or similar
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void setCameraView(int viewIndex);

// --- Renderer 实例 ---
std::unique_ptr<Renderer> g_renderer;

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "usage:\n\t" << argv[0] << " data_path\n";
    return -1;
  }
  std::string data_path = std::string(argv[1]);
  std::cout << argv[0] << " app start running..." << std::endl;

  // 1. 初始化 GLFW 和 OpenGL
  if (!initializeOpenGL()) {
    return -1;
  }

  // --- 加载相机参数等 (暂时保留) ---
  // ... (your existing parameter loading code) ...

  // 2. 创建和初始化 Renderer
  g_renderer = std::make_unique<Renderer>(data_path);
  if (!g_renderer->init()) { // Renderer::init should load shaders, setup basic
                             // geometry (triangle VAO/VBO)
    std::cerr << "Failed to initialize renderer!" << std::endl;
    cleanupGLFW();
    return -1;
  }

  // 3. 创建相机和控制器 // <--- NEW
  g_camera = std::make_unique<Camera>(
      glm::vec3(0.0f, 2.0f, 9.0f)); // Initial camera position
  g_cameraController = std::make_unique<CameraController>(*g_camera);
  g_cameraController->disableMouseControl(window);
  g_mouseControlActive = false;

  // 4. 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // a. 处理输入 (Keyboard handled here, mouse handled by callbacks)
    processInput(window);

    // b. 渲染指令
    if (g_renderer && g_camera) {
      // Calculate aspect ratio
      int currentWidth, currentHeight;
      glfwGetFramebufferSize(window, &currentWidth, &currentHeight);
      float aspectRatio = (currentHeight == 0)
                              ? 1.0f
                              : (float)currentWidth / (float)currentHeight;

      // Get matrices from camera
      glm::mat4 view = g_camera->getViewMatrix();
      glm::mat4 projection = g_camera->getProjectionMatrix(aspectRatio);

      // Call Renderer's draw method with matrices
      g_renderer->draw(view, projection); // <--- Pass matrices
    }

    // c. 交换缓冲区和检查事件
    glfwSwapBuffers(window);
    glfwPollEvents(); // Processes events, including mouse callbacks
  }

  // 5. 清理资源
  if (g_renderer) {
    g_renderer->cleanup();
  }
  cleanupGLFW();

  std::cout << argv[0] << " app finished" << std::endl;
  return 0;
}

bool initializeOpenGL() {
  // 初始化 GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // 创建窗口
  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                            "3D Surround View - Mouse Control", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window);

  // 设置回调函数
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(
      window, cursor_position_callback); // <--- Set mouse move callback
  glfwSetScrollCallback(window,
                        scroll_callback); // <--- Set mouse scroll callback
  glfwSetMouseButtonCallback(
      window, mouse_button_callback); // <--- Set mouse button callback

  // 初始化 GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return false;
  }

  // Configure global OpenGL state (optional, enable depth testing)
  glEnable(GL_DEPTH_TEST);

  return true;
}

// 窗口大小调整时的回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// 处理键盘输入
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // Add key (e.g., TAB) to toggle mouse control
  // (This requires tracking key state to avoid rapid toggling)
  // Example: Press M to toggle mouse control
  // static bool mKeyPressed = false;
  // if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !mKeyPressed) {
  //     g_mouseControlActive = !g_mouseControlActive;
  //     if (g_mouseControlActive && g_cameraController) {
  //         g_cameraController->enableMouseControl(window);
  //     } else if (g_cameraController) {
  //         g_cameraController->disableMouseControl(window);
  //     }
  //     mKeyPressed = true;
  // }
  // if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
  //     mKeyPressed = false;
  // }

  // --- Add keyboard camera movement later if needed ---
  // float cameraSpeed = g_camera->MovementSpeed * deltaTime; // Need deltaTime
  // calculation if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  //     g_camera->Position += cameraSpeed * g_camera->Front;
  // ... (A, S, D keys) ...
}

// 清理 GLFW 资源
void cleanupGLFW() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

// --- Mouse Callback Implementations --- // <--- NEW

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
  if (g_cameraController) {
    g_cameraController->processMouseMovement(window, xpos, ypos);
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  if (g_cameraController) {
    g_cameraController->processMouseScroll(window, xoffset, yoffset);
  }
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  // Example: Enable mouse control on left-click, disable on right-click
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    if (g_cameraController) {
      g_cameraController->enableMouseControl(window);
      g_mouseControlActive = true;
    }
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    if (g_cameraController) {
      g_cameraController->disableMouseControl(window);
      g_mouseControlActive = false;
    }
  }
}

void setCameraView(int viewIndex) {
  if (!g_camera)
    return;

  const float distance =
      8.0f; // Distance from the car center (adjust as needed)
  const float angle45 = glm::radians(45.0f);
  const float height = distance * glm::sin(angle45);         // Approx 5.65
  const float horizontalDist = distance * glm::cos(angle45); // Approx 5.65
  const float sideOffset =
      distance * glm::cos(angle45) * glm::cos(angle45); // Approx 4.0

  switch (viewIndex) {
  case 1: // 车正上方俯视
    g_camera->Position = glm::vec3(0.0f, distance, 0.0f);
    g_camera->Yaw = -90.0f;
    g_camera->Pitch = -89.9f; // Almost straight down
    break;
  case 2: // 从车头前上方 俯视45度看着自车中心
    g_camera->Position = glm::vec3(0.0f, height, horizontalDist);
    g_camera->Yaw = -90.0f;
    g_camera->Pitch = -45.0f;
    break;
  case 3: // 从车尾后上方 俯视45度看着自车中心
    g_camera->Position = glm::vec3(0.0f, height, -horizontalDist);
    // Looking from -Z towards origin (+Z direction relative to camera's
    // forward)
    g_camera->Yaw = 90.0f; // Adjusted Yaw
    g_camera->Pitch = -45.0f;
    break;
  case 4: // 从左侧后上方 俯视45度看着自车中心
    g_camera->Position = glm::vec3(-sideOffset, height, -sideOffset);
    // Looking from -X, -Z towards origin (+X, +Z direction relative to camera's
    // forward)
    g_camera->Yaw = 45.0f; // Adjusted Yaw
    g_camera->Pitch = -45.0f;
    break;
  case 5: // 从右侧后上方 俯视45度看着自车中心
    g_camera->Position = glm::vec3(sideOffset, height, -sideOffset);
    // Looking from +X, -Z towards origin (-X, +Z direction relative to camera's
    // forward)
    g_camera->Yaw = 135.0f; // Adjusted Yaw
    g_camera->Pitch = -45.0f;
    break;
  default:
    return; // Ignore other keys
  }

  // After setting position and angles, update the camera's internal vectors
  g_camera->updateCameraVectors();

  // Optional: Disable mouse control when switching to a fixed view
  // if (g_cameraController && g_mouseControlActive) {
  //     g_cameraController->disableMouseControl(window); // Need window handle
  //     here g_mouseControlActive = false; std::cout << "Mouse control DISABLED
  //     (fixed view)" << std::endl;
  // }
}