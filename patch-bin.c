//  Patch the vector table in Zephyr .bin firmware file.
//  Copy offset 0x200..0x2d7
//  To offset 0x20..0xf7
//  To patch zephyr.bin to zephyr2.bin:
//    gcc -o patch-bin patch-bin.c
//    ./patch-bin zephyr.bin zephyr2.bin
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define MAX_SIZE 512 * 1024  //  Max 512 KB

static uint8_t bytes[MAX_SIZE];

int main(int argc, char *argv[]) {
    if (argc != 3) {
        puts("patch-bin srcfile destfile");
        return 1;
    }
    char *src = argv[1];
    char *dest = argv[2];
    printf("Patching %s to %s...\n", src, dest);

    //  Read the source file
    FILE *f = fopen(src, "rb");
    assert(f != NULL);
    size_t bytes_read = fread(bytes, 1, sizeof(bytes), f);
    fclose(f);

    //  Patch in memory
    memcpy(
        &bytes[0x20],  //  Copy to offset 0x20...
        &bytes[0x200], //  Copy from offset 0x200...
        0xd8           //  For 0xd8 bytes
    );
    
    //  Write the dest file
    f = fopen(dest, "wb");
    assert(f != NULL);
    size_t bytes_written = fwrite(bytes, 1, bytes_read, f);
    fclose(f);

    assert(bytes_read == bytes_written);
    printf("*** Done! Patched %d bytes from %s to %s\n", (int) bytes_read, src, dest);
    return 0;
}