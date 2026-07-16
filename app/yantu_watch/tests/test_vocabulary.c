/****************************************************************************
 * apps/examples/yantu_watch/tests/test_vocabulary.c
 *
 * 单词引擎模块单元测试
 ****************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../core/vocabulary_engine.h"

int main(void)
{
  printf("=== 测试单词背诵引擎模块 ===\n\n");

  /* 初始化 */
  vocab_init();
  assert(vocab_get_count() == 0);
  printf("✅ 初始化成功\n");

  /* 加载词库 */
  int ret = vocab_load_list("kaoyan.json");
  if (ret != 0)
    {
      /* 在模拟器中可能没有词库文件 */
      printf("⚠️ 词库文件无法加载（模拟器环境），跳过后续测试\n");
      return 0;
    }

  int count = vocab_get_count();
  assert(count > 0);
  printf("✅ 加载词库成功，共 %d 个单词\n", count);

  /* 获取第一个单词 */
  word_t word;
  ret = vocab_get_word(0, &word);
  assert(ret == 0);
  assert(strlen(word.word) > 0);
  printf("✅ 第一个单词: %s [%s]\n", word.word, word.phonetic);

  /* 导航 */
  assert(vocab_get_current_index() == 0);
  vocab_next();
  assert(vocab_get_current_index() == 1);
  vocab_prev();
  assert(vocab_get_current_index() == 0);
  printf("✅ 导航: 下一个 → 上一个 正常工作\n");

  /* 标记 */
  vocab_set_mark(0, 1); /* 标记为已掌握 */
  assert(vocab_get_mark(0) == 1);
  assert(vocab_get_mastered_count() >= 1);
  printf("✅ 标记已掌握成功\n");

  vocab_set_mark(1, 2); /* 标记为需复习 */
  assert(vocab_get_mark(1) == 2);
  assert(vocab_get_review_count() >= 1);
  printf("✅ 标记需复习成功\n");

  printf("\n🎉 单词引擎测试全部通过！\n");
  return 0;
}