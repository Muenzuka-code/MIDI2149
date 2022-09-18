#ifndef MIDI2149_H
#define MIDI2149_h

#include "AY3891x.h"
#include "ardumidi.h"
#include "midi2149config.h"
#include "notes.h"

#if !(MIDI2149_CONFIG_DONE)
#error "Please config midi2149config.h"
#endif

struct PSGChannelState{
    byte PSGChannel;
    byte note;
};
struct PSGChannelMap{
    byte MIDIChannel;
    byte chlidx;
};
struct MIDIChannelState{
    byte totalPSGChannel;
    byte freePSGChannel;
    PSGChannelState PSGChannels[MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL];
};

class MIDI2149
{
public:
    enum
    {
        PSG_NONE = 0,
#if (PSG_NUM > 0)
        PSG0_CHANNEL_A = 1,
        PSG0_CHANNEL_B = 2,
        PSG0_CHANNEL_C = 3,
#if (PSG_NUM > 1)
        PSG1_CHANNEL_A = 4,
        PSG1_CHANNEL_B = 5,
        PSG1_CHANNEL_C = 6,
#if (PSG_NUM > 2)
        PSG2_CHANNEL_A = 7,
        PSG2_CHANNEL_B = 8,
        PSG2_CHANNEL_C = 9,
#if (PSG_NUM > 3)
        PSG2_CHANNEL_A = 10,
        PSG2_CHANNEL_B = 11,
        PSG2_CHANNEL_C = 12,
#if (PSG_NUM > 4)
        PSG2_CHANNEL_A = 13,
        PSG2_CHANNEL_B = 14,
        PSG2_CHANNEL_C = 15,
#if (PSG_NUM > 5)
        PSG2_CHANNEL_A = 16,
        PSG2_CHANNEL_B = 17,
        PSG2_CHANNEL_C = 18,
#if (PSG_NUM > 6)
        PSG2_CHANNEL_A = 19,
        PSG2_CHANNEL_B = 20,
        PSG2_CHANNEL_C = 21,
#if (PSG_NUM > 7)
        PSG2_CHANNEL_A = 22,
        PSG2_CHANNEL_B = 23,
        PSG2_CHANNEL_C = 24,
#if (PSG_NUM > 8)
        PSG2_CHANNEL_A = 25,
        PSG2_CHANNEL_B = 26,
        PSG2_CHANNEL_C = 27,
#if (PSG_NUM > 9)
        PSG2_CHANNEL_A = 28,
        PSG2_CHANNEL_B = 29,
        PSG2_CHANNEL_C = 30,
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
        PSG_IDLE = 255,
    };
    MIDI2149(AY3891x _psgs[PSG_NUM]);
    void begin();
    void bindPSGChannel2MIDIchannel(byte PSGChannel,byte MIDIChannel);
    void unBindPSGChannel2MIDIchannel(byte PSGChannel);
    void listen();

private:
    AY3891x *psgs;
    byte mixs[PSG_NUM]={0};
    MIDIChannelState MIDIChannels[MAXINUM_MIDI_CHANNEL]={0};
    PSGChannelMap PSGChannelMaps[PSG_NUM*3];
    MidiMessage m;
    void onNoteOn();
    void onNoteOff();
    void onPressure();
    void onController();
    void onProgram();
    void onChannel();
    void onPitchBend(); 

    byte getPSGChannelByMIDI(byte MIDIChannel, byte note);
    byte getFreePSGChannel(byte MIDIChannel);
    void PSGChannelOn(byte PSGChannel);
    void PSGChannelOn(byte PSGChannel, byte note, byte velo);
    void PSGChannelOff(byte PSGChannel);
    void PSGOff(byte psg);
    void PSGChannelOffAll();

    byte fastlog2(byte);
};
#endif // MIDI2149_H