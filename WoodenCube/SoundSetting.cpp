#include "SoundSetting.h"
#include "DisplayManager.h"
#include "IMUManager.h"
#include "SoundManager.h"

namespace SoundSetting {
  void enter() {
    SoundManager::toggleMute();
    DisplayManager::showMuteStatus(SoundManager::isMuted());
  }

  void update() {
    // 保持状态即可
  }

  bool checkExit() {
    return IMUManager::isShaking();
  }
}
