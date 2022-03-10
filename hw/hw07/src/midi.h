#ifndef __MIDI_H__
#define __MIDI_H__

typedef struct {
  const uint8_t *file_start;
  int length;
  uint8_t format;
  uint8_t tracks;
  uint16_t divisions;
} MIDI_Header;

#define MAXTICKS 0x10000000
typedef struct {
  int ticks;
  int nexttick;
} MIDI_Player;

// The core functions to interpret and play MIDI file contents...
MIDI_Player *midi_init(const uint8_t *array);
void midi_play(void);

// Functions you may implement to handle events in the real world.
void set_tempo(int time, int value, const MIDI_Header *hdr);
void note_off(int time, int chan, int key, int velo);
void note_on(int time, int chan, int key, int velo);

void key_pressure(int time, int chan, int key, int val);
void control_change(int time, int chan, int ctrl, int val);
void program_change(int time, int chan, int prog);
void channel_pressure(int time, int chan, int val);
void pitch_wheel_change(int time, int chan, int val);
void text_event(const char *str, int len);
void copyright_notice(const char *str, int len);
void track_name(const char *str, int len);
void lyric(int time, const char *str, int len);
void marker(int time, const char *str, int len);
void end_of_track(int time);
void smpte_offset(int time, int hr, int mn, int se, int fr, int ff);
void time_signature(int time, int n, int d, int b, int c);
void key_signature(int time, int8_t sf, int mi);

// Debugging.
void midi_error(int time, const uint8_t *p, const uint8_t *file_start, const char *msg);
void unknown_meta(int time, int offset, int type, int len, const uint8_t *buf);
void header_info(MIDI_Header *hdr);
void file_offset_info(int time);
void track_info(int num, const uint8_t *p, int len, const MIDI_Header *hdr);
void sysex_info(int time, const uint8_t *p, const uint8_t *buf, int len,
                const MIDI_Header *hdr);

#endif /* __MIDI_H__ */
