/****************************************************************************
 * apps/examples/yantu_watch/core/audio_player.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include "audio_player.h"

#define MAX_TRACKS 64
#define VOLUME_MAX 100

static audio_track_t g_tracks[MAX_TRACKS];
static int g_track_count = 0;
static int g_current_track = -1;
static int g_volume = 70;
static bool g_playing = false;

int audio_init(void)
{
  g_track_count = 0;
  g_current_track = -1;
  g_playing = false;
  g_volume = 70;
  return 0;
}

int audio_play(const char *filename)
{
  /* TODO: Implement MP3 playback via OpenVela audio SDK */
  g_playing = true;
  return 0;
}

int audio_pause(void)
{
  g_playing = false;
  return 0;
}

int audio_resume(void)
{
  g_playing = true;
  return 0;
}

int audio_stop(void)
{
  g_playing = false;
  g_current_track = -1;
  return 0;
}

int audio_seek(int seconds)
{
  /* TODO: Implement seek */
  return 0;
}

int audio_get_position(void)
{
  return 0;
}

int audio_get_duration(void)
{
  if (g_current_track >= 0 && g_current_track < g_track_count)
    return g_tracks[g_current_track].duration_sec;
  return 0;
}

bool audio_is_playing(void)
{
  return g_playing;
}

int audio_set_volume(int volume)
{
  if (volume < 0) volume = 0;
  if (volume > VOLUME_MAX) volume = VOLUME_MAX;
  g_volume = volume;
  return 0;
}

int audio_get_volume(void)
{
  return g_volume;
}

int audio_next_track(void)
{
  if (g_current_track < g_track_count - 1)
    {
      g_current_track++;
      return 0;
    }
  return -1;
}

int audio_prev_track(void)
{
  if (g_current_track > 0)
    {
      g_current_track--;
      return 0;
    }
  return -1;
}

int audio_scan_files(void)
{
  /* TODO: Scan /data/audio/ for MP3 files */
  g_track_count = 0;
  return 0;
}

int audio_get_track_count(void)
{
  return g_track_count;
}

int audio_get_track_info(int index, audio_track_t *track)
{
  if (index < 0 || index >= g_track_count) return -1;
  *track = g_tracks[index];
  return 0;
}