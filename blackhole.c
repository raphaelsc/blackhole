/*
 * Blackhole: A steganography program.
 * Copyright (C) 2013 Raphael S. Carvalho <raphael.scarv@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include "crc32.h"

#define BLACKHOLE_FILE_EXTENSION 	".bh"
#define BLACKHOLE_MAPFILE_EXTENSION 	".map"
#define BLACKHOLE_OUTPUT_EXTENSION 	".out"

#define PGSIZE 4096
#define PGSIZE_NIBBLES (PGSIZE * 2)

#define DEBUG

static uint8_t blackhole_map0[4][4] = {
	[0] {0x00, 0x01, 0x02, 0x03},
	[1] {0x04, 0x05, 0x06, 0x07},
	[2] {0x08, 0x09, 0x0a, 0x0b},
	[3] {0x0c, 0x0d, 0x0e, 0x0f},
};
static uint8_t blackhole_map1[4][4] = {
	[0] {0x10, 0x11, 0x12, 0x13},
	[1] {0x14, 0x15, 0x16, 0x17},
	[2] {0x18, 0x19, 0x1a, 0x1b},
	[3] {0x1c, 0x1d, 0x1e, 0x1f},
};
static uint8_t blackhole_map2[4][4] = {
	[0] {0x20, 0x21, 0x22, 0x23},
	[1] {0x24, 0x25, 0x26, 0x27},
	[2] {0x28, 0x29, 0x2a, 0x2b},
	[3] {0x2c, 0x2d, 0x2e, 0x2f},
};
static uint8_t blackhole_map3[4][4] = {
	[0] {0x30, 0x31, 0x32, 0x33},
	[1] {0x34, 0x35, 0x36, 0x37},
	[2] {0x38, 0x39, 0x3a, 0x3b},
	[3] {0x3c, 0x3d, 0x3e, 0x3f},
};
static uint8_t blackhole_map4[4][4] = {
	[0] {0x40, 0x41, 0x42, 0x43},
	[1] {0x44, 0x45, 0x46, 0x47},
	[2] {0x48, 0x49, 0x4a, 0x4b},
	[3] {0x4c, 0x4d, 0x4e, 0x4f},
};
static uint8_t blackhole_map5[4][4] = {
	[0] {0x50, 0x51, 0x52, 0x53},
	[1] {0x54, 0x55, 0x56, 0x57},
	[2] {0x58, 0x59, 0x5a, 0x5b},
	[3] {0x5c, 0x5d, 0x5e, 0x5f},
};
static uint8_t blackhole_map6[4][4] = {
	[0] {0x60, 0x61, 0x62, 0x63},
	[1] {0x64, 0x65, 0x66, 0x67},
	[2] {0x68, 0x69, 0x6a, 0x6b},
	[3] {0x6c, 0x6d, 0x6e, 0x6f},
};
static uint8_t blackhole_map7[4][4] = {
	[0] {0x70, 0x71, 0x72, 0x73},
	[1] {0x74, 0x75, 0x76, 0x77},
	[2] {0x78, 0x79, 0x7a, 0x7b},
	[3] {0x7c, 0x7d, 0x7e, 0x7f},
};
static uint8_t blackhole_map8[4][4] = {
	[0] {0x80, 0x81, 0x82, 0x83},
	[1] {0x84, 0x85, 0x86, 0x87},
	[2] {0x88, 0x89, 0x8a, 0x8b},
	[3] {0x8c, 0x8d, 0x8e, 0x8f},
};
static uint8_t blackhole_map9[4][4] = {
	[0] {0x90, 0x91, 0x92, 0x93},
	[1] {0x94, 0x95, 0x96, 0x97},
	[2] {0x98, 0x99, 0x9a, 0x9b},
	[3] {0x9c, 0x9d, 0x9e, 0x9f},
};
static uint8_t blackhole_map10[4][4] = {
	[0] {0xa0, 0xa1, 0xa2, 0xa3},
	[1] {0xa4, 0xa5, 0xa6, 0xa7},
	[2] {0xa8, 0xa9, 0xaa, 0xab},
	[3] {0xac, 0xad, 0xae, 0xaf},
};
static uint8_t blackhole_map11[4][4] = {
	[0] {0xb0, 0xb1, 0xb2, 0xb3},
	[1] {0xb4, 0xb5, 0xb6, 0xb7},
	[2] {0xb8, 0xb9, 0xba, 0xbb},
	[3] {0xbc, 0xbd, 0xbe, 0xbf},
};
static uint8_t blackhole_map12[4][4] = {
	[0] {0xc0, 0xc1, 0xc2, 0xc3},
	[1] {0xc4, 0xc5, 0xc6, 0xc7},
	[2] {0xc8, 0xc9, 0xca, 0xcb},
	[3] {0xcc, 0xcd, 0xce, 0xcf},
};
static uint8_t blackhole_map13[4][4] = {
	[0] {0xd0, 0xd1, 0xd2, 0xd3},
	[1] {0xd4, 0xd5, 0xd6, 0xd7},
	[2] {0xd8, 0xd9, 0xda, 0xdb},
	[3] {0xdc, 0xdd, 0xde, 0xdf},
};
static uint8_t blackhole_map14[4][4] = {
	[0] {0xe0, 0xe1, 0xe2, 0xe3},
	[1] {0xe4, 0xe5, 0xe6, 0xe7},
	[2] {0xe8, 0xe9, 0xea, 0xeb},
	[3] {0xec, 0xed, 0xee, 0xef},
};
static uint8_t blackhole_map15[4][4] = {
	[0] {0xf0, 0xf1, 0xf2, 0xf3},
	[1] {0xf4, 0xf5, 0xf6, 0xf7},
	[2] {0xf8, 0xf9, 0xfa, 0xfb},
	[3] {0xfc, 0xfd, 0xfe, 0xff},
};

static inline int create_file(const char *path)
{
	int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR);
	if (fd == -1)
		fprintf(stderr, "open: %s: %s\n", path, strerror(errno));

	return fd;
}

static inline int write_to(int fd, const uint8_t *buffer, size_t size)
{
	int ret = write(fd, buffer, size);
	if (ret == -1 || ret != size)
		return -1;

	return 0;
}

#define TABLE_IDX_MASK 0xF0
#define MAP_IDX_MASK 0x0F

/*
 * Create the obscure and the map files from the given file.
 */
static int obscure(int fd, int newfd, int mapfd, off_t st_size)
{
	uint32_t crc_32;
	uint8_t *addr;
	uint8_t byte, nibbles;
	uint8_t byte_i, table_i, map_i;
	uint8_t tablei_buffer[PGSIZE], mapi_buffer[PGSIZE];

	addr = mmap(NULL, st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("map");
		return -1;
	}

	/* Write file size to the beginning of the obscure file */
	if (write_to(newfd, (uint8_t *) &st_size, 4))
		return -1;

	/* Write crc_32 to the beginning of the map file */
	crc_32 = crc32_sz(addr, st_size);
	if (write_to(mapfd, (uint8_t *) &crc_32, 4))
		return -1;

	int i;
	for (i = nibbles = 0; i < st_size; i++) {
		byte = addr[i];
		table_i = (byte & TABLE_IDX_MASK) >> 4;
		map_i = byte & MAP_IDX_MASK;

		assert(nibbles <= PGSIZE_NIBBLES);
		if (nibbles == PGSIZE_NIBBLES) {
			if (write_to(newfd, tablei_buffer, PGSIZE))
				return -1;
			if (write_to(mapfd, mapi_buffer, PGSIZE))
				return -1;

			nibbles = 0;
		}

		byte_i = nibbles >> 1;
		tablei_buffer[byte_i] = (tablei_buffer[byte_i] << 4) | table_i;
		mapi_buffer[byte_i] = (mapi_buffer[byte_i] << 4) | map_i;
		nibbles++;
	}

	byte_i = nibbles >> 1;
	if (nibbles % 2 != 0) {
		tablei_buffer[byte_i] <<= 4;
		mapi_buffer[byte_i] <<= 4;
		byte_i++;
	}

	if (write_to(newfd, tablei_buffer, byte_i))
		return -1;
	if (write_to(mapfd, mapi_buffer, byte_i))
		return -1;

	munmap(addr, st_size);
	return 0;
}

static uint8_t (*blackhole_maps[16])[][4] = {
	&blackhole_map0,
	&blackhole_map1,
	&blackhole_map2,
	&blackhole_map3,
	&blackhole_map4,
	&blackhole_map5,
	&blackhole_map6,
	&blackhole_map7,
	&blackhole_map8,
	&blackhole_map9,
	&blackhole_map10,
	&blackhole_map11,
	&blackhole_map12,
	&blackhole_map13,
	&blackhole_map14,
	&blackhole_map15,
};

/*
 * table_i: least significant nibble is an index into blackhole_maps.
 * map_i structure: least significant nibble [row_i - 2 bits][col_i - 2 bits]
 */
static uint8_t reconstruct_data(uint8_t table_i, uint8_t map_i)
{
	uint8_t row_i, col_i;
	uint8_t (*blackhole_map)[][4];

	assert((table_i & 0xF0) == 0);
	assert((map_i & 0xF0) == 0);

	blackhole_map = blackhole_maps[table_i];
	row_i = map_i >> 2;
	col_i = map_i & 0x03;

	return (*blackhole_map)[row_i][col_i];
}

/*
 * Reveal info given the obscure and map files.
 */
static int reveal(int fd, int newfd, int mapfd, off_t st_size,
		  off_t map_st_size)
{
	uint32_t crc_32, file_size, bytes;
	uint8_t *addr, *map;
	uint8_t table_i, map_i;
	uint8_t buffer[PGSIZE];

	/* Both the obscure and the map file must have the same size */
	if (st_size != map_st_size) {
		fprintf(stderr, "Size of obscure and map files differ!\n");
		return -1;
	}

	addr = mmap(NULL, st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("map");
		return -1;
	}

	map = mmap(NULL, map_st_size, PROT_READ, MAP_SHARED, mapfd, 0);
	if (map == MAP_FAILED) {
		perror("map");
		return -1;
	}

	crc_32 = *((uint32_t *)map); /* Get crc_32 from map file */
	file_size = *((uint32_t *)addr); /* Get file size from obscure file */
	bytes = 0;

	/* - Get size of the obscure file;
	 * - subtract size of metadata;
	 * - and multiplies by 2 to get the number of bytes.
	 */
	if (((st_size - 4) * 2) - file_size > 1) {
		fprintf(stderr,
			"((size of obscure file - 4) * 2) "
			"shall not differ from file_size by more than 1.\n");
		return -1;
	}

	/* Start from 4 to skip the 32-bit metadata from both files */
	int i;
	for (i = 4; i < st_size; i++) {
		/* Reconstruct the first byte */
		table_i = (addr[i] & 0xF0) >> 4;
		map_i = (map[i] & 0xF0) >> 4;
		buffer[bytes++] = reconstruct_data(table_i, map_i);

		if (bytes == file_size)
			break;

		/* Reconstruct the second byte */
		table_i = (addr[i] & 0x0F);
		map_i = (map[i] & 0x0F);
		buffer[bytes++] = reconstruct_data(table_i, map_i);

		assert(bytes <= PGSIZE);
		if (bytes == PGSIZE) {
			if (write_to(newfd, buffer, PGSIZE))
				return -1;

			bytes = 0;
		}
	}

	/* Use crc32 to check file integrity */
	if (crc_32 != crc32_sz(buffer, bytes)) {
		fprintf(stderr, "Checksum doesn't match: crc_32: %08x\n",
			crc_32);
		return -1;
	}

#ifdef DEBUG
	printf("Remaining bytes: %u\n", bytes);
	for (i = 0; i < bytes; i++)
		putchar(buffer[i]);
#endif
	if (write_to(newfd, buffer, bytes))
		return -1;

	munmap(addr, st_size);
	munmap(map, map_st_size);
	return 0;
}

int main(int argc, char **argv)
{
	struct stat st, map_st;
	int fd, newfd, mapfd, ret;
	char newfile[256], mapfile[256];

	if (argc != 2) {
		fprintf(stderr, "usage: %s <file>\n", argv[0]);
		return -1;
	}

	fd = open(argv[1], O_RDONLY);
	if (!fd) {
		perror("open");
		return -1;
	}

	ret = stat(argv[1], &st);
	if (ret == -1) {
		perror("stat");
		return -1;
	}

	ret = lseek(fd, 0, SEEK_SET);
	if (ret == -1) {
		perror("lseek");
		return -1;
	}

	/* Obscure */
	if (!strstr(argv[1], BLACKHOLE_FILE_EXTENSION)) {
		snprintf(newfile, 256, "%s%s", argv[1],
			 BLACKHOLE_FILE_EXTENSION);
		snprintf(mapfile, 256, "%s%s", newfile,
			 BLACKHOLE_MAPFILE_EXTENSION);

		newfd = create_file(newfile);
		if (newfd == -1)
			return -1;
		printf("Generating obscure file: %s...\n", newfile);

		mapfd = create_file(mapfile);
		if (mapfd == -1)
			goto unlink_newfile;
		printf("Generating map file: %s...\n", mapfile);

		ret = obscure(fd, newfd, mapfd, st.st_size);
		if (ret == -1) {
			fprintf(stderr, "obscure failed!\n");
			goto unlink_mapfile;
		}
	/* Reveal */
	} else {
		snprintf(newfile, 256, "%s%s", argv[1],
			 BLACKHOLE_OUTPUT_EXTENSION);
		snprintf(mapfile, 256, "%s%s", argv[1],
			 BLACKHOLE_MAPFILE_EXTENSION);

		mapfd = open(mapfile, O_RDONLY);
		if (mapfd == -1) {
			fprintf(stderr, "open: %s: %s\n",
				mapfile, strerror(errno));
			return -1;
		}

		ret = stat(mapfile, &map_st);
		if (ret == -1) {
			perror("stat");
			return -1;
		}

		ret = lseek(mapfd, 0, SEEK_SET);
		if (ret == -1) {
			perror("lseek");
			return -1;
		}
			 
		newfd = create_file(newfile);
		if (newfd == -1)
			return -1;
		printf("Generating output file: %s...\n", newfile);

		ret = reveal(fd, newfd, mapfd, st.st_size, map_st.st_size);
		if (ret == -1) {
			fprintf(stderr, "reveal failed!\n");
			goto unlink_newfile;
		}
	}

	close(fd);
	close(newfd);
	close(mapfd);
	return 0;
unlink_mapfile:
	unlink(mapfile);
unlink_newfile:
	unlink(newfile);
	return -1;
}
