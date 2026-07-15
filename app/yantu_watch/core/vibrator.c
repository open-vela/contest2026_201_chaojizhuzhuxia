/****************************************************************************
 * apps/examples/yantu_watch/core/vibrator.c
 *
 * 震动马达驱动模块 (PWM 控制)
 * - 静音震动提醒，不打扰自习环境
 * - 支持不同震动模式（短震/长震/脉冲）
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include "vibrator.h"

/* PWM device path for SF32LB52 vibration motor */
#define PWM_DEV_PATH   "/dev/pwm2"
#define PWM_CHANNEL    0
#define PWM_FREQ       1000   /* 1kHz */
#define PWM_MAX_DUTY   500000 /* 50% duty for vibration */

/* Vibration patterns (duty cycle, duration ms, repeat) */
static const vibrate_pattern_def_t g_patterns[] = {
    [VIBRATE_SHORT]  = { PWM_MAX_DUTY, 200, 0, 1    },
    [VIBRATE_LONG]   = { PWM_MAX_DUTY, 500, 0, 1    },
    [VIBRATE_DOUBLE] = { PWM_MAX_DUTY, 200, 100, 2  },
    [VIBRATE_TRIPLE] = { PWM_MAX_DUTY, 300, 150, 3  },
    [VIBRATE_PULSE]  = { PWM_MAX_DUTY, 100, 50, 5   },
};

static int g_fd = -1;

int vibrator_init(void)
{
  /* Open PWM device */
  g_fd = open(PWM_DEV_PATH, O_RDWR);
  if (g_fd < 0)
    {
      /* PWM device may not be available in simulator; log and continue */
      printf("vibrator: PWM device %s not available\n", PWM_DEV_PATH);
      return 0;
    }

  /* Configure PWM frequency */
  /* TODO: Use PWM ioctl to set frequency */
  return 0;
}

int vibrator_set(vibrate_pattern_t pattern)
{
  if (pattern >= VIBRATE_MAX) return -1;

  const vibrate_pattern_def_t *p = &g_patterns[pattern];

  /* TODO: Implement actual PWM vibration sequence
   * For now, just log the vibration event */
  printf("vibrator: pattern=%d duty=%d on=%dms off=%dms repeat=%d\n",
         pattern, p->duty, p->on_ms, p->off_ms, p->repeat);

  return 0;
}

int vibrator_off(void)
{
  if (g_fd >= 0)
    {
      /* Set PWM duty to 0 to stop vibration */
      /* TODO: PWM ioctl to set duty cycle to 0 */
    }
  return 0;
}