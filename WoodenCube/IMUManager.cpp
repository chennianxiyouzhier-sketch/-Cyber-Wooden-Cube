#include "IMUManager.h"
#include <Wire.h>
#include <MPU6050.h>

namespace IMUManager {
  MPU6050 mpu;

  // 原始加速度
  float accX = 0;
  float accY = 0;
  float accZ = 0;

  // 参考基准值（当前姿态）
  float baseX = 0;
  float baseY = 0;
  float baseZ = 0;

  // 静止阈值与防抖参数
  const float tiltThreshold = 0.3;
  const float shakeThreshold = 10;
  const float stillThreshold = 0.05;
  const unsigned long debounceDelay = 300;

  // 校准控制
  bool calibrated = false;

  // 倾斜方向防抖时间
  unsigned long lastLeftTrigger = 0;
  unsigned long lastRightTrigger = 0;
  unsigned long lastForwardTrigger = 0;
  unsigned long lastBackwardTrigger = 0;

  void begin() {
    Wire.begin();
    mpu.initialize();
    delay(100); // 等待设备稳定

    if (!mpu.testConnection()) {
      Serial.println("MPU6050 connection failed!");
      return;
    }

    Serial.println("MPU6050 connected.");
    Serial.println("等待设置参考姿态...");
    
    // 等待一小段时间让传感器稳定
    delay(500);
    
    // 读取当前姿态作为参考基准
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    
    baseX = ax / 16384.0;
    baseY = ay / 16384.0;
    baseZ = az / 16384.0;
    
    calibrated = true;
    Serial.println("参考姿态已设置:");
    Serial.print("X基准: "); Serial.println(baseX, 4);
    Serial.print("Y基准: "); Serial.println(baseY, 4);
    Serial.print("Z基准: "); Serial.println(baseZ, 4);
  }

  void update() {
    int16_t accX_int, accY_int, accZ_int;
    mpu.getAcceleration(&accX_int, &accY_int, &accZ_int);

    // 计算相对于基准的偏移
    accX = (accX_int / 16384.0) - baseX;
    accY = (accY_int / 16384.0) - baseY;
    accZ = (accZ_int / 16384.0) - baseZ;
  }

  bool isTilted() {
    return abs(accX) > tiltThreshold || abs(accY) > tiltThreshold;
  }

  bool isShaking() {
    static float lastX = 0, lastY = 0, lastZ = 0;
    static unsigned long lastTime = 0;

    unsigned long now = millis();
    if (now - lastTime < 50) return false;

    float dx = abs(accX - lastX);
    float dy = abs(accY - lastY);
    float dz = abs(accZ - lastZ);

    lastX = accX;
    lastY = accY;
    lastZ = accZ;
    lastTime = now;

    const float shakeDeltaThreshold = 1.2;
    const float absoluteThreshold = 3.5;

    return (
      dx > shakeDeltaThreshold || dy > shakeDeltaThreshold || dz > shakeDeltaThreshold ||
      abs(accX) > absoluteThreshold || abs(accY) > absoluteThreshold
    );
  }

  bool isUpward() {
    return accZ > 0.7;
  }

  bool isTiltedX() {
    return abs(accX) > tiltThreshold;
  }

  bool isTiltedLeftRaw()     { return accX < -tiltThreshold; }
  bool isTiltedRightRaw()    { return accX > tiltThreshold;  }
  bool isTiltedForwardRaw()  { return accY < -tiltThreshold; }
  bool isTiltedBackwardRaw() { return accY > tiltThreshold;  }

  bool isTiltedLeft() {
    static bool wasTilted = false;
    bool now = isTiltedLeftRaw();
    unsigned long nowTime = millis();
    if (now && !wasTilted && (nowTime - lastLeftTrigger > debounceDelay)) {
      lastLeftTrigger = nowTime;
      wasTilted = true;
      return true;
    }
    wasTilted = now;
    return false;
  }

  bool isTiltedRight() {
    static bool wasTilted = false;
    bool now = isTiltedRightRaw();
    unsigned long nowTime = millis();
    if (now && !wasTilted && (nowTime - lastRightTrigger > debounceDelay)) {
      lastRightTrigger = nowTime;
      wasTilted = true;
      return true;
    }
    wasTilted = now;
    return false;
  }

  bool isTiltedForward() {
    static bool wasTilted = false;
    bool now = isTiltedForwardRaw();
    unsigned long nowTime = millis();
    if (now && !wasTilted && (nowTime - lastForwardTrigger > debounceDelay)) {
      lastForwardTrigger = nowTime;
      wasTilted = true;
      return true;
    }
    wasTilted = now;
    return false;
  }

  bool isTiltedBackward() {
    static bool wasTilted = false;
    bool now = isTiltedBackwardRaw();
    unsigned long nowTime = millis();
    if (now && !wasTilted && (nowTime - lastBackwardTrigger > debounceDelay)) {
      lastBackwardTrigger = nowTime;
      wasTilted = true;
      return true;
    }
    wasTilted = now;
    return false;
  }

  int getDirection() {
    if (isShaking()) return 5;
    if (isTiltedLeft()) return 1;
    if (isTiltedRight()) return 2;
    if (isTiltedForward()) return 3;
    if (isTiltedBackward()) return 4;
    return 0;
  }
  int getTiltX() {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    return ax;
  }

  float getAccY() {
    return accY;  // 返回已校准的 Y 轴加速度
  }

  float getAccZ() {
    return accZ;  // 返回已校准的 Z 轴加速度
  }
}
