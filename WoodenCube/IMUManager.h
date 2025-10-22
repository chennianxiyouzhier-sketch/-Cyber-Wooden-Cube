#pragma once
#include <Arduino.h>

namespace IMUManager {
  void begin();                  // 初始化 MPU6050
  void update();                 // 更新加速度数据

  // 倾斜方向判断
  bool isTiltedLeft();          // 向左倾斜
  bool isTiltedRight();         // 向右倾斜
  bool isTiltedForward();       // 向前倾斜（低头）
  bool isTiltedBackward();      // 向后倾斜（仰头）
  bool isUpward();              // Z轴朝上
  bool isDownward();            // Z轴朝下
  bool isShaking();             // 检测是否在摇晃
  int getDirection();           // 返回方向编号（自定义逻辑）

  // 可选：整体是否倾斜（任意方向）
  bool isTilted();              // 检测是否偏离静止角度
  int  getTiltX();
  float getAccY();  // 获取 Y 轴加速度
  float getAccZ();  // 获取 Z 轴加速度
}
