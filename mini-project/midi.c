#include <stdint.h>
#include <string.h>
#include "midi.h"

#define WEAK __attribute__((weak))

static MIDI_Header header;
static MIDI_Player player;

typedef struct {
  const MIDI_Header *header;
  const uint8_t *end;
  const uint8_t *pos;
  int nexttick;
  uint8_t prev_cmd;
  uint8_t done;
} Track;

#define MAXTRACKS 35
static Track track[MAXTRACKS];

//====<Weak symbols for empty functions.  Override in other modules>=========
void WEAK midi_error(int time, const uint8_t *p, const uint8_t *file_start,
        const char *msg)
{
}
void WEAK header_info(MIDI_Header *hdr)
{
}
void WEAK file_offset_info(int time)
{
}
void WEAK track_info(int num, const uint8_t *p, int len, const MIDI_Header *hdr)
{
}
void WEAK sysex_info(int time, const uint8_t *p, const uint8_t *buf, int len,
                     const MIDI_Header *hdr)
{
}

void WEAK note_off(int time, int chan, int key, int velo)
{
}

void WEAK note_on(int time, int chan, int key, int velo)
{
}

void WEAK key_pressure(int time, int chan, int key, int val)
{
}

void WEAK control_change(int time, int chan, int ctrl, int val)
{
}

void WEAK program_change(int time, int chan, int prog)
{
}

void WEAK channel_pressure(int time, int chan, int val)
{
}

void WEAK pitch_wheel_change(int time, int chan, int val)
{
}
void WEAK text_event(const char *str, int len)
{
}
void WEAK copyright_notice(const char *str, int len)
{
}
void WEAK track_name(const char *str, int len)
{
}
void WEAK lyric(int time, const char *str, int len)
{
}
void WEAK marker(int time, const char *str, int len)
{
}
void WEAK end_of_track(int time)
{
}
void WEAK set_tempo(int time, int value, const MIDI_Header *hdr)
{
}
void WEAK smpte_offset(int time, int hr, int mn, int se, int fr, int ff)
{
}
void WEAK time_signature(int time, int n, int d, int b, int c)
{
}
void WEAK key_signature(int time, int8_t sf, int mi)
{
}
void WEAK unknown_meta(int time, int offset, int type, int len, const uint8_t *buf)
{
}
//=========================


//===========================================================================
// Parser support functions.
//===========================================================================
int intvar(const uint8_t **pp)
{
  int x = 0;
  do {
    x = (x << 7) | ((**pp) & 0x7f);
  } while( *((*pp)++) & 0x80);
  return x;
}
int int1(const uint8_t **pp)
{
  int x = 0;
  x = (x<<8) | *((*pp)++);
  return x;
}
int int2(const uint8_t **pp)
{
  int x = 0;
  x = (x<<8) | *((*pp)++);
  x = (x<<8) | *((*pp)++);
  return x;
}
int int4(const uint8_t **pp)
{
  int x = 0;
  x = (x<<8) | *((*pp)++);
  x = (x<<8) | *((*pp)++);
  x = (x<<8) | *((*pp)++);
  x = (x<<8) | *((*pp)++);
  return x;
}
int match(const uint8_t *p, const char *s)
{
  int len = strlen(s);
  if (strncmp(s, (char *)p, len) != 0)
    return 0;
  return 1;
}
int eat(const uint8_t **pp, const char *s)
{
  int len = strlen(s);
  if (strncmp(s, (const char *)*pp, len) != 0)
    return 0;
  *pp += len;
  return 1;
}

//===========================================================================
// Main interpretation functions.
//===========================================================================
void midi_event(int time, int event, const uint8_t **pp, const MIDI_Header *hdr)
{
  const uint8_t *p = *pp;
  uint8_t type = event & 0xf0;
  uint8_t chan = event & 0x0f;

  file_offset_info(*pp - hdr->file_start);
  switch(type) {
    case 0x80: { int note=int1(&p); int velo=int1(&p);
                 note_off(time, chan, note, velo); break;
               }
    case 0x90: { int note=int1(&p); int velo=int1(&p);
                 note_on(time, chan, note, velo); break;
               }
    case 0xa0: { int note=int1(&p); int value=int1(&p);
                 key_pressure(time, chan, note, value); break;
               }
    case 0xb0: { int ctrl=int1(&p); int value=int1(&p);
                 control_change(time, chan, ctrl, value); break;
               }
    case 0xc0: { int prog=int1(&p);
                 program_change(time, chan, prog); break;
               }
    case 0xd0: { int value=int1(&p);
                 channel_pressure(time, chan, value); break;
               }
    case 0xe0: { int l=int1(&p); int h=int1(&p);
                 pitch_wheel_change(time, chan, (h<<7)+l); break;
               }
  }

  *pp = p;
}

void meta_event(int time, const uint8_t **pp, const MIDI_Header *hdr)
{
  const uint8_t *p = *pp;
  int1(&p); // should be 0xff
  int type = int1(&p);
  int len = intvar(&p);
  const uint8_t *buf = p;
  p += len;

  file_offset_info(*pp - hdr->file_start);
  switch(type) {
    case 0x01: text_event((char *)buf,len); break;
    case 0x02: copyright_notice((char *)buf,len); break;
    case 0x03: track_name((char *)buf,len); break;
    case 0x05: lyric(time, (char *)buf,len); break;
    case 0x06: marker(time, (char *)buf,len); break;
    case 0x2f: end_of_track(time); break;
    case 0x51: set_tempo(time, (buf[0]<<16)+(buf[1]<<8)+buf[2], hdr); break;
    case 0x54: smpte_offset(time, buf[0], buf[1], buf[2], buf[3], buf[4]); break;
    case 0x58: time_signature(time, buf[0], buf[1], buf[2], buf[3]); break;
    case 0x59: key_signature(time, buf[0], buf[1]); break;
    default:   unknown_meta(time, p - hdr->file_start, type, len, buf); break;
  }
  *pp = p;
}

void sysex_event(int time, const uint8_t **pp, const MIDI_Header *hdr)
{
  const uint8_t *p = *pp;
  /* int event = */ int1(&p); // Should be 0xF0
  int len;
  for(len=0; p[len] != 0xf7; len++)
    ;
  len += 1; // Include the 0xf7 in the length.
  sysex_info(time, *pp, p, len, hdr);
  p += len;
  *pp = p;
}

void init_track(int num, const uint8_t *p, int len, const MIDI_Header *hdr)
{
  track_info(num, p, len, hdr);
  Track *t = &track[num];
  t->header = hdr;
  t->end = p + len;
  t->nexttick = intvar(&p);
  t->pos = p;
  t->prev_cmd = 0;
}

void track_interpret(Track *t, int time)
{
  const uint8_t *p = t->pos;
  while(t->nexttick == time) {
    if (*p < 0x80) { // Repeated Note Message
      midi_event(time, t->prev_cmd, &p, t->header);
    } else if (*p < 0xf0) { // Channel Voice Message
      t->prev_cmd = *p;
      p++;
      midi_event(time, t->prev_cmd, &p, t->header);
    } else if (*p == 0xf0) { // System Exclusive Message
      sysex_event(time, &p, t->header);
    } else if (*p < 0xff) { // What?
      midi_error(time, p, t->header->file_start, "Unknown message");
      p++; // skip it
    } else { // Meta Event Message
      meta_event(time, &p, t->header);
    }
    if (p >= t->end) {
      t->nexttick = 0x10000000;
    }
    t->nexttick += intvar(&p);
  }
  t->pos = p;
}

void midi_play(void)
{
  player.ticks++;
  if (player.ticks < player.nexttick)
    return;
  int n;
  for(n=0; n<header.tracks; n++) {
    Track *t = &track[n];
    if (t->nexttick == player.nexttick)
      track_interpret(t, player.nexttick);
  }
  player.nexttick=MAXTICKS;
  for(n=0; n<header.tracks; n++) {
    Track *t = &track[n];
    if (t->pos < t->end && t->nexttick < player.nexttick)
      player.nexttick = t->nexttick;
  }
}

MIDI_Player *midi_init(const uint8_t *file_start)
{
  const uint8_t *p = file_start;
  if (!eat(&p,"MThd")) midi_error(0, p, file_start, "No MThd found");
  header.file_start = file_start;
  header.length = int4(&p);
  header.format = int2(&p);
  header.tracks = int2(&p);
  header.divisions = int2(&p);
  header_info(&header);

  int n=0;
  while(eat(&p,"MTrk")) {
    int len = int4(&p);
    if (n >= MAXTRACKS)
      midi_error(0, p, file_start, "Too many tracks");
    else
      init_track(n++, p, len, &header);
    p += len;
  }

  player.ticks = 0;
  player.nexttick = 0;
  return &player;
}
