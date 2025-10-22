#pragma once

namespace RandomABCD {

  // 初始化进入随机选择界面，默认显示 'A'
  void enter();

  // 主循环调用函数，用于处理倾斜检测、洗牌动画和结果显示
  void update();

  // 检查是否发生摇晃退出，返回 true 表示需要退出当前模块
  bool checkExit();

}
