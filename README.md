# MIDI2149
Arduino library for YM2149-based MIDI synthesizer. Support multi YM2149 and it should also work on AY3891x chips.
It works with two YM2149 and Arduino UNO board.
[中文](README-zh.md)
## Dependencies
[ardumidi by hairless-midiserial](https://github.com/projectgus/hairless-midiserial/tree/master/ardumidi)
[AY3891x](https://github.com/Andy4495/AY3891x)

**NOTICE**
The official version of ardumidi has bugs, check my [PR](https://github.com/projectgus/hairless-midiserial/pull/81/files). 
I fixed it [here](https://github.com/Muenzuka-code/hairless-midiserial/tree/master/ardumidi).
## Usage
Install ardumidi and AY3891x to your Arduino IDE or add those files to your workspace.
Install hairless-midiserial and LoopMIDI for playing MIDI.
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