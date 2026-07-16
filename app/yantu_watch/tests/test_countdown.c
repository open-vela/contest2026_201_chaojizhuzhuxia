/****************************************************************************
 * apps/examples/yantu_watch/tests/test_countdown.c
 *
 * 考研倒计时模块单元测试
 ****************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "../core/countdown_manager.h"

int main(void)
{
  printf("=== 测试考研倒计时模块 ===\n\n");

  /* 初始化 */
  int ret = countdown_init();
  assert(ret == 0);
  printf("✅ countdown_init() 通过\n");

  /* 获取默认日期（2026-12-19） */
  int year, month, day;
  countdown_get_exam_date(&year, &month, &day);
  assert(year == 2026);
  assert(month == 12);
  assert(day == 19);
  printf("✅ 默认考试日期: %d-%02d-%02d 正确\n", year, month, day);

  /* 计算剩余天数 */
  int days = countdown_get_days_remaining();
  assert(days >= 0);
  printf("✅ 距离考研还有 %d 天\n", days);

  /* 精确倒计时 */
  int h, m, s;
  countdown_get_time_remaining(&days, &h, &m, &s);
  assert(days >= 0 && h >= 0 && h < 24);
  assert(m >= 0 && m < 60 && s >= 0 && s < 60);
  printf("✅ 精确倒计时: %d天 %02d:%02d:%02d\n", days, h, m, s);

  /* 自定义考试日期 */
  countdown_set_exam_date(2026, 12, 20);
  countdown_get_exam_date(&year, &month, &day);
  assert(year == 2026 && month == 12 && day == 20);
  printf("✅ 设置考试日期为 2026-12-20 成功\n");

  /* 恢复默认 */
  countdown_set_exam_date(2026, 12, 19);

  printf("\n🎉 倒计时模块测试全部通过！\n");
  return 0;
}