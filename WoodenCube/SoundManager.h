#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

namespace SoundManager {

// 声音ID枚举
enum SoundID {
  SND_SCROLL,
  SND_FINAL,
  SND_MENU_CHANGE,
  SND_MENU_SELECT,
  SND_MENU_RETURN,
  SND_HOURGLASS_COUNTDOWN,
  SND_HOURGLASS_COMPLETE,
  SND_RANDOM_CHANGING,
  SND_RANDOM_FINAL,
  SND_SNAKE_EAT,
  SND_SNAKE_GAMEOVER,
  SND_DIVINATION,
  SND_STARTUP,
  SND_BLUE_BIRD,  // 新增：青鸟主题曲
};

// 初始化蜂鸣器引脚
void begin(int pin = 6);

// 播放蜂鸣器音调，频率（Hz），时长（毫秒）
void playBeep(int frequency, int duration);

// 播放指定声音效果
void play(SoundID id);

// 静音切换
void toggleMute();

// 查询是否静音
bool isMuted();

// 各种音效函数
void playStartupSound(int progress);
void playMenuChangeSound();
void playMenuSelectSound();
void playMenuReturnSound();
void playHourglassCountdownSound();
void playHourglassCompleteSound();
void playRandomSelectionChangingSound();
void playRandomSelectionFinalSound();
void playSnakeEatSound();
void playSnakeGameOverSound();
void playDivinationSound();
void playBlueBirdTheme();  // 新增：播放青鸟主题曲

} // namespace SoundManager

#endif // SOUNDMANAGER_H
