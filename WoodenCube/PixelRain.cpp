#include "PixelRain.h"
#include "DisplayManager.h"
#include "IMUManager.h"

namespace PixelRain {
  void enter() {
    DisplayManager::clear();
    DisplayManager::initRain();  // 可选函数，初始化雨点状态
  }

  void update() {
    DisplayManager::updateRain();  // 更新下雨效果
  }

  bool checkExit() {
    return IMUManager::isShaking();
  }
}
