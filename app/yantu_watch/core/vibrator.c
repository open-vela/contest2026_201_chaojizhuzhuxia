/****************************************************************************
 * apps/examples/yantu_watch/core/vibrator.c
 *
 * 震动马达驱动模块 (PWM 控制)
 * - 静音震动提醒，不打扰自习环境
 * - 使用 NuttX PWM 驱动 ioctl 接口
 * - 支持不同震动模式（短震/长震/脉冲/双震/三连）
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <nuttx/timers/pwm.h>
#include "vibrator.h"

/* PWM device path for SF32LB52 vibration motor */
#define PWM_DEV_PATH   "/dev/pwm2"
#define PWM_FREQ       1000   /* 1kHz */
#define PWM_HALF_DUTY  (1L << 15)  /* ub16_t: 32768 = 50% duty */

/* Vibration patterns (duty, on_ms, off_ms, repeat) */
static const vibrate_pattern_def_t g_patterns[] = {
    [VIBRATE_SHORT]  = { PWM_HALF_DUTY, 200, 0,   1 },
    [VIBRATE_LONG]   = { PWM_HALF_DUTY, 500, 0,   1 },
    [VIBRATE_DOUBLE] = { PWM_HALF_DUTY, 200, 100, 2 },
    [VIBRATE_TRIPLE] = { PWM_HALF_DUTY, 300, 150, 3 },
    [VIBRATE_PULSE]  = { PWM_HALF_DUTY, 100, 50,  5 },
};

static int g_fd = -1;

/****************************************************************************
 * PWM 硬件控制
 ****************************************************************************/

/* 启动 PWM 输出 */
static int pwm_start(int duty)
{
  if (g_fd < 0) return -1;

  struct pwm_info_s info;
  info.frequency = PWM_FREQ;
  info.duty = duty;

  int ret = ioctl(g_fd, PWMIOC_SETCHARACTERISTICS, (unsigned long)&info);
  if (ret < 0)
    {
      printf("vibrator: PWMIOC_SETCHARACTERISTICS failed\n");
      return ret;
    }

  ret = ioctl(g_fd, PWMIOC_START, 0);
  if (ret < 0)
    {
      printf("vibrator: PWMIOC_START failed\n");
    }

  return ret;
}

/* 停止 PWM 输出 */
static int pwm_stop(void)
{
  if (g_fd < 0) return -1;

  int ret = ioctl(g_fd, PWMIOC_STOP, 0);
  if (ret < 0)
    {
      printf("vibrator: PWMIOC_STOP failed\n");
    }

  return ret;
}

/* 毫秒级延时 */
static void delay_ms(int ms)
{
  if (ms > 0)
    usleep(ms * 1000);
}

/****************************************************************************
 * 公开 API
 ****************************************************************************/

int vibrator_init(void)
{
  /* Open PWM device */
  g_fd = open(PWM_DEV_PATH, O_RDWR);
  if (g_fd < 0)
    {
      /* PWM device may not be available in simulator; log and continue */
      printf("vibrator: PWM device %s not available (simulator mode)\n",
             PWM_DEV_PATH);
      return 0;
    }

  printf("vibrator: PWM device %s opened successfully\n", PWM_DEV_PATH);
  return 0;
}

int vibrator_set(vibrate_pattern_t pattern)
{
  if (pattern >= VIBRATE_MAX) return -1;

  const vibrate_pattern_def_t *p = &g_patterns[pattern];

  printf("vibrator: pattern=%d duty=%d on=%dms off=%dms repeat=%d\n",
         pattern, p->duty, p->on_ms, p->off_ms, p->repeat);

  /* Execute the vibration pattern */
  for (int i = 0; i < p->repeat; i++)
    {
      /* Vibrate on */
      pwm_start(p->duty);
      delay_ms(p->on_ms);

      /* Vibrate off */
      pwm_stop();

      /* Wait between repeats if needed */
      if (i < p->repeat - 1 && p->off_ms > 0)
        delay_ms(p->off_ms);
    }

  return 0;
}

int vibrator_off(void)
{
  int ret = pwm_stop();

  if (g_fd >= 0)
    {
      close(g_fd);
      g_fd = -1;
    }

  return ret;
}