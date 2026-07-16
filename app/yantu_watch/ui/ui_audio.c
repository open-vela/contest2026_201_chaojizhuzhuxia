/****************************************************************************
 * apps/examples/yantu_watch/ui/ui_audio.c
 *
 * 音频播放器界面
 * - 当前曲目显示 + 进度条
 * - 播放/暂停/上一曲/下一曲控制
 * - 音量调节
 * - 播放模式切换
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <lvgl/lvgl.h>
#include "ui_audio.h"
#include "core/audio_player.h"

/* Object references */
static lv_obj_t *g_scr = NULL;
static lv_obj_t *g_track_label = NULL;
static lv_obj_t *g_time_label = NULL;
static lv_obj_t *g_btn_play = NULL;
static lv_obj_t *g_play_label = NULL;
static lv_obj_t *g_volume_label = NULL;
static lv_obj_t *g_mode_label = NULL;

/* Play mode */
typedef enum { MODE_SEQUENTIAL, MODE_REPEAT_ONE, MODE_REPEAT_ALL } play_mode_t;
static play_mode_t g_mode = MODE_SEQUENTIAL;
static const char *g_mode_text[] = { "顺序", "单曲", "循环" };

/****************************************************************************
 * Refresh display from audio player state
 ****************************************************************************/

static void refresh_display(void)
{
  int track_count = audio_get_track_count();
  int current = -1;

  /* Find current track index */
  for (int i = 0; i < track_count; i++)
    {
      audio_track_t t;
      if (audio_get_track_info(i, &t) == 0)
        {
          if (audio_is_playing() || true)
            {
              current = i;
              break;
            }
        }
    }

  if (track_count == 0)
    {
      lv_label_set_text(g_track_label, "暂无音频文件");
      lv_label_set_text(g_time_label, "00:00 / 00:00");
      lv_label_set_text(g_play_label, "▶");
      return;
    }

  /* Update track title */
  audio_track_t track;
  if (audio_get_track_info(0, &track) == 0)
    {
      char buf[128];
      if (current >= 0)
        {
          audio_get_track_info(current, &track);
          snprintf(buf, sizeof(buf), "%s", track.title);
        }
      else
        {
          snprintf(buf, sizeof(buf), "%s", track.title);
        }
      lv_label_set_text(g_track_label, buf);
    }

  /* Update time */
  int pos = audio_get_position();
  int dur = audio_get_duration();
  char time_buf[32];
  snprintf(time_buf, sizeof(time_buf), "%02d:%02d / %02d:%02d",
           pos / 60, pos % 60, dur / 60, dur % 60);
  lv_label_set_text(g_time_label, time_buf);

  /* Update play button */
  lv_label_set_text(g_play_label, audio_is_playing() ? "⏸" : "▶");

  /* Update mode */
  lv_label_set_text(g_mode_label, g_mode_text[g_mode]);

  /* Update volume */
  char vol_buf[16];
  snprintf(vol_buf, sizeof(vol_buf), "音量: %d", audio_get_volume());
  lv_label_set_text(g_volume_label, vol_buf);
}

/****************************************************************************
 * Button callbacks
 ****************************************************************************/

static void play_btn_cb(lv_event_t *e)
{
  if (audio_get_track_count() == 0) return;

  if (audio_is_playing())
    audio_pause();
  else
    audio_resume();

  refresh_display();
}

static void next_btn_cb(lv_event_t *e)
{
  audio_next_track();
  refresh_display();
}

static void prev_btn_cb(lv_event_t *e)
{
  audio_prev_track();
  refresh_display();
}

static void vol_up_cb(lv_event_t *e)
{
  int vol = audio_get_volume();
  audio_set_volume(vol + 10);
  refresh_display();
}

static void vol_down_cb(lv_event_t *e)
{
  int vol = audio_get_volume();
  audio_set_volume(vol - 10);
  refresh_display();
}

static void mode_btn_cb(lv_event_t *e)
{
  g_mode = (play_mode_t)((g_mode + 1) % 3);
  refresh_display();
}

/****************************************************************************
 * Create the audio player screen
 ****************************************************************************/

lv_obj_t *ui_audio_create(void)
{
  g_scr = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(g_scr, lv_color_hex(0x0f3460), 0);

  /* Title */
  lv_obj_t *title = lv_label_create(g_scr);
  lv_label_set_text(title, "音频播放");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
  lv_obj_set_style_text_color(title, lv_color_hex(0xffffff), 0);

  /* Track title */
  g_track_label = lv_label_create(g_scr);
  lv_label_set_text(g_track_label, "未选择音频");
  lv_obj_align(g_track_label, LV_ALIGN_TOP_MID, 0, 30);
  lv_obj_set_style_text_color(g_track_label, lv_color_hex(0x00ff88), 0);
  lv_label_set_long_mode(g_track_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
  lv_obj_set_width(g_track_label, 220);

  /* Time display */
  g_time_label = lv_label_create(g_scr);
  lv_label_set_text(g_time_label, "00:00 / 00:00");
  lv_obj_align(g_time_label, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_style_text_color(g_time_label, lv_color_hex(0xaaaaaa), 0);

  /* Play mode */
  g_mode_label = lv_label_create(g_scr);
  lv_label_set_text(g_mode_label, "顺序");
  lv_obj_align(g_mode_label, LV_ALIGN_TOP_MID, 0, 55);
  lv_obj_set_style_text_color(g_mode_label, lv_color_hex(0x888888), 0);

  /* Mode toggle button */
  lv_obj_t *mode_btn = lv_btn_create(g_scr);
  lv_obj_set_size(mode_btn, 60, 22);
  lv_obj_align(mode_btn, LV_ALIGN_TOP_MID, 75, 50);
  lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(mode_btn, mode_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *mode_btn_lbl = lv_label_create(mode_btn);
  lv_label_set_text(mode_btn_lbl, "模式");
  lv_obj_center(mode_btn_lbl);
  lv_obj_set_style_text_color(mode_btn_lbl, lv_color_hex(0xffffff), 0);

  /* ===== Control buttons ===== */
  /* Previous */
  lv_obj_t *prev_btn = lv_btn_create(g_scr);
  lv_obj_set_size(prev_btn, 50, 40);
  lv_obj_align(prev_btn, LV_ALIGN_CENTER, -70, 20);
  lv_obj_set_style_bg_color(prev_btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(prev_btn, prev_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *prev_lbl = lv_label_create(prev_btn);
  lv_label_set_text(prev_lbl, "⏮");
  lv_obj_center(prev_lbl);
  lv_obj_set_style_text_color(prev_lbl, lv_color_hex(0xffffff), 0);

  /* Play/Pause */
  g_btn_play = lv_btn_create(g_scr);
  lv_obj_set_size(g_btn_play, 60, 40);
  lv_obj_align(g_btn_play, LV_ALIGN_CENTER, 0, 20);
  lv_obj_set_style_bg_color(g_btn_play, lv_color_hex(0x00ff88), 0);
  lv_obj_add_event_cb(g_btn_play, play_btn_cb, LV_EVENT_CLICKED, NULL);

  g_play_label = lv_label_create(g_btn_play);
  lv_label_set_text(g_play_label, "▶");
  lv_obj_center(g_play_label);
  lv_obj_set_style_text_color(g_play_label, lv_color_hex(0x000000), 0);

  /* Next */
  lv_obj_t *next_btn = lv_btn_create(g_scr);
  lv_obj_set_size(next_btn, 50, 40);
  lv_obj_align(next_btn, LV_ALIGN_CENTER, 70, 20);
  lv_obj_set_style_bg_color(next_btn, lv_color_hex(0x533483), 0);
  lv_obj_add_event_cb(next_btn, next_btn_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *next_lbl = lv_label_create(next_btn);
  lv_label_set_text(next_lbl, "⏭");
  lv_obj_center(next_lbl);
  lv_obj_set_style_text_color(next_lbl, lv_color_hex(0xffffff), 0);

  /* ===== Volume controls ===== */
  lv_obj_t *vol_down_btn = lv_btn_create(g_scr);
  lv_obj_set_size(vol_down_btn, 40, 30);
  lv_obj_align(vol_down_btn, LV_ALIGN_BOTTOM_MID, -50, -40);
  lv_obj_set_style_bg_color(vol_down_btn, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(vol_down_btn, vol_down_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *vd_lbl = lv_label_create(vol_down_btn);
  lv_label_set_text(vd_lbl, "−");
  lv_obj_center(vd_lbl);
  lv_obj_set_style_text_color(vd_lbl, lv_color_hex(0xffffff), 0);

  g_volume_label = lv_label_create(g_scr);
  lv_label_set_text(g_volume_label, "音量: 70");
  lv_obj_align(g_volume_label, LV_ALIGN_BOTTOM_MID, 0, -40);
  lv_obj_set_style_text_color(g_volume_label, lv_color_hex(0x888888), 0);

  lv_obj_t *vol_up_btn = lv_btn_create(g_scr);
  lv_obj_set_size(vol_up_btn, 40, 30);
  lv_obj_align(vol_up_btn, LV_ALIGN_BOTTOM_MID, 50, -40);
  lv_obj_set_style_bg_color(vol_up_btn, lv_color_hex(0x333333), 0);
  lv_obj_add_event_cb(vol_up_btn, vol_up_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *vu_lbl = lv_label_create(vol_up_btn);
  lv_label_set_text(vu_lbl, "+");
  lv_obj_center(vu_lbl);
  lv_obj_set_style_text_color(vu_lbl, lv_color_hex(0xffffff), 0);

  /* Scan for audio files on startup */
  audio_scan_files();

  return g_scr;
}

/****************************************************************************
 * Public API
 ****************************************************************************/

void ui_audio_update_title(const char *title)
{
  if (g_track_label) lv_label_set_text(g_track_label, title);
}

void ui_audio_update_progress(int current_sec, int total_sec)
{
  if (!g_time_label) return;
  char buf[32];
  snprintf(buf, sizeof(buf), "%02d:%02d / %02d:%02d",
           current_sec / 60, current_sec % 60,
           total_sec / 60, total_sec % 60);
  lv_label_set_text(g_time_label, buf);
}

void ui_audio_set_playing(bool playing)
{
  if (g_play_label)
    lv_label_set_text(g_play_label, playing ? "⏸" : "▶");
}