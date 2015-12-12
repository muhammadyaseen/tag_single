#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_

#include "instance.h"

#define KEY_LEN 	4

void random_select(uint8* , uint8*);
uint32 encrypt_rc4(uint8* , uint32 );

static const uint8 random_set[64] =
{
   0xD1, 0x98, 0x2F, 0xD0,
   0xB8, 0x6A, 0xBA, 0xF1,
   0x24, 0xB3, 0x08, 0x85,
   0x63, 0x71, 0xA4, 0xF4,
   0x0D, 0x72, 0x71, 0x82,
   0x7B, 0xC2, 0x9C, 0x2A,
   0xC5, 0x28, 0xCA, 0xB8,
   0x8E, 0x60, 0x6C, 0xB0,
   0xD7, 0xBD, 0x78, 0x55,
   0xE6, 0xAA, 0x57, 0x63,
   0x55, 0x2A, 0xB4, 0x11,
   0xA1, 0x7C, 0xB3, 0x63,
   0x2B, 0x74, 0xCE, 0x9B,
   0xAF, 0x6C, 0x7A, 0x28,
   0x3B, 0x6B, 0xC4, 0x66,
   0x61, 0xFB, 0x48, 0x5D
};

#endif
