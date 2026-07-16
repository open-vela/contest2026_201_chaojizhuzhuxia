/****************************************************************************
 * apps/examples/yantu_watch/core/audio_player.c
 *
 * 离线音频播放器模块
 * - 基于 NuttX Audio 框架（/dev/audio/pcm0）
 * - 控制接口：AUDIOIOC_START/STOP/PAUSE/RESUME/GETPOSITION
 * - 音量控制：AUDIOIOC_SETPARAMTER
 * - 从 Flash 扫描音频文件列表
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "audio_player.h"
#include "storage.h"

#define MAX_TRACKS 64
#define VOLUME_MAX 100

/* Audio device support (requires CONFIG_AUDIO in board config) */
#ifdef CONFIG_AUDIO
#  include <nuttx/audio/audio.h>
#  define AUDIO_DEV_PATH "/dev/audio/pcm0"
#else
/* Fallback ioctl definitions when CONFIG_AUDIO is not enabled */
#  define AUDIOIOC_START              0
#  define AUDIOIOC_STOP               0
#  define AUDIOIOC_PAUSE              0
#  define AUDIOIOC_RESUME             0
#  define AUDIOIOC_GETPOSITION        0
#  define AUDIOIOC_CONFIGURE          0
#  define AUDIOIOC_SETPARAMTER        0
#  define AUDIO_FMT_MP3               0
#  define AUDIO_FMT_PCM               0
#  define AUDIO_TYPE_OUTPUT           0
#  define AUDIO_TYPE_FEATURE          0
#  define AUDIO_DEV_PATH              ""
#endif

static audio_track_t g_tracks[MAX_TRACKS];
static int g_track_count = 0;
static int g_current_track = -1;
static int g_volume = 70;
static bool g_playing = false;
static bool g_paused = false;
static int g_position_sec = 0;
static time_t g_last_tick = 0;

/* Audio device file descriptor */
static int g_audio_fd = -1;

/****************************************************************************
 * 音频设备控制
 ****************************************************************************/

/* 打开音频设备 */
static int audio_dev_open(void)
{
  if (g_audio_fd >= 0)
    return 0; /* Already open */

  g_audio_fd = open(AUDIO_DEV_PATH, O_RDWR);
  if (g_audio_fd < 0)
    {
      printf("audio: cannot open %s (simulator mode)\n", AUDIO_DEV_PATH);
      return -1;
    }

  printf("audio: opened %s\n", AUDIO_DEV_PATH);
  return 0;
}

/* 关闭音频设备 */
static void audio_dev_close(void)
{
  if (g_audio_fd >= 0)
    {
      close(g_audio_fd);
      g_audio_fd = -1;
    }
}

/* 配置音频格式为 MP3 */
static int audio_dev_configure(void)
{
  if (g_audio_fd < 0) return -1;

#ifdef CONFIG_AUDIO
  struct audio_caps_s caps;
  memset(&caps, 0, sizeof(caps));
  caps.ac_len     = sizeof(caps);
  caps.ac_type    = AUDIO_TYPE_OUTPUT;
  caps.ac_format.hw = (1 << (AUDIO_FMT_MP3 - 1));
  caps.ac_channels = 2; /* Stereo */

  int ret = ioctl(g_audio_fd, AUDIOIOC_CONFIGURE, (unsigned long)&caps);
  if (ret < 0)
    {
      /* Try PCM format as fallback */
      caps.ac_format.hw = (1 << (AUDIO_FMT_PCM - 1));
      ret = ioctl(g_audio_fd, AUDIOIOC_CONFIGURE, (unsigned long)&caps);
    }

  return ret;
#else
  return 0;
#endif
}

/****************************************************************************
 * 公开 API
 ****************************************************************************/

int audio_init(void)
{
  g_track_count = 0;
  g_current_track = -1;
  g_playing = false;
  g_paused = false;
  g_volume = 70;
  g_position_sec = 0;

  /* Try to open and configure audio device */
  if (audio_dev_open() == 0)
    {
      audio_dev_configure();
    }

  return 0;
}

int audio_play(const char *filename)
{
  /* If filename is given, find the track index */
  if (filename != NULL)
    {
      for (int i = 0; i < g_track_count; i++)
        {
          if (strcmp(g_tracks[i].filename, filename) == 0)
            {
              g_current_track = i;
              break;
            }
        }
    }

  if (g_current_track < 0 || g_current_track >= g_track_count)
    return -1;

  g_playing = true;
  g_paused = false;
  g_position_sec = 0;
  g_last_tick = time(NULL);

  printf("audio: playing [%d] %s\n",
         g_current_track, g_tracks[g_current_track].filename);

  /* Start audio device playback */
  if (g_audio_fd >= 0)
    {
      ioctl(g_audio_fd, AUDIOIOC_START, 0);
    }

  return 0;
}

int audio_pause(void)
{
  if (!g_playing) return -1;
  g_paused = true;

  if (g_audio_fd >= 0)
    ioctl(g_audio_fd, AUDIOIOC_PAUSE, 0);

  return 0;
}

int audio_resume(void)
{
  if (!g_playing || !g_paused) return -1;
  g_paused = false;
  g_last_tick = time(NULL);

  if (g_audio_fd >= 0)
    ioctl(g_audio_fd, AUDIOIOC_RESUME, 0);

  return 0;
}

int audio_stop(void)
{
  g_playing = false;
  g_paused = false;
  g_position_sec = 0;

  if (g_audio_fd >= 0)
    ioctl(g_audio_fd, AUDIOIOC_STOP, 0);

  return 0;
}

int audio_seek(int seconds)
{
  if (g_current_track < 0) return -1;
  int duration = g_tracks[g_current_track].duration_sec;
  if (seconds < 0) seconds = 0;
  if (seconds > duration) seconds = duration;
  g_position_sec = seconds;

  /* Seek by restarting playback at new position */
  if (g_audio_fd >= 0)
    {
      ioctl(g_audio_fd, AUDIOIOC_STOP, 0);
      ioctl(g_audio_fd, AUDIOIOC_START, 0);
    }

  return 0;
}

int audio_get_position(void)
{
  /* Query hardware position if available */
#ifdef CONFIG_AUDIO
  if (g_audio_fd >= 0)
    {
      struct audio_info_s info;
      memset(&info, 0, sizeof(info));
      if (ioctl(g_audio_fd, AUDIOIOC_GETPOSITION,
                (unsigned long)&info) == 0)
        {
          if (info.samplerate > 0)
            return g_position_sec;
        }
    }
#endif

  /* Fallback: software timing */
  if (g_playing && !g_paused)
    {
      time_t now = time(NULL);
      int elapsed = (int)(now - g_last_tick);
      if (elapsed > 0)
        {
          g_position_sec += elapsed;
          g_last_tick = now;
          if (g_current_track >= 0)
            {
              int dur = g_tracks[g_current_track].duration_sec;
              if (g_position_sec > dur) g_position_sec = dur;
            }
        }
    }

  return g_position_sec;
}

int audio_get_duration(void)
{
  if (g_current_track >= 0 && g_current_track < g_track_count)
    return g_tracks[g_current_track].duration_sec;
  return 0;
}

bool audio_is_playing(void)
{
  return g_playing && !g_paused;
}

int audio_set_volume(int volume)
{
  if (volume < 0) volume = 0;
  if (volume > VOLUME_MAX) volume = VOLUME_MAX;
  g_volume = volume;

  /* Set volume via audio device */
#ifdef CONFIG_AUDIO
  if (g_audio_fd >= 0)
    {
      struct audio_caps_s caps;
      memset(&caps, 0, sizeof(caps));
      caps.ac_len     = sizeof(caps);
      caps.ac_type    = AUDIO_TYPE_FEATURE;
      caps.ac_format.hw = volume * 10;
      ioctl(g_audio_fd, AUDIOIOC_SETPARAMTER, (unsigned long)&caps);
    }
#endif

  return 0;
}

int audio_get_volume(void)
{
  return g_volume;
}

int audio_next_track(void)
{
  if (g_track_count == 0) return -1;

  if (g_current_track < g_track_count - 1)
    g_current_track++;
  else
    g_current_track = 0; /* Wrap around */

  return audio_play(NULL);
}

int audio_prev_track(void)
{
  if (g_track_count == 0) return -1;

  if (g_current_track > 0)
    g_current_track--;
  else
    g_current_track = g_track_count - 1; /* Wrap around */

  return audio_play(NULL);
}

int audio_scan_files(void)
{
  /* Scan /data/audio/ for MP3 files using storage_list */
  char files[64][64];
  int count = 64;

  if (storage_list("audio", files, &count) != 0)
    {
      g_track_count = 0;
      return 0;
    }

  g_track_count = 0;
  for (int i = 0; i < count && g_track_count < MAX_TRACKS; i++)
    {
      /* Accept .mp3 and .wav files */
      const char *ext = strrchr(files[i], '.');
      if (ext && (strcasecmp(ext, ".mp3") == 0 ||
                  strcasecmp(ext, ".wav") == 0))
        {
          audio_track_t *t = &g_tracks[g_track_count];
          strncpy(t->filename, files[i], sizeof(t->filename) - 1);

          /* Use filename (without extension) as title */
          char *dot = strrchr(files[i], '.');
          if (dot) *dot = '\0';
          strncpy(t->title, files[i], sizeof(t->title) - 1);
          if (dot) *dot = '.';

          /* Default duration (will be updated by decoder when playing) */
          t->duration_sec = 180;
          g_track_count++;
        }
    }

  return g_track_count;
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