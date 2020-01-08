/*
cdecode.h - c header for a base64 decoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#ifndef BASE64_CDECODE_H
#define BASE64_CDECODE_H

#include <stdint.h>
#include <zconf.h>

#if defined(__cplusplus)
extern "C"
{
#endif

typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

void base64_init_decodestate(base64_decodestate* state_in);

ptrdiff_t base64_decode_block(const char* code_in, int length_in, uint8_t *plaintext_out, base64_decodestate* state_in);

#if defined(__cplusplus)
}
#endif

#endif /* BASE64_CDECODE_H */
