# Tetris99Bot-C
Tetris99Bot的C++版本
通过FFmpeg获取采集卡画面，使用OpenCV进行识别，简单的基于评价函数的剪枝搜索，最后通过usb转串口发送信号至模拟usb手柄的arduino，最终实现一个Tetris99bot

这是4年来我第一次用C++，所以代码非常乱。

要想使用这个bot，你需要一张低延迟的采集卡和一个可以模拟usb手柄的设备，详情见wchill的[SwitchInputEmulators](https://github.com/wchill/SwitchInputEmulators)


需要的库：
- OpenCV 
- FFmpeg 
- SDL
- SDL2_image


![演示](demo.jpg)
