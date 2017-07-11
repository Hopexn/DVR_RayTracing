#include <iostream>
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/fast_square_root.hpp>
#include <QApplication>
#include <QImage>
#include <QRgb>
#include <QPainter>
#include <QRect>
#include <QLabel>

#include "Volume.h"
#include "VolumeRender.h"

using namespace std;
using namespace cv;
using namespace glm;


/**
 * 为每一个面编号：
 * 底： 1
 * 左： 2
 * 后： 3
 * 右： 4
 * 前： 5
 * 上： 6
 */
float caculate_enter_dist(vec3 cam_pos, vec3 ray_dir) {
    float lamda = 0.0f;
    vec3 dst_pos;
    if (cam_pos.x > 0.5f) {
        lamda = (1.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.y > 0.5f) {
        lamda = (1.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.z > 0.5f) {
        lamda = (1.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
    }
    return 0;
}

float caculate_leave_dist(vec3 cam_pos, vec3 ray_dir) {
    float lamda = 0.0f;
    vec3 dst_pos;
    if (cam_pos.x < 0.5f) {
        lamda = (1.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.x) / ray_dir.x;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.y > 0.0f && dst_pos.y < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.y < 0.5f) {
        lamda = (1.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.y) / ray_dir.y;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.z > 0.0f && dst_pos.z < 1.0f) {
            return lamda;
        }
    }

    if (cam_pos.z < 0.5f) {
        lamda = (1.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
    } else {
        lamda = (0.0f - cam_pos.z) / ray_dir.z;
        dst_pos = cam_pos + lamda * ray_dir;
        if (dst_pos.x > 0.0f && dst_pos.x < 1.0f && dst_pos.y > 0.0f && dst_pos.y < 1.0f) {
            return lamda;
        }
    }
    return 0;
}

float getVolumeValue(Volume &volume, vec3 pos) {
    int xIndex, yIndex, zIndex;
    float xFraction, yFraction, zFraction;
    xIndex = (int) pos.x;
    yIndex = (int) pos.y;
    zIndex = (int) pos.z;
    xFraction = pos.x - xIndex;
    yFraction = pos.y - yIndex;
    zFraction = pos.z - zIndex;
    int zNext = volume.xiSize * volume.yiSize;
    int yNext = volume.xiSize;
    int xNext = 1;
    unsigned char f000, f001, f010, f011, f100, f101, f110, f111;
    int index = zIndex * volume.xiSize * volume.yiSize + yIndex * volume.xiSize + xIndex;
    f000 = volume.data[index];
    f001 = volume.data[index + zNext];
    f010 = volume.data[index + yNext];
    f011 = volume.data[index + yNext + zNext];
    f100 = volume.data[index + xNext];
    f101 = volume.data[index + xNext + zNext];
    f110 = volume.data[index + xNext + zNext];
    f111 = volume.data[index + xNext + yNext + zNext];

    float value = f000 * (1 - xFraction) * (1 - yFraction) * (1 - zFraction) +
                  f001 * (1 - xFraction) * (1 - yFraction) * zFraction +
                  f010 * (1 - xFraction) * yFraction * (1 - zFraction) +
                  f011 * (1 - xFraction) * yFraction * zFraction +
                  f100 * xFraction * (1 - yFraction) * (1 - zFraction) +
                  f101 * xFraction * (1 - yFraction) * zFraction +
                  f110 * xFraction * yFraction * (1 - zFraction) +
                  f111 * xFraction * yFraction * zFraction;
    return value;
}

int main(int argc, char **argv) {
    int img_width = 256, img_height = 256;


    Volume volume;
    string filename = "assets/volume/aneurism.vifo";
    volume.loadRawData(filename.c_str());




//    1. 从前向后
//    C'(i+1) = C(i) + (1 - A(i)) * C'(i)
//     - C'(x) 表示从起点到x的累积光强
//     - C(x) 表示x点发光强度
//     - A(x) 表示x点的不透明度
//
//    2. 从后到前
//    C'(i) = C'(i + 1) + (1 - A'(i+1)) * C(i)
//    A'(i) = A'(i + 1) + (1 - A'(i+1)) * A(i)
//     - C'(x)表示从x到终点所有能到终点的光强之和
//     - C(x)表示x点的发光强度
//     - A'(x)表示从x点到终点的不透明度

    QApplication app(argc, argv);
    QImage image(img_width, img_height, QImage::Format_ARGB32);
    Mat mat(img_width, img_height, 4);

    vec3 cam_pos(0.5f, 0.5f, 10);
    vec3 vol_center(0.5, 0.5, 0.5);
    vec3 cam_dir = fastNormalize(vol_center - cam_pos);
    vec3 cam_up(0, 1, 0);
    vec3 cam_right = fastNormalize(cross(cam_dir, cam_up));

    float screen_dist = 9.0f;

    cout << caculate_enter_dist(cam_pos, cam_dir) << " " << caculate_leave_dist(cam_pos, cam_dir) << endl;

    vec3 screen_center = screen_dist * cam_dir + cam_pos;

    float step_dist = 1.0f / volume.ziSize;

    for (int i = 0; i < img_height; i++) {
        for (int j = 0; j < img_width; j++) {
            vec3 pixel_pos = screen_center + (-0.5f + (float) i / img_width) * cam_right +
                             (-0.5f + (float) j / img_height) * cam_up;
            //光线方向
            vec3 ray_dir = fastNormalize(pixel_pos - cam_pos);
            //计算进入点与离开点
            float begin = caculate_enter_dist(cam_pos, ray_dir);
            float end = caculate_leave_dist(cam_pos, ray_dir);
            glm::vec3 color_cum(0, 0, 0);
            float opacity_cum = 0;
            for (int k = 0; begin + k * step_dist < end; k++) {
                //TODO
                float value;  // = volume.data[k * volume.xiSize * volume.yiSize + j * volume.yiSize + i];
                vec3 pos(i, j, k);
                value = getVolumeValue(volume, pos);
                glm::vec4 color_and_alpha = volume.tf1d.trans_func(value);
                color_cum.r = glm::min(color_cum.r + (1.0f - opacity_cum) * color_and_alpha.r, 1.0f);
                color_cum.g = glm::min(color_cum.g + (1.0f - opacity_cum) * color_and_alpha.g, 1.0f);
                color_cum.b = glm::min(color_cum.b + (1.0f - opacity_cum) * color_and_alpha.b, 1.0f);
                opacity_cum = glm::min(opacity_cum + (1.0f - opacity_cum) * color_and_alpha.a, 1.0f);
                if (opacity_cum >= 1.0) break;
            }
            image.setPixel(i, j, qRgb((int) (255 * (1 - color_cum.r)), (int) (255 * (1 - color_cum.g)),
                                      (int) (255 * (1 - color_cum.b))));
        }
    }

//    for (int i = 0; i < img_height; i++) {
//        for (int j = 0; j < img_width; j++) {
//            glm::vec3 color_cum(0, 0, 0);
//            float opacity_cum = 0;
//            for (int k = volume.ziSize - 1; k >= 0; k--) {
//                glm::vec4 color_and_alpha = volume.tf1d.trans_func(
//                        volume.data[k * volume.xiSize * volume.yiSize + j * volume.yiSize + i]);
//                color_cum.r = min(color_cum.r + (1.0f - opacity_cum) * color_and_alpha.r, 1.0f);
//                color_cum.g = min(color_cum.g + (1.0f - opacity_cum) * color_and_alpha.g, 1.0f);
//                color_cum.b = min(color_cum.b + (1.0f - opacity_cum) * color_and_alpha.b, 1.0f);
//                opacity_cum = min(opacity_cum + (1.0f - opacity_cum) * color_and_alpha.a, 1.0f);
//                if (opacity_cum == 1.0) break;
//            }
//            image.setPixel(i, j, qRgb((int) (255 * (1 - color_cum.r)), (int) (255 * (1 - color_cum.g)),
//                                      (int) (255 * (1 - color_cum.b))));
//
//        }
//        }


    VolumeRender render;

    render.setImage(&image);

    render.show();

    return app.exec();
}