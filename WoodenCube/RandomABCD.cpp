#include "RandomABCD.h"
#include "DisplayManager.h"
#include "IMUManager.h"
#include "SoundManager.h"

namespace RandomABCD {

  static char currentChar = 'A';
  static bool isShuffling = false;         // 是否正在洗牌动画
  static unsigned long shuffleStartTime = 0;

  void enter() {
    DisplayManager::clear();
    currentChar = 'A';
    DisplayManager::showChar(currentChar);
    isShuffling = false;
  }

  void update() {
    unsigned long now = millis();

    if (isShuffling) {
      // 正在洗牌动画中
      if (now - shuffleStartTime < 1000) {
        // 每50ms换一个随机字母显示
        static unsigned long lastChangeTime = 0;
        if (now - lastChangeTime > 50) {
          currentChar = 'A' + random(0, 4);
          DisplayManager::showChar(currentChar);
          SoundManager::play(SoundManager::SND_RANDOM_CHANGING);
          lastChangeTime = now;
        }
      } else {
        // 洗牌结束，显示最终随机结果
        currentChar = 'A' + random(0, 4);
        DisplayManager::showChar(currentChar);
        SoundManager::play(SoundManager::SND_RANDOM_FINAL);
        isShuffling = false;
      }
    } else {
      // 非洗牌状态，检测X轴倾斜开始洗牌
      if (IMUManager::getDirection()==1) {
        isShuffling = true;
        shuffleStartTime = now;
        SoundManager::play(SoundManager::SND_MENU_SELECT);
      }
    }
  }

  bool checkExit() {
    if (IMUManager::getDirection()==5) {
      SoundManager::play(SoundManager::SND_MENU_RETURN);
      return true;  // 退出返回菜单
    }
    return false;
  }
}
