#include "SnakeGame.h"
#include "DisplayManager.h"
#include "IMUManager.h"
#include "SoundManager.h"

namespace SnakeGame {
// 8x8地图
struct Point { int x, y; };
static Point snake[64]; // 最长64节
static int snakeLen;
static int dir; // 0上1右2下3左
static Point food;
int score = 0;
static bool gameOver = false;
static unsigned long gameOverTime = 0;
static unsigned long lastDirChange = 0;
static unsigned long moveInterval = 300;
static const unsigned long GAME_OVER_DISPLAY_TIME = 3000; // 游戏结束显示时间延长到3秒

// 随机生成食物，不能在蛇身上
static void spawnFood() {
  while (true) {
    food.x = random(0, 8);
    food.y = random(0, 8);
    bool onSnake = false;
    for (int i = 0; i < snakeLen; ++i) {
      if (snake[i].x == food.x && snake[i].y == food.y) { onSnake = true; break; }
    }
    if (!onSnake) break;
  }
}

void enter() {
  DisplayManager::clear();
  snakeLen = 3;
  snake[0] = {4, 4};
  snake[1] = {4, 5};
  snake[2] = {4, 6};
  dir = 0; // 初始向上
  score = 0;
  gameOver = false;
  moveInterval = 300; // 重置移动间隔
  spawnFood();
}

// 方向控制：IMUManager::getDirection() 返回值：
// 0: 正面朝上, 1: 背面朝上, 2: 左倾斜, 3: 右倾斜, 4: 前倾斜, 5: 后倾斜
static void updateDir() {
  int d = IMUManager::getDirection();
  unsigned long now = millis();
  if (now - lastDirChange < 300) return;

  int newDir = dir;

  // 映射陀螺仪返回值 -> 蛇的方向
  if (d == 3) newDir = 0;      // 前倾 -> 上
  else if (d == 1) newDir = 1; // 右倾 -> 右
  else if (d == 4) newDir = 2; // 后倾 -> 下
  else if (d == 2) newDir = 3; // 左倾 -> 左

  // 禁止蛇调头（例如：原来是右，不能立即改成左）
  if (newDir != (dir + 2) % 4 && newDir != dir) {
    dir = newDir;
    lastDirChange = now;
  }
}

static void draw() {
  uint8_t buf[8] = {0};
  // 画蛇
  for (int i = 0; i < snakeLen; ++i) {
    buf[snake[i].y] |= (1 << (7 - snake[i].x));
  }
  // 画食物
  buf[food.y] |= (1 << (7 - food.x));
  DisplayManager::showIcon(buf);
}

void update() {
  static unsigned long lastMove = 0;
  unsigned long now = millis();
  
  if (gameOver) {
    // 显示分数，延长显示时间到3秒
    if (now - gameOverTime < GAME_OVER_DISPLAY_TIME) {
      DisplayManager::showNumber2Digits(score);
    }
    return;
  }
  
  // 移动间隔
  if (now - lastMove < moveInterval) return;
  lastMove = now;
  
  // 更新方向
  updateDir();
  
  // 移动
  Point next = snake[0];
  if (dir == 0) next.y--;
  else if (dir == 1) next.x++;
  else if (dir == 2) next.y++;
  else if (dir == 3) next.x--;
  
  // 穿墙
  if (next.x < 0) next.x = 7;
  if (next.x > 7) next.x = 0;
  if (next.y < 0) next.y = 7;
  if (next.y > 7) next.y = 0;
  
  // 撞到自己
  for (int i = 0; i < snakeLen; ++i) {
    if (snake[i].x == next.x && snake[i].y == next.y) {
      gameOver = true;
      gameOverTime = now;
      SoundManager::play(SoundManager::SND_SNAKE_GAMEOVER);
      return;
    }
  }
  
  // 移动身体
  for (int i = snakeLen - 1; i > 0; --i) snake[i] = snake[i - 1];
  snake[0] = next;
  
  // 吃到食物
  if (next.x == food.x && next.y == food.y) {
    if (snakeLen < 64) snakeLen++;
    score++;
    SoundManager::play(SoundManager::SND_SNAKE_EAT);
    spawnFood();

    // 速度提高，间隔变短，不能低于最小值100ms
    if (moveInterval > 100) {
      moveInterval -= 10;
    }
  }
  
  draw();
}

bool checkExit() {
  if (gameOver && millis() - gameOverTime > GAME_OVER_DISPLAY_TIME) return true;
  return IMUManager::isShaking();
}

} // namespace SnakeGame
