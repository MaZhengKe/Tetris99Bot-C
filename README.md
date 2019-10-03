# Tetris99Bot-C

**请不要一遍又一遍的去运行这个程序看bot获胜，这只是在浪费你的时间，同时也毁坏了他人的游戏体验。**

这只是一个享受编程过程的玩具，或者是一个可以启发其他控制ns更有意思的项目的引子。

通过FFmpeg获取采集卡画面，使用OpenCV进行识别，简单的基于评价函数的剪枝搜索，最后通过usb转串口发送信号至模拟usb手柄的arduino，最终实现一个Tetris99bot

要想使用这个bot，你需要一张低延迟的采集卡和一个可以模拟usb手柄的设备，详情见wchill的[SwitchInputEmulator](https://github.com/wchill/SwitchInputEmulator)


需要的库：
- OpenCV 
- FFmpeg 
- SDL
- SDL2_image


![演示](demo.jpg)
