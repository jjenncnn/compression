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
    int in = 0;
    int out = 1;
    bool inf = false;
    bool outf = false;
    FileHeader header;
    struct stat in_info;
    struct stat out_info;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'v': info = true; break;

        case 'i':
            inf = true;
            in = open(optarg, O_RDONLY);
            fstat(in, &in_info);
            header.magic = 0xBAADBAAC;
            header.protection = in_info.st_mode;
            break;

        case 'o':
            outf = true;
            out = open(optarg, O_WRONLY | O_CREAT | O_TRUNC);
            fchmod(out, 0600);
            break;

        case 'h':
            printf("SYNOPSIS\n");
            printf("   Decompresses files with the LZ78 decompression algorithm.\n");
            printf("   Used with files compressed with the corresponding encoder.\n");
            printf("\nUSAGE\n   ./decode [-vh] [-i input] [-o output]\n");
            printf("\nOPTIONS\n");
            printf("   -v          Display decompression statistics\n");
            printf("   -i input    Specify input to decompress (stdin by default)\n");
            printf("   -o output   Specify output of decompressed input (stdout by default)\n");
            printf("   -h          Display program help and usage\n");
            return 0;
        }
    }
    if (inf == false) {
        fstat(in, &in_info);
        header.magic = 0xBAADBAAC;
        header.protection = in_info.st_mode;
    }
    read_header(in, &header);

    WordTable *table = wt_create();
    uint8_t curr_sym = 0;
    uint16_t curr_code = 0;
    uint16_t next_code = START_CODE;
    while (read_pair(in, &curr_code, &curr_sym, bit_counter(next_code)) == true) {
        table[next_code] = word_append_sym(table[curr_code], curr_sym);
        write_word(out, table[next_code]);
        next_code = next_code + 1;
        if (next_code == MAX_CODE) {
            wt_reset(table);
            next_code = START_CODE;
        }
    }
    flush_words(out);
    wt_delete(table);
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
            size_t com = in_info.st_size;
            size_t uncom = out_info.st_size;
            double space = 1 - ((double) com / (double) uncom);
            fprintf(stderr, "Compressed file size: %zu bytes\n", com);
            fprintf(stderr, "Uncompressed file size: %zu bytes\n", uncom);
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
