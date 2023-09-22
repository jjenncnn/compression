# Assignment 5: Lempel-Ziv Compression

## Short Description:
This prorgram implements LZ78 data compression and decompression with the files encode and decode, respectively. trie.c implements the Trie ADT used for creating tries, word.c implements the Word ADT for creating Words and WordTables, and io.c implements the I/O module for buffering the I/O.

## Files Included:
- randstate.c: Implements the random state interface for the SS library and number theory functions.

- encode.c: contains the main() function for the encode program.

- decode.c: contains the main() function for the decode program.

- trie.c: the source file for the Trie ADT.

- trie.h: the header file for the Trie ADT.

- word.c: the source file for the Word ADT.

- word.h: the header file for the Word ADT.

- io.c: the source file for the I/O module.

- io.h: the header file for the I/O module.

- endian.h: the header file for the endianess module.

- code.h: the header file containing macros for reserved codes.

- README.md: This file.

- Makefile: compiles the program.

- DESIGN.pdf: Documents the design process and includes psuedocode and explanation on how program structure works.

- WRITEUP.pdf: Overview of knowledge learned.

## Build:
To build all executables:
 - $ make (or) $ make all
 
To build encode:
 - $ make encode
 
To build decode:
 - $ make decode
 
 To run programs:
 - $ ./(program_name) -options

## Debugging:
Run 'make debug' to compile the program with debug flags. Add 'valgrind' to the front of the file execution command to run the file with valgrind to check for leaks and errors.

## Cleaning:
To clean the directory after building, type 'make clean' into the command line.
