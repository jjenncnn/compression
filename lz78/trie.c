#include <stdint.h>
#include <stdlib.h>
#include "code.h"
#include "trie.h"

TrieNode *trie_node_create(uint16_t code) {
    TrieNode *tree = (TrieNode *) calloc(1, sizeof(TrieNode));
    tree->code = code;
    return tree;
}

void trie_node_delete(TrieNode *n) {
    free(n);
}

TrieNode *trie_create(void) {
    return trie_node_create(EMPTY_CODE);
}

void trie_reset(TrieNode *root) {
    for (int i = 0; i < ALPHABET; i++) {
        trie_delete(root->children[i]);
        root->children[i] = NULL;
    }
}

void trie_delete(TrieNode *n) {
    if (n) {
        for (int i = 0; i < ALPHABET; i++) {
            trie_delete(n->children[i]);
        }
        trie_node_delete(n);
        n = NULL;
    }
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    return n->children[sym];
}
