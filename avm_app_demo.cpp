/***
 * function: 360 surrond view combine c++ demo
 * author: joker.mao
 * date: 2023/07/15
 * copyright: ADAS_EYES all right reserved
 */

#include "common.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// 视角控制参数
struct ViewpointParams {
  float height; // 视角高度 (0.0-1.0)
  float angle;  // 围绕车辆旋转角度 (0-360度)
  float tilt;   // 视角倾斜度 (-30到30度)
  float zoom;   // 缩放系数 (0.5-2.0)
};

// #define DEBUG
#define AWB_LUN_BANLANCE_ENALE 1

// 函数声明：处理一帧图像
cv::Mat processFrame(const std::string &data_path, cv::Mat &car_img,
                     std::vector<cv::Mat> &merge_weights_img,
                     CameraPrms prms[4], const ViewpointParams &viewParams);

// 在图像上显示处理时间和FPS
void displayStats(cv::Mat &img, double process_time, double fps,
                  const ViewpointParams &viewParams);

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "usage:\n\t" << argv[0] << " path\n";
    return -1;
  }
  std::cout << argv[0] << " app start running..." << std::endl;
  std::string data_path = std::string(argv[1]);
  cv::Mat car_img;
  cv::Mat merge_weights_img[4];
  float *w_ptr[4];
  CameraPrms prms[4];

  // 1. 读取车辆图像
  car_img = cv::imread(data_path + "/images/car.png");
  cv::resize(car_img, car_img, cv::Size(xr - xl, yb - yt));

  // 2. 读取权重图
  cv::Mat weights = cv::imread(data_path + "/yaml/weights.png", -1);

  if (weights.channels() != 4) {
    std::cerr << "imread weights failed " << weights.channels() << "\r\n";
    return -1;
  }

  // 3. 处理权重图
  std::vector<cv::Mat> weights_vector;
  for (int i = 0; i < 4; ++i) {
    merge_weights_img[i] =
        cv::Mat(weights.size(), CV_32FC1, cv::Scalar(0, 0, 0));
    w_ptr[i] = (float *)merge_weights_img[i].data;
    weights_vector.push_back(merge_weights_img[i]);
  }

  int pixel_index = 0;
  for (int h = 0; h < weights.rows; ++h) {
    uchar *uc_pixel = weights.data + h * weights.step;
    for (int w = 0; w < weights.cols; ++w) {
      w_ptr[0][pixel_index] = uc_pixel[0] / 255.0f;
      w_ptr[1][pixel_index] = uc_pixel[1] / 255.0f;
      w_ptr[2][pixel_index] = uc_pixel[2] / 255.0f;
      w_ptr[3][pixel_index] = uc_pixel[3] / 255.0f;
      uc_pixel += 4;
      ++pixel_index;
    }
  }

  // 4. 读取相机参数
  for (int i = 0; i < 4; ++i) {
    auto &prm = prms[i];
    prm.name = camera_names[i];
    auto ok = read_prms(data_path + "/yaml/" + prm.name + ".yaml", prm);
    if (!ok) {
      return -1;
    }
  }

  // 创建窗口
  cv::namedWindow("ADAS_EYES_360_VIEW", cv::WINDOW_NORMAL);
  cv::resizeWindow("ADAS_EYES_360_VIEW", 800, 600);

  // 计时相关变量
  double process_time = 0;
  double fps = 0;
  int frame_count = 0;
  double total_time = 0;
  char key = 0;

  // 视角参数初始化
  ViewpointParams viewParams = {1.0f, 0.0f, 0.0f, 1.0f}; // 默认为顶视图

  // 主循环
  while (key != 'q' && key != 27) { // 'q'或Esc键退出

    //
    switch (key) {
    case 'w': // 增加高度（更俯视）
      viewParams.height = std::min(viewParams.height + 0.05f, 1.0f);
      break;
    case 's': // 降低高度（更平视）
      viewParams.height = std::max(viewParams.height - 0.05f, 0.0f);
      break;
    case 'a': // 逆时针旋转视角
      viewParams.angle = fmod(viewParams.angle - 10.0f + 360.0f, 360.0f);
      break;
    case 'd': // 顺时针旋转视角
      viewParams.angle = fmod(viewParams.angle + 10.0f, 360.0f);
      break;
    case 'z': // 缩小
      viewParams.zoom = std::max(viewParams.zoom - 0.1f, 0.5f);
      break;
    case 'x': // 放大
      viewParams.zoom = std::min(viewParams.zoom + 0.1f, 2.0f);
      break;
    }

    int64 start = cv::getTickCount();

    // 处理帧
    cv::Mat result =
        processFrame(data_path, car_img, weights_vector, prms, viewParams);

    // 计算处理时间
    int64 end = cv::getTickCount();
    process_time = (end - start) * 1000 / cv::getTickFrequency(); // 毫秒

    // 计算FPS
    frame_count++;
    total_time += process_time;
    if (total_time >= 1000) { // 每秒更新一次FPS
      fps = frame_count * 1000.0 / total_time;
      frame_count = 0;
      total_time = 0;
    }

    // 在图像上显示处理时间和FPS
    displayStats(result, process_time, fps, viewParams);

    // 显示图像
    cv::imshow("ADAS_EYES_360_VIEW", result);

    // 等待按键
    key = cv::waitKey(1);
  }

  cv::destroyAllWindows();
  std::cout << argv[0] << " app finished" << std::endl;
  return 0;
}

// 在图像上显示处理时间和FPS
void displayStats(cv::Mat &img, double process_time, double fps,
                  const ViewpointParams &viewParams) {
  std::stringstream ss;
  ss << "Processing time: " << std::fixed << std::setprecision(1)
     << process_time << " ms";
  cv::putText(img, ss.str(), cv::Point(20, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7,
              cv::Scalar(0, 0, 255), 2);

  ss.str("");
  ss << "FPS: " << std::fixed << std::setprecision(1) << fps;
  cv::putText(img, ss.str(), cv::Point(20, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7,
              cv::Scalar(0, 0, 255), 2);

  // 添加视角信息显示
  ss.str("");
  ss << "View: H=" << std::fixed << std::setprecision(2) << viewParams.height
     << " A=" << std::setprecision(0) << viewParams.angle
     << " Z=" << std::setprecision(1) << viewParams.zoom;
  cv::putText(img, ss.str(), cv::Point(20, 90), cv::FONT_HERSHEY_SIMPLEX, 0.7,
              cv::Scalar(0, 0, 255), 2);
}

cv::Mat calculateViewMatrix(const cv::Mat &baseMatrix,
                            const ViewpointParams &viewParams) {
  // 确保使用float类型
  cv::Mat viewMatrix;
  baseMatrix.convertTo(viewMatrix, CV_32F);

  // 根据角度调整
  float angleRad = viewParams.angle * CV_PI / 180.0;
  cv::Mat rotMat = cv::Mat::eye(3, 3, CV_32F);
  rotMat.at<float>(0, 0) = cos(angleRad);
  rotMat.at<float>(0, 1) = -sin(angleRad);
  rotMat.at<float>(1, 0) = sin(angleRad);
  rotMat.at<float>(1, 1) = cos(angleRad);

  // 应用变换
  viewMatrix = rotMat * viewMatrix;

  // 应用缩放
  viewMatrix *= viewParams.zoom;

  return viewMatrix;
}

// 处理一帧图像的函数
cv::Mat processFrame(const std::string &data_path, cv::Mat &car_img,
                     std::vector<cv::Mat> &merge_weights_img,
                     CameraPrms prms[4], const ViewpointParams &viewParams) {
  cv::Mat origin_dir_img[4];
  cv::Mat undist_dir_img[4];
  cv::Mat out_put_img =
      cv::Mat(cv::Size(total_w, total_h), CV_8UC3, cv::Scalar(0, 0, 0));

  // 1.读取图片并进行亮度均衡和自动白平衡
  std::vector<cv::Mat *> srcs;
  for (int i = 0; i < 4; ++i) {
    auto &prm = prms[i];
    origin_dir_img[i] = cv::imread(data_path + "/images/" + prm.name + ".png");
    srcs.push_back(&origin_dir_img[i]);
  }

#if AWB_LUN_BANLANCE_ENALE
  awb_and_lum_banlance(srcs);
#endif

  // 2.去畸变处理
  for (int i = 0; i < 4; ++i) {
    auto &prm = prms[i];
    cv::Mat &src = origin_dir_img[i];

    undist_by_remap(src, src, prm);

    // 计算动态视角投影矩阵
    cv::Mat dynamicMatrix = calculateViewMatrix(prm.project_matrix, viewParams);

    // 应用变换后的投影矩阵
    cv::warpPerspective(src, src, dynamicMatrix, project_shapes[prm.name]);

    if (camera_flip_mir[i] == "r+") {
      cv::rotate(src, src, cv::ROTATE_90_CLOCKWISE);
    } else if (camera_flip_mir[i] == "r-") {
      cv::rotate(src, src, cv::ROTATE_90_COUNTERCLOCKWISE);
    } else if (camera_flip_mir[i] == "m") {
      cv::rotate(src, src, cv::ROTATE_180);
    }
    undist_dir_img[i] = src.clone();
  }

  // 3.开始合成
  // 3.1 放置车辆图像
  car_img.copyTo(out_put_img(cv::Rect(xl, yt, car_img.cols, car_img.rows)));

  // 3.2 复制四个方向的图像
  for (int i = 0; i < 4; ++i) {
    cv::Rect roi;
    if (std::string(camera_names[i]) == "front") {
      roi = cv::Rect(xl, 0, xr - xl, yt);
      undist_dir_img[i](roi).copyTo(out_put_img(roi));
    } else if (std::string(camera_names[i]) == "left") {
      roi = cv::Rect(0, yt, xl, yb - yt);
      undist_dir_img[i](roi).copyTo(out_put_img(roi));
    } else if (std::string(camera_names[i]) == "right") {
      roi = cv::Rect(0, yt, xl, yb - yt);
      undist_dir_img[i](roi).copyTo(
          out_put_img(cv::Rect(xr, yt, total_w - xr, yb - yt)));
    } else if (std::string(camera_names[i]) == "back") {
      roi = cv::Rect(xl, 0, xr - xl, yt);
      undist_dir_img[i](roi).copyTo(out_put_img(cv::Rect(xl, yb, xr - xl, yt)));
    }
  }

  // 3.3 合成四个角的图像
  cv::Rect roi;
  // 左上
  roi = cv::Rect(0, 0, xl, yt);
  merge_image(undist_dir_img[0](roi), undist_dir_img[1](roi),
              merge_weights_img[2], out_put_img(roi));
  // 右上
  roi = cv::Rect(xr, 0, xl, yt);
  merge_image(undist_dir_img[0](roi), undist_dir_img[3](cv::Rect(0, 0, xl, yt)),
              merge_weights_img[1], out_put_img(cv::Rect(xr, 0, xl, yt)));
  // 左下
  roi = cv::Rect(0, yb, xl, yt);
  merge_image(undist_dir_img[2](cv::Rect(0, 0, xl, yt)), undist_dir_img[1](roi),
              merge_weights_img[0], out_put_img(roi));
  // 右下
  roi = cv::Rect(xr, 0, xl, yt);
  merge_image(undist_dir_img[2](roi),
              undist_dir_img[3](cv::Rect(0, yb, xl, yt)), merge_weights_img[3],
              out_put_img(cv::Rect(xr, yb, xl, yt)));

  return out_put_img;
}