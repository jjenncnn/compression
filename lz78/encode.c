#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "trie.h"
#include "word.h"
#include "io.h"
#include "code.h"

#define OPTIONS "vi:o:h"

int bit_counter(uint16_t n) {
    int counter = 0;
    while (n) {
        counter++;
        n >>= 1;
    }
    return counter;
}

int main(int argc, char **argv) {
    int opt = 0;
    bool info = false;
    int in = STDIN_FILENO;
    int out = STDOUT_FILENO;
    bool inf = false;
    bool outf = false;
    FileHeader header; // = calloc(1, sizeof(FileHeader));
    struct stat in_info;
    struct stat out_info;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': info = true; break;

        case 'i':
            inf = true;
            in = open(optarg, O_RDONLY);
            break;

        case 'o':
            outf = true;
            out = open(optarg, O_WRONLY | O_CREAT | O_TRUNC);
            fchmod(out, 0600);
            fstat(out, &out_info);
            header.magic = 0xBAADBAAC;
            header.protection = out_info.st_mode;
            break;

        case 'h':
            printf("SYNOPSIS\n");
            printf("   Compresses files using the LZ78 compression algorithm.\n");
            printf("   Compressed files are decompressed with the corresponding decoder.\n");
            printf("\nUSAGE\n   ./encode [-vh] [-i input] [-o output]\n");
            printf("\nOPTIONS\n");
            printf("   -v          Display compression statistics\n");
            printf("   -i input    Specify input to compress (stdin by default)\n");
            printf("   -o output   Specify output of compressed input (stdout by default)\n");
            printf("   -h          Display program help and usage\n");
            return 0;
        }
    }

    if (outf == false) {
        fstat(out, &out_info);
        header.magic = 0xBAADBAAC;
        header.protection = out_info.st_mode;
    }
    write_header(out, &header);
    //free(header);
    TrieNode *root = trie_create();
    TrieNode *curr_node;
    curr_node = root;
    TrieNode *prev_node;
    prev_node = NULL;
    uint8_t curr_sym = 0;
    uint8_t prev_sym = 0;
    uint16_t next_code = START_CODE;
    TrieNode *next_node;

    while (read_sym(in, &curr_sym) == true) {
        next_node = trie_step(curr_node, curr_sym);
        if (next_node != NULL) {
            prev_node = curr_node;
            curr_node = next_node;
        } else {
            write_pair(out, curr_node->code, curr_sym, bit_counter(next_code));
            curr_node->children[curr_sym] = trie_node_create(next_code);
            curr_node = root;
            next_code = next_code + 1;
        }
        if (next_code == MAX_CODE) {
            trie_reset(root);
            curr_node = root;
            next_code = START_CODE;
        }
        prev_sym = curr_sym;
    }
    if (curr_node != root) {
        write_pair(out, prev_node->code, prev_sym, bit_counter(next_code));
        next_code = (next_code + 1) % MAX_CODE;
    }
    write_pair(out, STOP_CODE, 0, bit_counter(next_code));
    flush_pairs(out);
    trie_delete(root);
    if (info == true) {
        if (outf == false) {
            double space1 = 1 - ((double) (total_bits / 8) / (double) (total_syms));
            fprintf(stderr,
                "Compressed file size:"
                "%" PRIu64 " bytes\n",
                total_bits / 8);
            fprintf(stderr,
                "Uncompressed file size:"
                "%" PRIu64 " bytes\n",
                total_syms);
            fprintf(stderr, "Space saving: %.2f%%\n", 100 * space1);
        } else {
            fstat(in, &in_info);
            fstat(out, &out_info);
            int com = out_info.st_size;
            int uncom = in_info.st_size;
            double space = 1 - ((double) com / (double) uncom);
            fprintf(stderr, "Compressed file size: %d bytes\n", com);
            fprintf(stderr, "Uncompressed file size: %d bytes\n", uncom);
            fprintf(stderr, "Space saving: %.2f%%\n", 100 * space);
        }
    }
    if (inf == true) {
        close(in);
    }
    if (outf == true) {
        close(out);
    }
    return 0;
}
