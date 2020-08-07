#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define MAX_CHARACTERS 16 // Possible hex

typedef struct Trie
{
    bool is_leaf;
    char value;
    struct Trie *characters[MAX_CHARACTERS];
} Trie;

Trie *trie_init(void);

void trie_free(Trie *root);

void trie_insert(Trie *root, const char *str, char value);

char trie_find(Trie *root, const char *str, int *dist);

#ifdef DEBUG

void debug_trie(Trie *root, int depth, char value);

#endif

#endif
