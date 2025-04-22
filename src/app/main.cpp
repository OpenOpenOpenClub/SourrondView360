#include "common.h" // 移动到了 srcs/common/
#include <iostream>
#include <memory>

// --- OpenGL / Windowing ---
// 使用 GLAD (确保已生成并包含头文件)
#include <glad/glad.h> // 放在 GLFW 之前
///

#include <GLFW/glfw3.h>

// --- 数学库 ---
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// --- 引入 Renderer ---
#include "renderer.h" // <--- 包含 Renderer 头文件

// --- 渲染相关 ---
#include "shader.h" // <--- 包含 Shader 类

// --- 全局变量 (暂时) ---
GLFWwindow *window = nullptr;
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
Shader *basicShader = nullptr; // <--- 指向 Shader 对象
unsigned int VBO, VAO;         // <--- 顶点缓冲和顶点数组对象

// --- 函数声明 ---
bool initializeOpenGL();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void cleanupGLFW();
bool setupScene(); // <--- 新增：设置顶点数据和缓冲

// --- Renderer 实例 ---
std::unique_ptr<Renderer> g_renderer; // <--- 使用智能指针管理 Renderer

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

  // --- 加载相机参数等 (暂时保留，后续移至 DataManager) ---
  CameraPrms prms[4];
  for (int i = 0; i < 4; ++i) {
    auto &prm = prms[i];
    prm.name = camera_names[i];
    std::string param_file_path = data_path + "/yaml/" + prm.name + ".yaml";
    std::cout << "Attempting to read parameters from: " << param_file_path
              << std::endl;
    auto ok = read_prms(param_file_path, prm);
    if (!ok) {
      std::cerr << "Failed to read camera parameters for " << prm.name
                << " from " << param_file_path << std::endl;
      cleanupGLFW(); // 清理 GLFW
      return -1;
    }
    std::cout << "Loaded params for: " << prm.name << std::endl;
  }
  // --- 其他数据加载 (车、权重等) 暂时省略 ---

  // 2. 创建和初始化 Renderer
  g_renderer = std::make_unique<Renderer>(data_path); // <--- 创建 Renderer 实例
  if (!g_renderer->init()) {                          // <--- 调用初始化
    std::cerr << "Failed to initialize renderer!" << std::endl;
    cleanupGLFW();
    return -1;
  }
  std::string shaderBasePath = "../data/shaders/"; // 根据你的运行目录调整
  basicShader = new Shader((shaderBasePath + "basic.vert").c_str(),
                           (shaderBasePath + "basic.frag").c_str());
  if (basicShader->ID == 0) { // 检查着色器是否加载成功
    std::cerr << "Failed to load shaders." << std::endl;
    cleanupGLFW();
    return -1;
  }

  // 3. 设置顶点数据和缓冲区
  if (!setupScene()) {
    std::cerr << "Failed to set up scene." << std::endl;
    cleanupGLFW();
    return -1;
  }

  // 4. 渲染循环
  while (!glfwWindowShouldClose(window)) {
    // a. 处理输入
    processInput(window);

    // b. 渲染指令 (委托给 Renderer)
    if (g_renderer) {
      g_renderer->draw(); // <--- 调用 Renderer 的 draw 方法
    }

    // c. 交换缓冲区和检查事件
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // 5. 清理资源
  if (g_renderer) {
    g_renderer->cleanup(); // <--- 调用 Renderer 的清理方法
  }
  cleanupGLFW(); // <--- 清理 GLFW

  std::cout << argv[0] << " app finished" << std::endl;
  return 0;
}

bool initializeOpenGL() {
  // 初始化 GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用核心模式
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // macOS 需要
#endif

  // 创建窗口
  window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Surround View", NULL, NULL);
  // ... 错误检查 ...
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // 初始化 GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return false;
  }

  // 配置 OpenGL 全局状态
  glEnable(GL_DEPTH_TEST); // 启用深度测试

  std::cout << "OpenGL Initialized: Version " << glGetString(GL_VERSION)
            << std::endl;

  return true;
}

// 设置顶点数据和缓冲
bool setupScene() {
  // 定义三角形顶点
  float vertices[] = {
      -0.5f, -0.5f, 0.0f, // 左下
      0.5f,  -0.5f, 0.0f, // 右下
      0.0f,  0.5f,  0.0f  // 顶部
  };

  glGenVertexArrays(1, &VAO); // 创建 VAO
  glGenBuffers(1, &VBO);      // 创建 VBO

  glBindVertexArray(VAO); // 绑定 VAO

  glBindBuffer(GL_ARRAY_BUFFER, VBO); // 绑定 VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
               GL_STATIC_DRAW); // 将顶点数据复制到 VBO

  // 设置顶点属性指针
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0); // 启用顶点属性 0 (对应 layout (location = 0))

  // 解绑 VBO 和 VAO
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return true; // 表示设置成功
}

// 窗口大小调整时的回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height); // 设置视口大小
  // 如果 Renderer 需要知道窗口大小变化（例如更新投影矩阵），可以在这里通知它
  // if (g_renderer) {
  //     g_renderer->onWindowResize(width, height);
  // }
}

// 处理输入: 在每一帧检查按键
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true); // 按 ESC 关闭窗口

  // --- 在这里添加鼠标/键盘控制相机的逻辑 (后续移至 CameraController) ---
}

// 清理 GLFW 资源
void cleanupGLFW() {
  if (window != nullptr) {
    glfwDestroyWindow(window);
    window = nullptr; // 置空指针
  }
  glfwTerminate();
  std::cout << "GLFW resources cleaned up." << std::endl;
}