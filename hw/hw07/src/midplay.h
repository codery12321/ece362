#if !defined(__MIDIPLAY_H__)
#define __MIDIPLAY_H__

// The DAC rate
#define RATE 20000
// The wavetable size
#define N 1000

extern const int step[128];
extern short int wavetable[N];

void init_wavetable_sine(void);     // sine wave
void init_wavetable_sawtooth(void); // sawtooth wave
void init_wavetable_square(void);   // square wave
void init_wavetable_hybrid(void);   // combination of sine/sawtooth
void init_wavetable_hybrid2(void);  // another comb of sine/sawtooth

// The encoded MIDI file to play.
extern unsigned char midifile[];

#endif /* __MIDIPLAY_H__ */
