# MIDI2149
支持使用多片YM2149的MIDI合成器的Arduino库，AY3891系列芯片应该也可以使用。
使用2片YM2149和Arduino UNO进行了测试。
[English](README.md)
## 依赖
[ardumidi by hairless-midiserial](https://github.com/projectgus/hairless-midiserial/tree/master/ardumidi)
[AY3891x](https://github.com/Andy4495/AY3891x)
**注意**
官方版本的ardumidi有bug，可以参照我提交的[PR](https://github.com/projectgus/hairless-midiserial/pull/81/files)。
我的[fork](https://github.com/Muenzuka-code/hairless-midiserial/tree/master/ardumidi).里对这个bug进行了修正。
## 使用方法
将ardumidi和AY3891x安装到你的Arduino IDE中，或者把文件放到你的项目路径下。
安装hairless-midiserial与LoopMIDI用于播放MIDI。
### Example code
```c++
#include "midi2149.h"

AY3891x psg1(A3, 8, 7, 6, 5, 4, 2, 3, A2, A1, A0);
AY3891x psg2(A3, 8, 7, 6, 5, 4, 2, 3, 11, 10, 9);
AY3891x psgs[2] = {psg1, psg2};

MIDI2149 midi(psgs);
void setup()
{
  Serial.begin(115200);
  midi.begin();
  midi.bindPSGChannel2MIDIchannel(MIDI2149::PSG0_CHANNEL_A, 0);
  midi.bindPSGChannel2MIDIchannel(MIDI2149::PSG0_CHANNEL_B, 0);
  midi.bindPSGChannel2MIDIchannel(MIDI2149::PSG0_CHANNEL_C, 0);
  midi.bindPSGChannel2MIDIchannel(MIDI2149::PSG1_CHANNEL_A, 1);
  midi.bindPSGChannel2MIDIchannel(MIDI2149::PSG1_CHANNEL_B, 2);
  midi.bindPSGChannel2MIDIchannel(MIDI2149::PSG1_CHANNEL_C, 3);
}

void loop()
{
  midi.listen();
  delay(1);
}

```
Then enjoy.