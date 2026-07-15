#ifndef __YANTU_CORE_VOCABULARY_ENGINE_H
#define __YANTU_CORE_VOCABULARY_ENGINE_H

#include <stdbool.h>

typedef struct {
    char word[64];
    char phonetic[64];
    char meaning[256];
    char example[512];
    int  mark; /* 0=unmarked, 1=mastered, 2=review, 3=favorite */
} word_t;

int  vocab_init(void);
int  vocab_load_list(const char *filename);
int  vocab_get_count(void);
int  vocab_get_current_index(void);
int  vocab_get_word(int index, word_t *word);
int  vocab_set_mark(int index, int mark);
int  vocab_get_mark(int index);
int  vocab_next(void);
int  vocab_prev(void);
int  vocab_get_mastered_count(void);
int  vocab_get_review_count(void);

#endif /* __YANTU_CORE_VOCABULARY_ENGINE_H */