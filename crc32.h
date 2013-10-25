/*
 * Taken from PostgreSQL source code.
 * contrib/hstore/crc32.h
 */
#ifndef _CRC32_H
#define _CRC32_H

#include <stdint.h>

/* Returns crc32 of data block */
extern unsigned int crc32_sz(uint8_t *buf, int size);

/* Returns crc32 of null-terminated string */
#define crc32(buf) crc32_sz((buf),strlen(buf))

#endif
