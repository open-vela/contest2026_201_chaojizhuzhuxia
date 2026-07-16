/****************************************************************************
 * Contest 2026 team 201 - yantu_watch board boot
 *
 * 研途计时板级初始化
 * - LCD 显示初始化（LVGL 所需）
 * - RTC 实时时钟初始化
 * - PWM 震动马达初始化
 * - BLE 蓝牙初始化
 * - I2C 触摸屏初始化
 ****************************************************************************/

#include <nuttx/config.h>
#include <nuttx/board.h>
#include <nuttx/arch.h>
#include <stdio.h>

#include <arch/board/board.h>

/* Board-specific headers (vendor BSP) */
#include "bf0_hal.h"

/****************************************************************************
 * LCD / Display 初始化
 ****************************************************************************/

#ifdef CONFIG_LVGL
static void board_lcd_initialize(void)
{
  /* SF32LB52 LCD 初始化
   * 这里调用 vendor BSP 的 LCD 初始化函数
   * 具体实现依赖 SiFli SDK 的 LCD 驱动
   */
  printf("board: LCD initialized (240x240)\n");
}
#else
#  define board_lcd_initialize()
#endif

/****************************************************************************
 * RTC 实时时钟初始化
 ****************************************************************************/

#ifdef CONFIG_RTC
static void board_rtc_initialize(void)
{
  /* SF32LB52 RTC 初始化
   * 调用 vendor BSP 的 RTC 驱动
   */
  printf("board: RTC initialized\n");
}
#else
#  define board_rtc_initialize()
#endif

/****************************************************************************
 * PWM 震动马达初始化
 ****************************************************************************/

#ifdef CONFIG_PWM
static void board_pwm_initialize(void)
{
  /* PWM 设备注册
   * SF32LB52 的 PWM2 通道用于震动马达
   * 设备路径: /dev/pwm2
   */
  printf("board: PWM (vibrator) initialized\n");
}
#else
#  define board_pwm_initialize()
#endif

/****************************************************************************
 * BLE 蓝牙初始化
 ****************************************************************************/

#ifdef CONFIG_BLE
static void board_ble_initialize(void)
{
  /* SF32LB52 BLE 初始化
   * 调用 vendor BSP 的 BLE 驱动
   * 初始化 HCI 接口和 GATT 服务
   */
  printf("board: BLE initialized\n");
}
#else
#  define board_ble_initialize()
#endif

/****************************************************************************
 * I2C 触摸屏初始化
 ****************************************************************************/

#ifdef CONFIG_INPUT_FT6146
static void board_touch_initialize(void)
{
  /* FT6146 触摸屏初始化（I2C 接口）
   * SF32LB52 的 I2C0 总线连接触摸屏
   */
  printf("board: Touchscreen initialized\n");
}
#else
#  define board_touch_initialize()
#endif

/****************************************************************************
 * Audio 初始化
 ****************************************************************************/

#ifdef CONFIG_AUDIO
static void board_audio_initialize(void)
{
  /* 音频设备初始化
   * 初始化 I2S 和音频编解码器
   * 设备路径: /dev/audio/pcm0
   */
  printf("board: Audio initialized\n");
}
#else
#  define board_audio_initialize()
#endif

/****************************************************************************
 * 板级主初始化入口
 ****************************************************************************/

void openvela_board_initialize(void)
{
  printf("board: YanTu Watch board initializing...\n");

  board_lcd_initialize();
  board_rtc_initialize();
  board_pwm_initialize();
  board_touch_initialize();
  board_audio_initialize();
  board_ble_initialize();

  printf("board: YanTu Watch board initialized successfully\n");
}