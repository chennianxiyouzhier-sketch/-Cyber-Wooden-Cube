#pragma once
#include <Arduino.h>

namespace Timer {
  // 启动计时器（从当前时间开始计时）
  void start();

  // 重新设置计时器（重置起始时间）
  void reset();

  // 获取已经过去的毫秒数
  unsigned long elapsed();

  // 判断是否超过指定毫秒数
  bool isTimeout(unsigned long timeoutMs);
}
