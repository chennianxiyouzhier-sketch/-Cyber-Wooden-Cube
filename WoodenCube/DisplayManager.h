#pragma once
#include <Arduino.h>
#include <LedControl.h>

namespace DisplayManager {

void begin();
void clear();
void showIcon(const uint8_t icon[8]);
void showChar(char c);
void drawHourglassFrame(unsigned long elapsed);
void initRain();
void updateRain();
void drawSnakeFrame();
void showGua(int index);
void showMuteStatus(bool muted);
void playBootAnimation();  // 开机螺旋动画
void showSmileFace();      // 显示笑脸图标
// 新增：检测静止时间，显示笑脸
void checkIdleAndShowSmile(unsigned long idleTimeoutMs);
void showNumber2Digits(int num);

} // namespace DisplayManager
