#include "Hourglass.h"
#include "DisplayManager.h"
#include "IMUManager.h"
#include "SoundManager.h"

namespace Hourglass {
  static int minutes = 0;
  static bool isSetting = true;
  static unsigned long startTime;
  static unsigned long lastUpdateTime = 0;
  static unsigned long lastNumberChangeTime = 0;  // 用于控制数字变化速度
  static const int UPDATE_INTERVAL = 1000;  // 每秒更新一次
  static bool isRunning = false;
  static bool isComplete = false;

  // 去抖相关变量
  static unsigned long lastStateChangeTime = 0;
  static const unsigned long DEBOUNCE_DELAY = 500;

  // 数字调整速度控制
  static const int MIN_CHANGE_INTERVAL = 50;    // 最快50ms变化一次
  static const int MAX_CHANGE_INTERVAL = 500;   // 最慢500ms变化一次
  static const float TILT_THRESHOLD = 0.5;      // 加速度阈值
  static const float MIN_TILT = 0.1;            // 最小有效倾斜值

  void adjustNumber(int direction) {
    unsigned long currentTime = millis();
    float tiltAmount = abs(IMUManager::getAccY());  // 获取Y轴加速度绝对值
    
    // 如果倾斜太小，不进行变化
    if (tiltAmount < MIN_TILT) {
      return;
    }
    
    // 根据倾斜程度计算变化间隔
    int changeInterval = map(tiltAmount * 100, MIN_TILT * 100, TILT_THRESHOLD * 100, MAX_CHANGE_INTERVAL, MIN_CHANGE_INTERVAL);
    changeInterval = constrain(changeInterval, MIN_CHANGE_INTERVAL, MAX_CHANGE_INTERVAL);
    
    if (currentTime - lastNumberChangeTime >= changeInterval) {
      lastNumberChangeTime = currentTime;
      
      if (direction > 0) {  // 增加
        minutes = (minutes + 1) % 100;  // 循环到99
      } else {  // 减少
        minutes = (minutes - 1 + 100) % 100;  // 循环到99
      }
      
      DisplayManager::showNumber2Digits(minutes);
      SoundManager::play(SoundManager::SND_MENU_CHANGE);
    }
  }

  void enter() {
    DisplayManager::clear();
    minutes = 0;
    isSetting = true;
    isRunning = false;
    isComplete = false;
    DisplayManager::showNumber2Digits(minutes);
    lastStateChangeTime = millis();
  }

  void rotateHourglass() {
    isRunning = false;
    isSetting = true;
    enter();
  }

  void update() {
    unsigned long currentTime = millis();

    if (isSetting) {
      int dir = IMUManager::getDirection();
      float tiltAmount = IMUManager::getAccY();  // 获取Y轴加速度
      
      if (dir == 3) {  // 向上倾斜
        adjustNumber(1);
      } else if (dir == 4) {  // 向下倾斜
        adjustNumber(-1);
      } else if ((dir == 1 || dir == 2) && minutes > 0) {  // 左右倾斜确认
        if (currentTime - lastStateChangeTime > DEBOUNCE_DELAY) {
          isSetting = false;
          isRunning = true;
          startTime = currentTime;
          lastUpdateTime = startTime;
          SoundManager::play(SoundManager::SND_MENU_SELECT);
          lastStateChangeTime = currentTime;
        }
      }
    } else {
      if (isRunning) {
        // 每秒更新一次显示
        if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
          lastUpdateTime = currentTime;
          
          unsigned long elapsed = (currentTime - startTime) / 1000;
          unsigned long remaining = minutes * 60 - elapsed;

          if (remaining <= 0) {
            if (!isComplete) {
              isComplete = true;
              SoundManager::playBlueBirdTheme();  // 播放完整的结束旋律
            }
            DisplayManager::showNumber2Digits(0);
          } else {
            // 最后一分钟显示秒数
            if (remaining <= 60) {
              DisplayManager::showNumber2Digits(remaining);
            } else {
              // 显示剩余分钟数
              int remainingMinutes = remaining / 60;
              DisplayManager::showNumber2Digits(remainingMinutes);
            }

            // 最后10秒播放音效
            if (remaining <= 10 && remaining > 0) {
              SoundManager::play(SoundManager::SND_RANDOM_CHANGING);
            }
          }
        }
      } else {
        DisplayManager::showNumber2Digits(minutes);
      }
    }
  }

  bool checkExit() {
    // 在设置状态下，需要连续摇晃才能退出
    if (isSetting) {
      static unsigned long lastShakeTime = 0;
      static int shakeCount = 0;
      unsigned long currentTime = millis();
      
      if (IMUManager::isShaking()) {
        if (currentTime - lastShakeTime < 1000) {  // 1秒内的摇晃
          shakeCount++;
          if (shakeCount >= 3) {  // 需要连续摇晃3次
            return true;
          }
        } else {
          shakeCount = 1;  // 重置计数
        }
        lastShakeTime = currentTime;
      } else {
        if (currentTime - lastShakeTime > 2000) {  // 2秒无摇晃重置计数
          shakeCount = 0;
        }
      }
      return false;
    }
    
    // 在运行状态下，时间到或摇晃都可以退出
    if (!isSetting && (millis() - startTime) / 1000 >= minutes * 60) {
      return true;
    }
    return IMUManager::isShaking();
  }
}
