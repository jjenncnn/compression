#include "word.h"
#include "code.h"
#include "io.h"
#include "endian.h"
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t buffer[BLOCK];
static uint8_t buffer2[BLOCK];
uint64_t total_syms = 0;
uint64_t total_bits = 0;
uint64_t bit_idx = 0;
uint64_t sym_idx = 0;
uint64_t size = 0;

int read_bytes(int infile, uint8_t *buf, int to_read) {
    int bytes_read = 0;
    int val;
    while (bytes_read < to_read) {
        if ((val = read(infile, buf, to_read - bytes_read)) < 1) {
            return bytes_read;
        }
        bytes_read += val;
    }
    return bytes_read;
}

int write_bytes(int outfile, uint8_t *buf, int to_write) {
    int written = 0;
    int val;
    while (written < to_write) {
        if ((val = write(outfile, buf, to_write - written)) < 1) {
            return written;
        }
        written += val;
    }
    return written;
}

void read_header(int infile, FileHeader *header) {
    read_bytes(infile, (uint8_t *) header, sizeof(FileHeader));
    if (big_endian() == true) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
    if (header->magic != 0xBAADBAAC) {
        printf("Bad magic number.\n");
        exit(1);
    }
}

void write_header(int outfile, FileHeader *header) {
    write_bytes(outfile, (uint8_t *) header, sizeof(FileHeader));
    if (big_endian() == true) {
        header->magic = swap32(header->magic);
        header->protection = swap16(header->protection);
    }
}

bool read_sym(int infile, uint8_t *sym) {
    int bytes_read = 0;
    if (sym_idx == 0) {
        if ((bytes_read = read_bytes(infile, buffer, BLOCK)) < BLOCK) {
            size = bytes_read + 1;
        }
    }
    *sym = buffer[sym_idx];
    sym_idx++;
    sym_idx %= BLOCK;

    if (sym_idx != size) {
        total_syms++;
        return true;
    } else {
        return false;
    }
}

void write_pair(int outfile, uint16_t code, uint8_t sym, int bitlen) {
    for (int i = 0; i < bitlen; i++) {
        if (code & (1 << i)) {
            buffer2[bit_idx / 8] |= (1 << (bit_idx % 8));
        } else {
            buffer2[bit_idx / 8] &= ~(1 << (bit_idx % 8));
        }
        bit_idx++;
        total_bits++;
        if (bit_idx == 32768) {
            flush_pairs(outfile);
            bit_idx = 0;
        }
    }

    for (int j = 0; j < 8; j++) {
        if (sym & (1 << (j % 8))) {
            buffer2[bit_idx / 8] |= (1 << (bit_idx % 8));
        } else {
            buffer2[bit_idx / 8] &= ~(1 << (bit_idx % 8));
        }
        bit_idx++;
        total_bits++;
        if (bit_idx == 32768) {
            flush_pairs(outfile);
            bit_idx = 0;
        }
    }
}

void flush_pairs(int outfile) {
    if (bit_idx < BLOCK * 8) {
        write_bytes(outfile, buffer2, (bit_idx / 8));
        for (int i = 0; i < BLOCK; i++) {
            buffer2[i] = 0;
        }
    } else {
        write_bytes(outfile, buffer2, BLOCK);
        for (int i = 0; i < BLOCK; i++) {
            buffer2[i] = 0;
        }
    }
}

bool read_pair(int infile, uint16_t *code, uint8_t *sym, int bitlen) {
    *code = 0;
    *sym = 0;
    for (int i = 0; i < bitlen; i++) {
        read_bytes(infile, buffer2, BLOCK);
        if (buffer2[bit_idx / 8] & (1 << (bit_idx % 8))) {
            *code |= (1 << i);
        } else {
            *code &= ~(1 << i);
        }
        bit_idx++;
        total_bits++;
        bit_idx %= (32768);
    }

    for (int j = 0; j < 8; j++) {
        read_bytes(infile, buffer2, BLOCK);
        if (buffer2[bit_idx / 8] & (1 << (bit_idx % 8))) {
            *sym |= (1 << j);
        } else {
            *sym &= ~(1 << j);
        }
        bit_idx++;
        total_bits++;
        bit_idx %= (32768);
    }
    if (*code == STOP_CODE) {
        return false;
    }
    return true;
}

void write_word(int outfile, Word *w) {
    for (uint32_t i = 0; i < w->len; i++) {
        buffer[sym_idx] = w->syms[i];
        sym_idx++;
        total_syms++;
        if (sym_idx == BLOCK) {
            flush_words(outfile);
        }
    }
}

void flush_words(int outfile) {
    if (sym_idx < BLOCK) {
        write_bytes(outfile, buffer, sym_idx);
        for (int i = 0; i < BLOCK; i++) {
            buffer[i] = 0;
        }
        sym_idx = 0;
    } else {
        write_bytes(outfile, buffer, BLOCK);
        for (int i = 0; i < BLOCK; i++) {
            buffer[i] = 0;
        }
        sym_idx = 0;
    }
}
