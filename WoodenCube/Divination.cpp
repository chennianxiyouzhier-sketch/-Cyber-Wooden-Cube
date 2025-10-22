#include "Divination.h"
#include "DisplayManager.h"
#include "IMUManager.h"
#include "icons.h"
#include "SoundManager.h"

namespace Divination {
  static byte gua[3]; // 保存当前卦象
  static bool generated = false;
  static unsigned long lastTiltTime = 0;
  static bool isShuffling = false;         // 是否正在洗牌动画
  static unsigned long shuffleStartTime = 0;

  // 将三爻图像绘制到8x8缓冲区，并居中显示
  void drawGua(byte gua[3]) {
    uint8_t buf[8] = {0};

    for (int i = 0; i < 3; ++i) {
      const byte* symbol = (gua[i] == 0) ? Icons::YIN_SYMBOL : Icons::YANG_SYMBOL;
      int row = i * 3;
      
      // 水平居中：将符号左移1位，使其在8x8矩阵中居中
      buf[row]   = symbol[0] << 1;
      buf[row+1] = symbol[1] << 1;
      // row+2 自动为 0，空行
    }

    DisplayManager::showIcon(buf);
  }

  void enter() {
    for (int i = 0; i < 3; ++i) {
      gua[i] = random(0, 2); // 生成阴（0）或阳（1）
    }
    drawGua(gua);
    isShuffling = false;
  }

  void update() {
    unsigned long now = millis();

    if (isShuffling) {
      // 正在洗牌动画中
      if (now - shuffleStartTime < 1000) {
        // 每50ms换一个随机卦象显示
        static unsigned long lastChangeTime = 0;
        if (now - lastChangeTime > 50) {
          for (int i = 0; i < 3; ++i) {
            gua[i] = random(0, 2);
          }
          drawGua(gua);
          SoundManager::play(SoundManager::SND_RANDOM_CHANGING);
          lastChangeTime = now;
        }
      } else {
        // 洗牌结束，显示最终随机结果
        for (int i = 0; i < 3; ++i) {
          gua[i] = random(0, 2);
        }
        drawGua(gua);
        SoundManager::play(SoundManager::SND_RANDOM_FINAL);
        isShuffling = false;
      }
    } else {
      // 检测X方向倾斜（左右倾）
      int dir = IMUManager::getDirection();
      if ((dir == 2 || dir == 1) && now - lastTiltTime > 1000) {
        isShuffling = true;
        shuffleStartTime = now;
        SoundManager::play(SoundManager::SND_MENU_SELECT);
      }
    }
  }

  bool checkExit() {
    return IMUManager::isShaking(); // 摇晃退出
  }
}
