#include <Arduino.h>
#include "SoundManager.h"
#include "config.h"
#include "pitches.h"

namespace SoundManager {
  
  static int _buzzerPin = -1;  // 默认未初始化

  bool muted = false;

  void toggleMute() {
    muted = !muted;
  }

  bool isMuted() {
    return muted;
  }

  void begin(int pin) {
    _buzzerPin = pin;
    pinMode(_buzzerPin, OUTPUT);
  }

  void playBeep(int frequency, int duration) {
    if (muted) return;  // 静音状态下不播放
    if (_buzzerPin < 0) return;  // 未初始化蜂鸣器引脚则不播放
    tone(_buzzerPin, frequency, duration);
  }

  void playStartupSound(int progress) {
    int frequency = 40 + (progress * 5);
    playBeep(frequency, 50);
  }

  void playMenuChangeSound() {
    playBeep(800, 30);
  }

  void playMenuSelectSound() {
    playBeep(1000, 50);
    delay(50);
    playBeep(1200, 70);
  }

  void playMenuReturnSound() {
    playBeep(1200, 50);
    delay(50);
    playBeep(1000, 70);
  }

  void playHourglassCountdownSound() {
    playBeep(500, 100);
  }

  void playHourglassCompleteSound() {
    for (int i = 0; i < 3; i++) {
      playBeep(800, 100);
      delay(100);
      playBeep(1000, 100);
      delay(100);
    }
    playBeep(1200, 200);
  }

  void playRandomSelectionChangingSound() {
    int frequency = random(600, 1200);
    playBeep(frequency, 30);
  }

  void playRandomSelectionFinalSound() {
    playBeep(1000, 100);
    delay(100);
    playBeep(1200, 150);
  }

  void playSnakeEatSound() {
    playBeep(900, 50);
  }

  void playSnakeGameOverSound() {
    playBeep(300, 200);
    delay(100);
    playBeep(200, 300);
  }

  void playDivinationSound() {
    for (int i = 0; i < 5; i++) {
      int frequency = random(400, 800);
      playBeep(frequency, 50);
      delay(60);
    }
    playBeep(1000, 200);
  }

  // 青鸟主题曲开场旋律
  void playBlueBirdTheme() {
    if (_buzzerPin < 0) return;  // 未初始化蜂鸣器引脚则不播放

    // 节拍时间单位（单位：ms）
    const int tempo = 140;
    const int quarterNote = 60000 / tempo;

    // 音符结构体
    struct Note {
    int pitch;
    float duration;
    };

    // 旋律音符
    Note melody[] = {
    {NOTE_E4, 0.5}, {NOTE_A4, 0.5}, {NOTE_B4, 0.5},
    {NOTE_C5, 2}, {NOTE_B4, 1.5}, {NOTE_A4, 2.5},
    {REST, 0.5}, {NOTE_E4, 0.5}, {NOTE_A4, 0.5}, {NOTE_B4, 0.5},
    {NOTE_C5, 2}, {NOTE_D5, 1}, {NOTE_C5, 0.5}, {NOTE_D5, 1},
    {NOTE_E5, 1.5}, {REST, 0.5}, {NOTE_E5, 0.5}, {NOTE_A4, 0.5}, {NOTE_B4, 0.5},
    {NOTE_C5, 2}, {NOTE_B4, 1.5}, {NOTE_A4, 2.5},
    {REST, 0.5}, {NOTE_A4, 0.5}, {NOTE_E5, 0.5}, {NOTE_D5, 1},
    {NOTE_A4, 0.5}, {NOTE_E5, 0.5}, {NOTE_D5, 1}, {NOTE_G4, 1}, {NOTE_G4, 1},
    {NOTE_A4, 0.5}, {NOTE_A4, 3}
    };

    int melodyLength = sizeof(melody) / sizeof(Note);

    // 播放旋律
    for (int i = 0; i < melodyLength; i++) {
      int duration = melody[i].duration * quarterNote;

      if (melody[i].pitch == REST) {
      delay(duration);
      } else {
        playBeep(melody[i].pitch, duration * 0.9);  // 发声部分
        delay(duration);                            // 等待总时长，确保音长一致（playBeep 不阻塞）
        noTone(_buzzerPin);                         // 主动关闭蜂鸣器，避免拖音
      }
    }
  }
  void play(SoundID id) {
    switch(id) {
      case SND_SCROLL:
      case SND_RANDOM_CHANGING:
        playRandomSelectionChangingSound();
        break;

      case SND_FINAL:
      case SND_RANDOM_FINAL:
        playRandomSelectionFinalSound();
        break;

      case SND_MENU_CHANGE:
        playMenuChangeSound();
        break;

      case SND_MENU_SELECT:
        playMenuSelectSound();
        break;

      case SND_MENU_RETURN:
        playMenuReturnSound();
        break;

      case SND_HOURGLASS_COUNTDOWN:
        playHourglassCountdownSound();
        break;

      case SND_HOURGLASS_COMPLETE:
        playHourglassCompleteSound();
        break;

      case SND_SNAKE_EAT:
        playSnakeEatSound();
        break;

      case SND_SNAKE_GAMEOVER:
        playSnakeGameOverSound();
        break;

      case SND_DIVINATION:
        playDivinationSound();
        break;

      case SND_STARTUP:
        playStartupSound(0);  // 使用默认进度值0
        break;

      case SND_BLUE_BIRD:
        playBlueBirdTheme();  // 直接调用播放青鸟主题曲的函数
        break;

      default:
        // 非法ID不做处理
        break;
    }
  }

} // namespace SoundManager
