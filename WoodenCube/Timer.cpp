#include "Timer.h"

namespace Timer {
  static unsigned long startTime = 0;

  void start() {
    startTime = millis();
  }

  void reset() {
    startTime = millis();
  }

  unsigned long elapsed() {
    return millis() - startTime;
  }

  bool isTimeout(unsigned long timeoutMs) {
    return elapsed() >= timeoutMs;
  }
}
