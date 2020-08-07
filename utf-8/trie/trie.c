#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "trie.h"

/**
 * DUMB HEX TO DEC.
 */
static int hex_to_dec(char hex)
{
    if ('0' <= hex && hex <= '9')
        return hex - '0';
    if ('A' <= hex && hex <= 'F')
        return hex - 'A' + 10;
    return -1;
}

#ifdef DEBUG
/**
 * DUMB DEC TO HEX.
 */
static char dec_to_hex(int dec)
{
    if (0 <= dec && dec <= 9)
        return (char)dec + '0';
    if (9 < dec && dec < 16)
        return (char)(dec - 10) + 'A';
    return -1;
}

static void test_hex()
{
    for (int i = 0; i < 16; i++)
        printf("dec to hex: %d, %c\n", i, dec_to_hex(i));

    char hex[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    for (int i = 0; i < 16; i++)
        printf("hex to dec: %c, %d\n", hex_to_dec(hex[i]), hex[i]);
}
#endif

/**
 * Initialize a trie node.
 */
Trie *trie_init(void)
{
    Trie *root = (Trie *)malloc(sizeof(Trie));
    root->is_leaf = false;
    root->value = '\0';
    for (int i = 0; i < MAX_CHARACTERS; i++)
        root->characters[i] = NULL;

    return root;
}

/**
 * Free a trie node and its children.
 */
void trie_free(Trie *root)
{
    for (int i = 0; i < MAX_CHARACTERS; i++)
    {
        if (root->characters[i] != NULL)
        {
            trie_free(root->characters[i]);
        }
    }
    free(root);
}

/**
 * Insert a string and value into a trie recursively.
 */
void trie_insert(Trie *root, const char *str, char value)
{
    // Do we already have a character with this character?
    int current = hex_to_dec(*str);

    // Do we have invalid hex?
    if (current == -1)
        return;

    // Can we continue down the path?
    if (root->characters[current])
    {
        trie_insert(root->characters[current], ++str, value);
        return;
    }
    // We need to init the new node.
    root->characters[current] = trie_init();

    // Are do we have more to store?
    if (strlen(str) > 1)
    {
        trie_insert(root->characters[current], ++str, value);
        return;
    }

    // We are the leaf no more insertions.
    root->characters[current]->is_leaf = true;
    root->characters[current]->value = value;
    return;
}

/**
 * Find a string within the trie.
 */
char trie_find(Trie *root, const char *str, int *dist)
{
    *dist += 1;
    int dec = hex_to_dec(*str);

    // Have we finished?
    if (root->is_leaf)
        return root->value;

    // Get the next Trie to search.
    Trie *next = root->characters[dec];

    // Continue the search.
    if (next)
        return trie_find(next, ++str, dist);

    // Cannot search further, return box character to symbolize unsupported.
    return -1;
}

#ifdef DEBUG
void debug_trie(Trie *root, int depth, char value)
{
    char spacer[50];
    memset(spacer, '\0', 50);
    memset(spacer, '-', depth);
    char strval[] = {value};
    printf("%s> %s -> '%c' (%lu)\n", spacer, value ? "�" : strval, root->value, root);
    for (int i = 0; i < MAX_CHARACTERS; i++)
        if (root->characters[i] != NULL)
            debug_trie(root->characters[i], depth + 1, dec_to_hex(i));
}
#endif
