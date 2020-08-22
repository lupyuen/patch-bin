//  Patch the vector table in Zephyr .bin firmware file.
//  Copy offset 0x200..0x2d7
//  To offset 0x0..0xd7

//  To patch zephyr.bin to zephyr-patched.bin:
//    gcc -o patch-bin patch-bin.c
//    ./patch-bin zephyr.bin zephyr-patched.bin zephyr-vector.bin
//  The vector table is written to zephyr-vector.bin

//  To create MCUBoot firmware image zephyr-img.bin from the patched file zephyr-patched.bin:
//    mcuboot/scripts/imgtool.py create --pad-header --align 4 --version 1.0.0 --header-size 32 --slot-size 475136 zephyr-patched.bin zephyr-img.bin
//    mcuboot/scripts/imgtool.py verify zephyr-img.bin
//  We include the "--pad-header" option because the file does not contain an empty MCUBoot header.
//  See https://lupyuen.github.io/pinetime-rust-mynewt/articles/dfu#generate-a-firmware-image-file-for-pinetime

//  To generate the DFU package zephyr-dfu.zip from the MCUBoot firmware image zephyr-img.bin:
//    adafruit-nrfutil dfu genpkg --dev-type 0x0052 --application zephyr-img.bin zephyr-dfu.zip
//  See https://lupyuen.github.io/pinetime-rust-mynewt/articles/cloud#create-dfu-package

//  To test with pinetime-updater:
//  Flash zephyr-vector.bin at address 0x0
//  Flash zephyr-img.bin at address 0x8000

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define MAX_SIZE          512 * 1024  //  Max 512 KB
#define VECTOR_TABLE_SIZE 0xd8        //  Size of vector table

static uint8_t bytes[MAX_SIZE];

int main(int argc, char *argv[]) {
    if (argc != 4) {
        puts("patch-bin zephyr.bin zephyr-patched.bin zephyr-vector.bin");
        return 1;
    }
    char *src = argv[1];
    char *dest = argv[2];
    char *vector = argv[3];
    printf("Patching %s to %s...\n", src, dest);

    //  Read the source file
    FILE *f = fopen(src, "rb");
    assert(f != NULL);
    size_t bytes_read = fread(bytes, 1, sizeof(bytes), f);
    fclose(f);

    //  Patch in memory
    memcpy(
        &bytes[0x0],       //  Copy to offset 0x0...
        &bytes[0x200],     //  From offset 0x200...
        VECTOR_TABLE_SIZE  //  For 0xd8 bytes
    );
    
    //  Write the dest file
    f = fopen(dest, "wb");
    assert(f != NULL);
    size_t bytes_written = fwrite(bytes, 1, bytes_read, f);
    fclose(f);
    assert(bytes_read == bytes_written);

    //  Write the vector file
    f = fopen(vector, "wb");
    assert(f != NULL);
    bytes_written = fwrite(bytes, 1, VECTOR_TABLE_SIZE, f);
    fclose(f);
    assert(bytes_written == VECTOR_TABLE_SIZE);

    printf("*** Done! Patched %d bytes from %s to %s\n", (int) bytes_read, src, dest);
    printf("Vector table written to %s\n", vector);
    return 0;
}
