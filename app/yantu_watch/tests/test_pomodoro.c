/****************************************************************************
 * apps/examples/yantu_watch/tests/test_pomodoro.c
 *
 * 番茄钟计时器模块单元测试
 ****************************************************************************/

#include <stdio.h>
#include <assert.h>
#include "../core/pomodoro_timer.h"

/* 测试回调计数器 */
static int g_callback_count = 0;
static pomodoro_state_t g_last_state = POMODORO_IDLE;

static void test_callback(pomodoro_state_t state, subject_t subject)
{
  g_callback_count++;
  g_last_state = state;
}

int main(void)
{
  printf("=== 测试番茄钟计时器模块 ===\n\n");

  /* 初始化 */
  pomodoro_init();
  assert(pomodoro_get_state() == POMODORO_IDLE);
  printf("✅ 初始化状态为 IDLE\n");

  /* 注册回调 */
  pomodoro_register_callback(test_callback);
  g_callback_count = 0;

  /* 开始番茄 */
  int ret = pomodoro_start(SUBJECT_ENGLISH);
  assert(ret == 0);
  assert(pomodoro_get_state() == POMODORO_FOCUS);
  assert(pomodoro_get_subject() == SUBJECT_ENGLISH);
  printf("✅ 开始英语学习番茄钟\n");

  /* 暂停 */
  ret = pomodoro_pause();
  assert(ret == 0);
  assert(pomodoro_get_state() == POMODORO_BREAK);
  printf("✅ 暂停番茄钟\n");

  /* 恢复 */
  ret = pomodoro_resume();
  assert(ret == 0);
  assert(pomodoro_get_state() == POMODORO_FOCUS);
  printf("✅ 恢复番茄钟\n");

  /* 停止 */
  ret = pomodoro_stop();
  assert(ret == 0);
  assert(pomodoro_get_state() == POMODORO_IDLE);
  printf("✅ 停止番茄钟\n");

  /* 设置时长 */
  pomodoro_set_duration(30, 10);
  int focus, break_t;
  pomodoro_get_duration(&focus, &break_t);
  assert(focus == 30 && break_t == 10);
  printf("✅ 设置时长: 专注30min, 休息10min\n");

  /* 跳过 */
  pomodoro_start(SUBJECT_MATH);
  ret = pomodoro_skip();
  assert(ret == 0);
  assert(pomodoro_get_state() == POMODORO_BREAK);
  printf("✅ 跳过专注阶段，进入休息\n");

  /* 科目名称 */
  const char *name = pomodoro_get_subject_name(SUBJECT_POLITICS);
  assert(name != NULL);
  printf("✅ 科目名称: %s\n", name);

  pomodoro_stop();

  printf("\n🎉 番茄钟模块测试全部通过！\n");
  return 0;
}