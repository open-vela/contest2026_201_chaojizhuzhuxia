/****************************************************************************
 * apps/examples/yantu_watch/core/vocabulary_engine.c
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "vocabulary_engine.h"
#include "storage.h"
#include "json_parser.h"

#define MAX_WORDS 4096

static word_t g_words[MAX_WORDS];
static int g_word_count = 0;
static int g_current_index = 0;
static char g_current_list[64] = "";

int vocab_init(void)
{
  g_word_count = 0;
  g_current_index = 0;
  return 0;
}

int vocab_load_list(const char *filename)
{
  char path[256];
  snprintf(path, sizeof(path), "vocab/%s", filename);

  size_t len = 1024 * 512; /* 512KB max */
  char *buf = malloc(len);
  if (!buf) return -1;

  if (storage_read(path, buf, &len) != 0)
    {
      free(buf);
      return -1;
    }

  cJSON *root = cJSON_Parse(buf);
  free(buf);
  if (!root) return -1;

  cJSON *words = cJSON_GetObjectItem(root, "words");
  if (!words || !cJSON_IsArray(words))
    {
      cJSON_Delete(root);
      return -1;
    }

  g_word_count = cJSON_GetArraySize(words);
  if (g_word_count > MAX_WORDS) g_word_count = MAX_WORDS;

  for (int i = 0; i < g_word_count; i++)
    {
      cJSON *item = cJSON_GetArrayItem(words, i);
      if (!item) continue;

      const char *w = json_get_string(item, "word", "");
      const char *p = json_get_string(item, "phonetic", "");
      const char *m = json_get_string(item, "meaning", "");
      const char *e = json_get_string(item, "example", "");

      strncpy(g_words[i].word, w, sizeof(g_words[i].word) - 1);
      strncpy(g_words[i].phonetic, p, sizeof(g_words[i].phonetic) - 1);
      strncpy(g_words[i].meaning, m, sizeof(g_words[i].meaning) - 1);
      strncpy(g_words[i].example, e, sizeof(g_words[i].example) - 1);
      g_words[i].mark = 0;
    }

  strncpy(g_current_list, filename, sizeof(g_current_list) - 1);
  g_current_index = 0;
  cJSON_Delete(root);
  return 0;
}

int vocab_get_count(void)
{
  return g_word_count;
}

int vocab_get_current_index(void)
{
  return g_current_index;
}

int vocab_get_word(int index, word_t *word)
{
  if (index < 0 || index >= g_word_count) return -1;
  *word = g_words[index];
  return 0;
}

int vocab_set_mark(int index, int mark)
{
  if (index < 0 || index >= g_word_count) return -1;
  g_words[index].mark = mark;
  return 0;
}

int vocab_get_mark(int index)
{
  if (index < 0 || index >= g_word_count) return -1;
  return g_words[index].mark;
}

int vocab_next(void)
{
  if (g_current_index < g_word_count - 1)
    {
      g_current_index++;
      return 0;
    }
  return -1;
}

int vocab_prev(void)
{
  if (g_current_index > 0)
    {
      g_current_index--;
      return 0;
    }
  return -1;
}

int vocab_get_mastered_count(void)
{
  int count = 0;
  for (int i = 0; i < g_word_count; i++)
    {
      if (g_words[i].mark == 1) count++;
    }
  return count;
}

int vocab_get_review_count(void)
{
  int count = 0;
  for (int i = 0; i < g_word_count; i++)
    {
      if (g_words[i].mark == 2) count++;
    }
  return count;
}