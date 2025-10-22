#ifndef ICONS_H
#define ICONS_H
#include <LedControl.h>

namespace Icons {
// 图标定义
const uint8_t smileFace[8] = {
  B00000000,
  B00000000,
  B01100110,
  B01100110,
  B00000000,
  B00111100,
  B00011000,
  B00000000
};

const byte HOURGLASS_ICON[8] = {
  B00011000,
  B01000010,
  B00010000,
  B10010001,
  B10011001,
  B00000000,
  B01000010,
  B00011000
};

const byte RandomABCD_ICON[8] = {
  B00000000,
  B00000111,
  B10111101,
  B10101101,
  B10101111,
  B10111000,
  B00000000,
  B11111111
};

const byte SNAKE_GAME_ICON[8] = {
  B00000000,
  B00110100,
  B00100000,
  B00111100,
  B00000100,
  B00000100,
  B01111100,
  B00000000
};

const byte PIXEL_RAIN_ICON[8] = {
  B00000000,
  B01001010,
  B01010000,
  B00100100,
  B01010010,
  B00010100,
  B01010010,
  B00000000
};

const byte DIVINATION_ICON[8] = {
  B01100110,
  B01100110,
  B00000000,
  B01111110,
  B01111110,
  B00000000,
  B01100110,
  B01100110
};

const byte SOUND_SETTINGS_ICON[8] = {
  B00000000,
  B00001100,
  B00011100,
  B01111100,
  B01111100,
  B00011100,
  B00001100,
  B00000000
};

const byte SOUND_MUTE_ICON[8] = {
  B00000000,
  B00001100,
  B00011110,
  B01111100,
  B01111100,
  B00111100,
  B01001100,
  B00000000
};

const byte SOUND_ON_ICON[8] = {
  B00000000,
  B00001100,
  B00011100,
  B01111100,
  B01111100,
  B00011100,
  B00001100,
  B00000000
};

const byte YIN_SYMBOL[2] = {
  B110011,
  B110011
};

const byte YANG_SYMBOL[2] = {
  B111111,
  B111111
};

// 数字定义 (3x6 像素)
const byte DIGITS[10][6] = {
  {
    B111,
    B101,
    B101,
    B101,
    B101,
    B111
  }, // 0
  {
    B010,
    B110,
    B010,
    B010,
    B010,
    B111
  }, // 1
  {
    B111,
    B001,
    B111,
    B100,
    B100,
    B111
  }, // 2
  {
    B111,
    B001,
    B111,
    B001,
    B001,
    B111
  }, // 3
  {
    B101,
    B101,
    B101,
    B111,
    B001,
    B001
  }, // 4
  {
    B111,
    B100,
    B111,
    B001,
    B001,
    B111
  }, // 5
  {
    B111,
    B100,
    B111,
    B101,
    B101,
    B111
  }, // 6
  {
    B111,
    B001,
    B001,
    B001,
    B001,
    B001
  }, // 7
  {
    B111,
    B101,
    B111,
    B101,
    B101,
    B111
  }, // 8
  {
    B111,
    B101,
    B111,
    B001,
    B001,
    B111
  }  // 9
};

// 字母定义 (4x6 像素)
const byte LETTERS[4][6] = {
  {
    B0110,
    B1001,
    B1001,
    B1111,
    B1001,
    B1001
  }, // A
  {
    B1110,
    B1001,
    B1110,
    B1001,
    B1001,
    B1110
  }, // B
  {
    B0111,
    B1000,
    B1000,
    B1000,
    B1000,
    B0111
  }, // C
  {
    B1110,
    B1001,
    B1001,
    B1001,
    B1001,
    B1110
  }  // D
};

// 数字显示函数，显示单个数字 (0-9) 在(x,y)位置
inline void displayDigit(class LedControl* lc, int digit, int x, int y) {
  if (digit < 0 || digit > 9) return;
  for (int row = 0; row < 6; row++) {
    for (int col = 0; col < 3; col++) {
      bool isOn = bitRead(DIGITS[digit][row], 2 - col);
      lc->setLed(0, y + row, x + col, isOn);
    }
  }
}

// 字母显示函数，显示字母 A-D 在(x,y)位置
inline void displayLetter(class LedControl* lc, char letter, int x=2, int y=1) {
  int index;
  switch (letter) {
    case 'A': index = 0; break;
    case 'B': index = 1; break;
    case 'C': index = 2; break;
    case 'D': index = 3; break;
    default: return;
  }
  for (int row = 0; row < 6; row++) {
    for (int col = 0; col < 4; col++) {
      bool isOn = bitRead(LETTERS[index][row], 3 - col);
      lc->setLed(0, y + row, x + col, isOn);
    }
  }
}

// 显示两位数字（00-99），居中显示
inline void displayNumber(class LedControl* lc, int number) {
  if (number < 0 || number > 99) return;
  int tens = number / 10;
  int ones = number % 10;
  lc->clearDisplay(0);
  displayDigit(lc, tens, 1, 1);
  displayDigit(lc, ones, 4, 1);
}

// 显示阴阳符号，isYang==true显示阳，false显示阴，line为第几行(0-2)
inline void displayBaguaSymbol(class LedControl* lc, bool isYang, int line) {
  if (line < 0 || line > 2) return;
  const byte* symbol = isYang ? YANG_SYMBOL : YIN_SYMBOL;
  int y = line * 3;
  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 6; col++) {
      bool isOn = bitRead(symbol[row], 5 - col);
      lc->setLed(0, y + row, col + 1, isOn);
    }
  }
}

} // namespace Icons

// 在命名空间定义后声明外部图标
extern const uint8_t iconHourglass[];
extern const uint8_t iconABCD[];
extern const uint8_t iconSnake[];
extern const uint8_t iconRain[];
extern const uint8_t iconDivination[];
extern const uint8_t iconSound[];

#endif // ICONS_H
