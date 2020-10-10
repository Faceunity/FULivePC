#include <fstream>
#include <iostream>
#include <vector>

#include "authpack.h"
#include "fuai_interface.h"
#include "opencv2/opencv.hpp"

bool ReadBinary(const std::string &path, std::vector<char> &content);
void DrawLandmarks(cv::Mat &img, const float *pts, int num_pts,
                   int opencv_draw_thickness);
void BGRToNV21(const cv::Mat &bgr, cv::Mat &nv21);

#define FI_SAFE_CALL(func)                                                     \
  do {                                                                         \
    auto ret = func;                                                           \
    if (ret != FI_STATUS_OK) {                                                 \
      printf("FI call with error code %d\n", ret);                             \
      return -1;                                                               \
    }                                                                          \
  } while (0)

int main() {
  // setup the library
  FI_SAFE_CALL(FI_Setup(g_auth_package, sizeof(g_auth_package)));

  // [optional] set log level, default=0, range [0,4]; the larger, the more log.
  FI_VLogSetLevel(0);

  // load the ai data
  std::vector<char> buffer;
  ReadBinary("../assets/ai_face_processor_lite.bundle", buffer);

  // create face ai model
  FI_FaceProcessor *face_model = nullptr;
  FI_SAFE_CALL(
      FI_NewFaceProcessorFromBundle(buffer.data(), buffer.size(), &face_model));

  // [optional] try setting face dense landmarks, default 0
  FI_FaceProcessorSetUseDenseLandmark(face_model, 1);

  // load a test image
  cv::Mat test_img = cv::imread("../testdata/Obama.jpg");
  cv::imshow("test_img", test_img);
  cv::waitKey(0);

  // test a supported format
  const auto test_format = FI_IMAGE_RGBA;

  const int width = test_img.cols, height = test_img.rows;
  const unsigned char *data0 = nullptr, *data1 = nullptr, *data2 = nullptr;
  int stride0 = 0, stride1 = 0, stride2 = 0;
  cv::Mat image_formated;
  switch (test_format) {
  default:
    break;
  case FI_IMAGE_RGBA:
    cv::cvtColor(test_img, image_formated, cv::COLOR_BGR2RGBA);
    data0 = image_formated.data;
    stride0 = image_formated.step[0];
    break;
  case FI_IMAGE_YUV_NV21:
    BGRToNV21(test_img, image_formated);
    data0 = image_formated.data;
    stride0 = image_formated.step[0];
    data1 = data0 + stride0 * height;
    stride1 = stride0;
    break;
  case FI_IMAGE_YUV_I420:
    cv::cvtColor(test_img, image_formated, cv::COLOR_BGR2YUV_I420);
    data0 = image_formated.data;
    stride0 = image_formated.step[0];
    data1 = data0 + stride0 * height;
    stride1 = (stride0 + 1) / 2;
    data2 = data1 + (stride0 * height + 3) / 4;
    stride2 = stride1;
    break;
  }
  printf("w=%d,h=%d,s0=%d,s1=%d,s2=%d\n", width, height, stride0, stride1,
         stride2);

  // set a rotation
  auto rotation_mode = FI_IMAGE_ROTATION_0;

  // create image_view
  FI_ImageView *image_view = nullptr;
  FI_NewImageView(&image_view);
  FI_ImageViewInit(image_view, test_format, width, height, rotation_mode, data0,
                   stride0, data1, stride1, data2, stride2);

  // [optional] set the min face, default=0.2; 
  //            the smaller face needed, the more computation cost
  FI_FaceProcessorSetMinFaceRatio(face_model, 0.1f);

  // process
  FI_FaceProcessorProcess(face_model, image_view);

  // release image view
  FI_DeleteImageView(image_view);

  //
  int num_faces = 0;
  FI_FaceProcessorGetNumResults(face_model, &num_faces);

  // get result landmarks and visualize
  cv::Mat resultsImg = test_img.clone();
  for (int face_i = 0; face_i < num_faces; face_i++) {
    int size = 0;
    const float *landmarks = nullptr;
    FI_FaceProcessorGetResultAllLandmarks(face_model, face_i, &landmarks, &size);
    DrawLandmarks(resultsImg, landmarks, size / 2, 3);
  }
  cv::imshow("result points", resultsImg);
  cv::waitKey(0);

  // release face model
  FI_DeleteFaceProcessor(face_model);

  // destroy the lib
  FI_SAFE_CALL(FI_Destroy());

  return 0;
}

bool ReadBinary(const std::string &path, std::vector<char> &content) {
  std::ifstream fin(path, std::ios::binary);
  if (!fin.is_open()) {
    std::cout << "Open file faild! filename: " << path;
    return false;
  }
  fin.seekg(0, std::ios::end);
  size_t size = fin.tellg();
  content.resize(size);
  fin.seekg(0, std::ios::beg);
  fin.read(content.data(), size);
  fin.close();
  return true;
}

void DrawLandmarks(cv::Mat &img, const float *pts, int num_pts,
                   int opencv_draw_thickness) {
  for (int pt_i = 0; pt_i < num_pts; pt_i++) {
    float pt_x = pts[pt_i * 2];
    float pt_y = pts[pt_i * 2 + 1];
    cv::circle(img, cv::Point(pt_x, pt_y), opencv_draw_thickness,
               cv::Scalar(0, 255, 0));
  }
}

void BGRToNV21(const cv::Mat &bgr, cv::Mat &nv21) {
  cv::Mat i420;
  cv::cvtColor(bgr, i420, cv::COLOR_BGR2YUV_I420);
  nv21.create(i420.rows, i420.cols, CV_8UC1);

  const int w = bgr.cols, h = bgr.rows;
  const int stride0 = i420.step[0];

  const uchar *i420_data0 = i420.data;
  const uchar *i420_data1 = i420_data0 + stride0 * h;
  const int i420_stride1 = (stride0 + 1) / 2;
  const uchar *i420_data2 = i420_data1 + (stride0 * h + 3) / 4;
  const int i420_stride2 = (stride0 + 1) / 2;

  uchar *nv21_data0 = nv21.data;
  uchar *nv21_data1 = nv21_data0 + stride0 * h;
  const int nv21_stride1 = stride0;

  // copy Y channel
  memcpy(nv21_data0, i420_data0, stride0 * h);

  // U,V
  for (int y = 0; y < (h + 1) / 2; y++) {
    uchar *nv21_row_uv = nv21_data1 + y * nv21_stride1;
    const uchar *i420_row_u = i420_data1 + y * i420_stride1;
    const uchar *i420_row_v = i420_data2 + y * i420_stride2;
    for (int x = 0; x < (w + 1) / 2; x++) {
      nv21_row_uv[x * 2 + 1] = i420_row_u[x];
      nv21_row_uv[x * 2 + 0] = i420_row_v[x];
    } // x
  }   // y
}
