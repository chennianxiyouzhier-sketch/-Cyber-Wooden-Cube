#include "config.h"
#include "icons.h"

#include "DisplayManager.h"
#include "IMUManager.h"
#include "SoundManager.h"
#include "Timer.h"

// 各功能模块头文件
#include "Hourglass.h"
#include "RandomABCD.h"
#include "SnakeGame.h"
#include "PixelRain.h"
#include "Divination.h"
#include "SoundSetting.h"

// 当前模式索引
int currentMode = 0;
bool inFeature = false;

// 每个模式的信息
typedef struct {
  void (*enter)();
  void (*update)();
  bool (*checkExit)();
  const byte* icon;
} Mode;

Mode modes[] = {
  { Hourglass::enter,     Hourglass::update,     Hourglass::checkExit,     iconHourglass },
  { RandomABCD::enter,    RandomABCD::update,    RandomABCD::checkExit,    iconABCD },
  { SnakeGame::enter,     SnakeGame::update,     SnakeGame::checkExit,     iconSnake },
  { PixelRain::enter,     PixelRain::update,     PixelRain::checkExit,     iconRain },
  { Divination::enter,    Divination::update,    Divination::checkExit,    iconDivination },
  { SoundSetting::enter,  SoundSetting::update,  SoundSetting::checkExit,  iconSound },
};

const unsigned long idleTimeout = 5 * 60 * 1000UL; // 5分钟
const int totalModes = sizeof(modes) / sizeof(Mode);

void setup() {
  Serial.begin(115200);  // 添加串口调试
  Serial.println("Starting setup...");
  
  DisplayManager::begin();
  Serial.println("DisplayManager initialized");
  
  IMUManager::begin();
  Serial.println("IMUManager initialized");
  
  SoundManager::begin(BEEP_PIN);
  Serial.println("SoundManager initialized");
  
  DisplayManager::playBootAnimation();
  Serial.println("Boot animation played");
  
  DisplayManager::showSmileFace();
  Serial.println("Smile face shown");
  
  delay(1000);
  Serial.println("Setup completed");
}

void loop() {
  IMUManager::update(); // 刷新IMU数据
  static unsigned long lastSwitchTime = 0;      // 上次切换菜单时间
  const unsigned long SWITCH_INTERVAL = 300;    // 菜单切换间隔

  // 重置空闲计时器
  if (IMUManager::isTilted()) {
    Timer::reset();
  }

  // 超时处理
  if (Timer::isTimeout(idleTimeout)) {
    DisplayManager::showSmileFace();
    inFeature = false;
    return;
  }

  // 功能运行中
  if (inFeature) {
    if (modes[currentMode].update) {
      modes[currentMode].update();
    }

    // 如果检测到摇晃，则退出功能
    if (IMUManager::isShaking()) {
      inFeature = false;
      DisplayManager::showIcon(modes[currentMode].icon);
      SoundManager::playMenuReturnSound();
      delay(300);
      return;
    }

    // 检查是否需要退出功能
    if (modes[currentMode].checkExit && modes[currentMode].checkExit()) {
      inFeature = false;
      DisplayManager::showIcon(modes[currentMode].icon);
      SoundManager::playMenuReturnSound();
      delay(300);
    }
    return;
  }

  // 模式选择界面
  int direction = IMUManager::getDirection();   // 获取当前稳定方向
  unsigned long now = millis();

  // 调试输出
  /*Serial.print("Direction: ");
  Serial.print(direction);
  Serial.print(" Current Mode: ");
  Serial.print(currentMode);
  Serial.print(" In Feature: ");
  Serial.println(inFeature);*/

  // 只在方向稳定且非正面朝上时才允许切换菜单
  if (direction != -1 && direction != 0 && (now - lastSwitchTime) > SWITCH_INTERVAL) {
    // 根据方向切换菜单
    if (direction == 1) {  // 向左倾斜
      currentMode = (currentMode - 1 + totalModes) % totalModes;
      DisplayManager::showIcon(modes[currentMode].icon);
      SoundManager::playMenuChangeSound();
      lastSwitchTime = now;
    } else if (direction == 2) {  // 向右倾斜
      currentMode = (currentMode + 1) % totalModes;
      DisplayManager::showIcon(modes[currentMode].icon);
      SoundManager::playMenuChangeSound();
      lastSwitchTime = now;
    }
  }

  // 如果检测到背面朝上（3），则进入功能
  if (direction == 3) {
    if (modes[currentMode].enter) {
      inFeature = true;
      modes[currentMode].enter();
      SoundManager::playMenuSelectSound();
      delay(300);
      //Serial.println("Entered feature mode");
    }
  }

  delay(50);  // 主循环延迟
}
