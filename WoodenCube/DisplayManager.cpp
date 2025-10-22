#include "DisplayManager.h"
#include "config.h"
#include "icons.h"
#include "SoundManager.h"  // 添加声音管理器头文件

// LedControl 对象，控制1个设备
static LedControl lc(LED_DIN_PIN, LED_CLK_PIN, LED_CS_PIN, 1);

namespace DisplayManager {

// 用于雨滴动画的数据结构
static uint8_t rainDrops[8] = {0};  // 每行8位，代表雨滴状态
static unsigned long lastRainUpdate = 0;
static const unsigned long rainUpdateInterval = 100; // 毫秒

// 用于贪吃蛇动画的帧计数
static int snakeFrame = 0;

// 静止检测用时间记录
static unsigned long lastActivityTime = 0;
static bool smileShown = false;

// 声音状态
extern bool soundEnabled;  // 从 SoundManager 中引用

void begin() {
    lc.shutdown(0, false); // 唤醒设备
    lc.setIntensity(0, 6); // 亮度0-15
    lc.clearDisplay(0);
    lastActivityTime = millis();
    smileShown = false;
}

void clear() {
    lc.clearDisplay(0);
}

void showIcon(const uint8_t icon[8]) {
    for (int row = 0; row < 8; row++) {
        lc.setRow(0, row, icon[row]);
    }
}

void showSmileFace() {
    showIcon(Icons::smileFace);
    
}

void showChar(char c) {
    // 简单支持 A-D, 0-9 显示，调用 icons.h 里的函数
    if (c >= '0' && c <= '9') {
        int digit = c - '0';
        Icons::displayDigit(&lc, digit, 2, 1);
    } else if (c >= 'A' && c <= 'D') {
        Icons::displayLetter(&lc, c, 2, 1);
    } else {
        clear();
    }
}

void drawHourglassFrame(unsigned long elapsed) {
    // 显示沙漏动画，elapsed毫秒根据时间切换图形（简单切换两帧）
    if ((elapsed / 500) % 2 == 0) {
        showIcon(Icons::HOURGLASS_ICON);
    } else {
        // 沙漏倒置版（自己做个简单反转）
        uint8_t flipped[8];
        for (int i = 0; i < 8; i++) {
            flipped[i] = Icons::HOURGLASS_ICON[7 - i];
        }
        showIcon(flipped);
    }
}

void initRain() {
    // 初始化雨滴状态随机
    for (int i = 0; i < 8; i++) {
        rainDrops[i] = random(0, 256);
    }
    lastRainUpdate = millis();
}

void updateRain() {
    unsigned long now = millis();
    if (now - lastRainUpdate < rainUpdateInterval) return;
    lastRainUpdate = now;

    // 雨滴从上向下移动
    for (int row = 7; row > 0; row--) {
        rainDrops[row] = rainDrops[row - 1];
    }
    // 顶行随机产生新雨滴
    rainDrops[0] = random(0, 256);

    // 显示雨滴
    for (int row = 0; row < 8; row++) {
        lc.setRow(0, row, rainDrops[row]);
    }
}

void drawSnakeFrame() {
    // 简单贪吃蛇动画，每次调用绘制不同帧
    static const uint8_t frames[][8] = {
        {B00000000, B00110000, B00011000, B00001100, B00000110, B00000011, B00000001, B00000000},
        {B00000000, B00001100, B00011000, B00110000, B01100000, B11000000, B10000000, B00000000},
    };
    showIcon(frames[snakeFrame % 2]);
    snakeFrame++;
}

void showGua(int index) {
    // 显示阴阳符号，第0-2行，index表示行数，true显示阳，false显示阴
    // 这里index代表第几行，演示显示阳（true）或阴（false）
    if (index < 0 || index > 2) return;
    // 清除显示，为演示每次显示一个阴阳符号
    clear();
    bool isYang = (index % 2 == 0);
    Icons::displayBaguaSymbol(&lc, isYang, index);
}

void showMuteStatus(bool muted) {
    if (muted) {
        showIcon(Icons::SOUND_MUTE_ICON);
    } else {
        showIcon(Icons::SOUND_ON_ICON);
    }
}

void playBootAnimation() {
    // 初始化变量
    int x = 3, y = 4;  // 中心点
    int dx = 0, dy = -1;  // 初始方向向上
    int steps = 1;
    int stepCount = 0;
    int totalSteps = 0;
    
    // 创建LED状态跟踪数组
    bool ledState[8][8] = {false};
    
    // 第一步：点亮中心点
    lc.setLed(0, y, x, true);
    ledState[y][x] = true;
    if (!SoundManager::isMuted()) {
        SoundManager::play(SoundManager::SND_STARTUP);  // 使用声音ID播放
    }
    delay(200);
    
    // 第二步：螺旋动画
    while (totalSteps < 64) {  // 最多64个LED
        // 移动到下一个位置
        x += dx;
        y += dy;
        
        // 检查边界
        if (x < 0 || x > 7 || y < 0 || y > 7) {
            // 改变方向
            int temp = dx;
            dx = -dy;
            dy = temp;
            x += dx;
            y += dy;
            
            // 增加步数
            stepCount++;
            if (stepCount == steps) {
                stepCount = 0;
                steps++;
            }
        }
        
        // 点亮当前位置
        if (!ledState[y][x]) {
            lc.setLed(0, y, x, true);
            ledState[y][x] = true;
            totalSteps++;
            
            // 计算延迟时间（从快到慢）
            int delayTime = 100 - (totalSteps * 1.5);  // 基础延迟从100ms逐渐减少
            if (delayTime < 20) delayTime = 20;  // 最小延迟20ms
            
            // 播放音效（频率随进度变化）
            if (!SoundManager::isMuted()) {
                int frequency = 500 + (totalSteps * 50);  // 频率从2000Hz逐渐增加
                if (frequency > 2000) frequency = 2000;    // 最大频率4000Hz
                SoundManager::playBeep(frequency, 10);  // 短促的音效
            }
            
            delay(delayTime);
        }
        
        // 更新方向
        stepCount++;
        if (stepCount == steps) {
            stepCount = 0;
            int temp = dx;
            dx = -dy;
            dy = temp;
            if (dx == 0) {
                steps++;
            }
        }
    }
    
    // 最终闪烁效果
    for (int i = 0; i < 2; i++) {
        lc.clearDisplay(0);
        if (!SoundManager::isMuted()) {
            SoundManager::playBeep(3000, 50);  // 高音
        }
        delay(100);
        
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                lc.setLed(0, row, col, true);
            }
        }
        
        if (!SoundManager::isMuted()) {
            SoundManager::playBeep(3500, 50);  // 更高音
        }
        delay(100);
    }
    
    // 最终停留
    delay(300);
    lc.clearDisplay(0);
}

void checkIdleAndShowSmile(unsigned long idleTimeoutMs) {
    unsigned long now = millis();
    if (now - lastActivityTime > idleTimeoutMs) {
        if (!smileShown) {
            showSmileFace();
            smileShown = true;
        }
    } else {
        smileShown = false;
    }
}

void showNumber2Digits(int num){
    // 确保数字在0-99范围内
    num = constrain(num, 0, 99);
    
    // 分离十位和个位
    int tens = num / 10;
    int ones = num % 10;
    
    // 清除显示
    clear();
    
    // 始终显示十位数
    Icons::displayDigit(&lc, tens, 0, 1);
    
    // 显示个位数
    Icons::displayDigit(&lc, ones, 4, 1);
}

} // namespace DisplayManager
