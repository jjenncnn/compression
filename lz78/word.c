#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "code.h"
#include "word.h"

Word *word_create(uint8_t *syms, uint32_t len) {
    Word *w = (Word *) calloc(1, sizeof(Word));
    w->syms = (uint8_t *) calloc(len, sizeof(uint8_t));
    w->len = len;
    if (syms != NULL) {
        memcpy(w->syms, syms, len);
    }
    return w;
}

Word *word_append_sym(Word *w, uint8_t sym) {
    Word *new = (Word *) calloc(1, sizeof(Word));
    new->len = (w->len) + 1;
    new->syms = (uint8_t *) calloc(new->len, sizeof(uint8_t));
    memcpy(new->syms, w->syms, w->len);
    new->syms[(new->len) - 1] = sym;
    return new;
}

void word_delete(Word *w) {
    free(w->syms);
    free(w);
}

WordTable *wt_create(void) {
    WordTable *table = (WordTable *) calloc(MAX_CODE, sizeof(WordTable));
    table[EMPTY_CODE] = word_create(NULL, 0);
    return table;
}

void wt_reset(WordTable *wt) {
    for (int i = 2; i < MAX_CODE; i++) {
        word_delete(wt[i]);
        wt[i] = NULL;
    }
}

void wt_delete(WordTable *wt) {
    for (int i = 0; i < MAX_CODE; i++) {
        if (wt[i]) {
            word_delete(wt[i]);
            wt[i] = NULL;
        }
    }
    free(wt);
}
