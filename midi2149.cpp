#include "midi2149.h"

MIDI2149::MIDI2149(AY3891x _psgs[PSG_NUM])
{
    psgs = _psgs;
}
void MIDI2149::begin()
{
    for (size_t i = 0; i < PSG_NUM; i++)
    {
        psgs[i].begin();
        mixs[i] = 255;
    }
    pinMode(LED_BUILTIN, OUTPUT);
}
void MIDI2149::bindPSGChannel2MIDIchannel(byte PSGChannel, byte MIDIChannel)
{
    byte first_empty = MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL;
    for (byte i = 0; i < MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL; i++)
    {
        if (MIDIChannels[MIDIChannel].PSGChannels[i].PSGChannel == MIDI2149::PSG_NONE)
        {
            first_empty = i;
            break;
        }
    }
    if (first_empty >= MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL ||
        MIDIChannels[MIDIChannel].totalPSGChannel >= MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL)
    {
        return;
    }
    PSGChannelMaps[PSGChannel - 1].MIDIChannel = MIDIChannel;
    PSGChannelMaps[PSGChannel - 1].chlidx = first_empty;

    MIDIChannels[MIDIChannel].PSGChannels[first_empty].PSGChannel = PSGChannel;
    MIDIChannels[MIDIChannel].PSGChannels[first_empty].note = MIDI2149::PSG_IDLE;
    MIDIChannels[MIDIChannel].totalPSGChannel++;
}
void MIDI2149::unBindPSGChannel2MIDIchannel(byte PSGChannel)
{
    byte midichl = PSGChannelMaps[PSGChannel - 1].MIDIChannel;
    byte chlidx = PSGChannelMaps[PSGChannel - 1].chlidx;
    PSGChannelMaps[PSGChannel - 1].MIDIChannel = 0;
    PSGChannelMaps[PSGChannel - 1].chlidx = 0;
    MIDIChannels[midichl].totalPSGChannel--;
    MIDIChannels[midichl].PSGChannels[chlidx].PSGChannel = MIDI2149::PSG_NONE;
    MIDIChannels[midichl].PSGChannels[chlidx].note = MIDI2149::PSG_IDLE;
}
void MIDI2149::listen()
{
    while (midi_message_available() > 0)
    {
        MidiMessage _m = read_midi_message();
        digitalWrite(LED_BUILTIN, HIGH);
        this->m = _m;

        switch (m.command)
        {
        case MIDI_NOTE_ON:
            onNoteOn();
            break;
        case MIDI_NOTE_OFF:
            onNoteOff();
            break;
        case MIDI_PRESSURE:
            onPressure();
            break;
        case MIDI_CONTROLLER_CHANGE:
            onController();
            break;
        case MIDI_PROGRAM_CHANGE:
            onProgram();
            break;
        case MIDI_CHANNEL_PRESSURE:
            onChannel();
            break;
        case MIDI_PITCH_BEND:
            onPitchBend();
            break;
        default:
            break;
        }
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void MIDI2149::onNoteOn()
{
    byte channel = m.channel;
    byte note = m.param1;
    byte velo = m.param2;
    if (velo == 0)
    {
        onNoteOff();
        return;
    }
    byte chlidx = getFreePSGChannel(channel);
    if (chlidx >= MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL)
    {
        return;
    }
    if (note < 35)
    {
        return;
    }
    MIDIChannels[channel].PSGChannels[chlidx].note = note;
    byte psgchl = MIDIChannels[channel].PSGChannels[chlidx].PSGChannel;
    if (channel != 9)
        PSGChannelOn(psgchl, note, velo);
    else
        PSGChannelOnPercussion(psgchl, note, velo);
}

void MIDI2149::onNoteOff()
{
    byte channel = m.channel;
    byte note = m.param1;
    byte psgchl = getPSGChannelByMIDI(channel, note);
    if (psgchl == MIDI2149::PSG_NONE)
    {
        return;
    }
    PSGChannelOff(psgchl);
}

void MIDI2149::onPressure() {}
void MIDI2149::onController()
{
    if (m.param1 == 7)
    {
        for (byte i = 0; i < MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL; i++)
        {
            byte PSGChannel = MIDIChannels[m.channel].PSGChannels[i].PSGChannel;
            if (PSGChannel != MIDI2149::PSG_NONE)
            {
                byte psg = (PSGChannel - 1) / 3;
                byte chl = (PSGChannel - 1) % 3;

                psgs[psg].write(chl + 8, m.param2 >> 3);
            }
        }
    }

    if (m.param1 >= 120)
    {
        PSGChannelOffAll();
        return;
    }
}
void MIDI2149::onProgram() {}
void MIDI2149::onChannel() {}
void MIDI2149::onPitchBend() {}

byte MIDI2149::getPSGChannelByMIDI(byte MIDIChannel, byte note)
{
    for (byte i = 0; i < MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL; i++)
    {
        if (MIDIChannels[MIDIChannel].PSGChannels[i].note == note)
        {
            return MIDIChannels[MIDIChannel].PSGChannels[i].PSGChannel;
        }
    }
    return MIDI2149::PSG_NONE;
}

byte MIDI2149::getFreePSGChannel(byte MIDIChannel)
{

    byte first_free = 0;
    for (first_free = 0; first_free < MAXINUM_PSG_CHANNEL_PER_MIDI_CHANNEL; first_free++)
    {
        PSGChannelState PSGChl = MIDIChannels[MIDIChannel].PSGChannels[first_free];
        if (PSGChl.PSGChannel != MIDI2149::PSG_NONE && PSGChl.note == MIDI2149::PSG_IDLE)
        {
            break;
        }
    }
    return first_free;
}

void MIDI2149::PSGChannelOn(byte PSGChannel)
{
    byte psg = (PSGChannel - 1) / 3;
    byte chl = (PSGChannel - 1) % 3;

    mixs[psg] = mixs[psg] & ~(byte)(1 << chl);
    psgs[psg].write(AY3891x::Enable_Reg, mixs[psg]);
}

void MIDI2149::PSGChannelOn(byte PSGChannel, byte note, byte velo)
{
    byte psg = (PSGChannel - 1) / 3;
    byte chl = (PSGChannel - 1) % 3;

    mixs[psg] = mixs[psg] & ~(byte)(1 << chl);
    psgs[psg].write(chl * 2 + 1, pgm_read_word(&notes[note]) >> 8);
    psgs[psg].write(chl * 2, pgm_read_word(&notes[note]) & TONE_GENERATOR_FINE);
    psgs[psg].write(AY3891x::Enable_Reg, mixs[psg]);
    psgs[psg].write(chl + 8, velo >> 3);
}

void MIDI2149::PSGChannelOnPercussion(byte PSGChannel, byte note, byte velo)
{
    if (note < 35)
    {
        return;
    }
    byte psg = (PSGChannel - 1) / 3;
    byte chl = (PSGChannel - 1) % 3;
    

    if (note < 37 || note == 41)
    {
        mixs[psg] = mixs[psg] | (byte)(1 << chl);
        psgs[psg].write(AY3891x::Noise_Period_Reg, 0x1F);
        psgs[psg].write(AY3891x::Env_Period_Coarse_Reg, 4);
    }
    else if (note < 42 || note == 47 || note == 48)
    {
        mixs[psg] = mixs[psg] & ~(byte)(1 << chl + 3) | (byte)(1 << chl);
        psgs[psg].write(AY3891x::Noise_Period_Reg, 0xF);
        psgs[psg].write(AY3891x::Env_Period_Coarse_Reg, 4);
    }
    else
    {
        mixs[psg] = mixs[psg] & ~(byte)(1 << chl + 3) | (byte)(1 << chl);
        psgs[psg].write(AY3891x::Noise_Period_Reg, 0x0);
        psgs[psg].write(AY3891x::Env_Period_Coarse_Reg, 0x1c);
    }
    psgs[psg].write(AY3891x::Env_Period_Fine_Reg, 0);
    psgs[psg].write(chl + 8, 0b0010000);
    psgs[psg].write(AY3891x::Env_Shape_Cycle, 0);

    psgs[psg].write(AY3891x::Enable_Reg, mixs[psg]);
}

void MIDI2149::PSGChannelOff(byte PSGChannel)
{
    byte psg = (PSGChannel - 1) / 3;
    byte chl = (PSGChannel - 1) % 3;
    mixs[psg] = mixs[psg] | (byte)(1 << chl) | (byte)(1 << (chl + 3));
    psgs[psg].write(AY3891x::Enable_Reg, mixs[psg]);

    byte midichl = PSGChannelMaps[PSGChannel - 1].MIDIChannel;
    byte chlidx = PSGChannelMaps[PSGChannel - 1].chlidx;
    MIDIChannels[midichl].PSGChannels[chlidx].note = MIDI2149::PSG_IDLE;
}

void MIDI2149::PSGChannelOffAll()
{
    for (byte i = 1; i <= PSG_NUM * 3; i++)
    {
        PSGChannelOff(i);
    }
}

byte MIDI2149::fastlog2(byte a)
{
    byte log = 0;
    while (a > 0)
    {
        a >> ++log;
    }
    return log;
}