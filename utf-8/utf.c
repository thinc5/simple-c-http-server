#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "./trie/trie.h"

/**
 * Initialize the UTF-8 trie.
 */
Trie *prep_utf8_trie(void)
{
    Trie *root = trie_init();
    trie_insert(root, "20", ' ');
    trie_insert(root, "21", '!');
    trie_insert(root, "22", '"');
    trie_insert(root, "23", '#');
    trie_insert(root, "24", '$');
    trie_insert(root, "25", '%');
    trie_insert(root, "26", '&');
    trie_insert(root, "27", '\'');
    trie_insert(root, "28", '(');
    trie_insert(root, "29", ')');
    trie_insert(root, "2A", '*');
    trie_insert(root, "2B", '+');
    trie_insert(root, "2C", ',');
    trie_insert(root, "2D", '-');
    trie_insert(root, "2E", '.');
    trie_insert(root, "2F", '/');
    trie_insert(root, "30", '0');
    trie_insert(root, "31", '1');
    trie_insert(root, "32", '2');
    trie_insert(root, "33", '3');
    trie_insert(root, "34", '4');
    trie_insert(root, "35", '5');
    trie_insert(root, "36", '6');
    trie_insert(root, "37", '7');
    trie_insert(root, "38", '8');
    trie_insert(root, "39", '9');
    trie_insert(root, "3A", ':');
    trie_insert(root, "3B", ';');
    trie_insert(root, "3C", '<');
    trie_insert(root, "3D", '=');
    trie_insert(root, "3E", '>');
    trie_insert(root, "3F", '?');
    trie_insert(root, "40", '@');
    trie_insert(root, "41", 'A');
    trie_insert(root, "42", 'B');
    trie_insert(root, "43", 'C');
    trie_insert(root, "44", 'D');
    trie_insert(root, "45", 'E');
    trie_insert(root, "46", 'F');
    trie_insert(root, "47", 'G');
    trie_insert(root, "48", 'H');
    trie_insert(root, "49", 'I');
    trie_insert(root, "4A", 'J');
    trie_insert(root, "4B", 'K');
    trie_insert(root, "4C", 'L');
    trie_insert(root, "4D", 'M');
    trie_insert(root, "4E", 'N');
    trie_insert(root, "4F", 'O');
    trie_insert(root, "50", 'P');
    trie_insert(root, "51", 'Q');
    trie_insert(root, "52", 'R');
    trie_insert(root, "53", 'S');
    trie_insert(root, "54", 'T');
    trie_insert(root, "55", 'U');
    trie_insert(root, "56", 'V');
    trie_insert(root, "57", 'W');
    trie_insert(root, "58", 'X');
    trie_insert(root, "59", 'Y');
    trie_insert(root, "5A", 'Z');
    trie_insert(root, "5B", '[');
    trie_insert(root, "5C", '\\');
    trie_insert(root, "5D", ']');
    trie_insert(root, "5E", '^');
    trie_insert(root, "5F", '_');
    trie_insert(root, "60", '`');
    trie_insert(root, "61", 'a');
    trie_insert(root, "62", 'b');
    trie_insert(root, "63", 'c');
    trie_insert(root, "64", 'd');
    trie_insert(root, "65", 'e');
    trie_insert(root, "66", 'f');
    trie_insert(root, "67", 'g');
    trie_insert(root, "68", 'h');
    trie_insert(root, "69", 'i');
    trie_insert(root, "6A", 'j');
    trie_insert(root, "6B", 'k');
    trie_insert(root, "6C", 'l');
    trie_insert(root, "6D", 'm');
    trie_insert(root, "6E", 'n');
    trie_insert(root, "6F", 'o');
    trie_insert(root, "70", 'p');
    trie_insert(root, "71", 'q');
    trie_insert(root, "72", 'r');
    trie_insert(root, "73", 's');
    trie_insert(root, "74", 't');
    trie_insert(root, "75", 'u');
    trie_insert(root, "76", 'v');
    trie_insert(root, "77", 'w');
    trie_insert(root, "78", 'x');
    trie_insert(root, "79", 'y');
    trie_insert(root, "7A", 'z');
    trie_insert(root, "7B", '{');
    trie_insert(root, "7C", '|');
    trie_insert(root, "7D", '}');
    trie_insert(root, "7E", '~');
    trie_insert(root, "7F", ' ');
    return root;
}

/**
 * Function that takes a source string and a buffer to write to and decodes
 * utf-8 to standard ASCII.
 * 
 * Assumes that the destination buffer is zeroed.
 */
size_t convert_from_utf8(const char *src, char *dest)
{
    Trie *root = prep_utf8_trie();
    int step = 0;
    size_t src_length = strlen(src);
    // Amount of data copied from src in to dest so far.
    size_t dest_point = 0;
    size_t src_base = 0;

    // Scan the string and look for %.
    size_t point = 0;
    while (point < src_length)
    {
        if (src[point] == '%')
        {
            step = 0;
            // Attempt to decode if we fail we just continue.
            char decoded = trie_find(root, src + point + 1, &step);
            if (decoded == -1)
            {
                printf("Failed to decode %.3s\n", src + point);
                continue;
            }

            // Copy what we have passed so far into dest.
            memmove(dest + dest_point, src + src_base,
                    point - src_base);

            // Update our tracking variables.
            dest_point += point - src_base;
            // Insert the decoded character.
            dest[dest_point] = decoded;
            dest_point++;

            // printf("Decoded char %c from str %.3s %d\n", decoded, src + point, step);
            // dest[dest_location] = decoded;
            //printf("DEST: %d: %.10s\n", point, dest + dest_location - 3);
            // Test.
            point += step;
            src_base = point;
        }
        else
        {
            point++;
        }
    }
    trie_free(root);
    return strlen(dest);
}
