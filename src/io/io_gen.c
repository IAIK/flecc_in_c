/*
 * io_gen.c
 *
 *  Created on: Mar 19, 2014
 *      Author: Thomas Unterluggauer
 */

/*
 * Copyright (C) 2014 Stiftung Secure Information and
 *                    Communication Technologies SIC
 * http://www.sic.st
 *
 * All rights reserved.
 *
 * This source is provided for inspection purposes and recompilation only,
 * unless specified differently in a contract with IAIK. This source has to
 * be kept in strict confidence and must not be disclosed to any third party
 * under any circumstances. Redistribution in source and binary forms, with
 * or without modification, are <not> permitted in any case!
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */ 
 
#include <stdio.h>
#include "../bi/bi.h"
#include "io.h"

/** Hex lookup for converting hexadecimal nibbles
 *  to string representation. */
const char hex_lookup[] = {'0', '1', '2', '3',
					 '4', '5', '6', '7',
					 '8', '9', 'a', 'b',
					 'c', 'd', 'e', 'f'};

/**
 * Prints an unsigned integer to a string.
 * @param buffer the buffer that is written to
 * @param u	the unsigned integer
 * @return a pointer to the end of the buffer
 */
char *print_uint_to_buffer(char *buffer, const uint_t u) {

#if (BYTES_PER_WORD > 2)
    *buffer++ = hex_lookup[ (u >> 28) & 0xF ];
    *buffer++ = hex_lookup[ (u >> 24) & 0xF];
    *buffer++ = hex_lookup[ (u >> 20) & 0xF];
    *buffer++ = hex_lookup[ (u >> 16) & 0xF];
#endif

#if (BYTES_PER_WORD > 1)
	*buffer++ = hex_lookup[(u >> 12) & 0xF ];
	*buffer++ = hex_lookup[(u >> 8) & 0xF];
#endif

	*buffer++ = hex_lookup[(u >> 4) & 0xF];
	*buffer++ = hex_lookup[(u & 0xF)];

	return buffer;
}

/**
 * Writes a character string of specified length to the default output.
 * @param buffer the character string to be printed
 * @param length the length of the character string
 */
void io_gen_write(const char *buffer, const int length) {
	int i;
	for (i = 0; i < length; i++) {
		io_write_byte(buffer[i]);
	}
}

/**
 * Reads a character string with a maximum size of
 * length from the default input.
 * @param buffer the buffer to be filled with the character string being read
 * @param length the size of the buffer
 * @return the number of characters actually read
 */
int io_gen_read(char *buffer, const int length) {
	int i = 0;
	uint8_t byte;

	do {
		byte = io_read_byte();
		buffer[i++] = byte;
	} while (byte != '\n' && i < length);

	return i;
}

/**
 * Prints a NULL-terminated character string to the default output.
 * @param buffer the character string to be printed
 */
void io_print(const char *buffer) {
	while (*buffer) {
		io_write_byte(*buffer++);
	}
}

/**
 * Prints a NULL-terminated character string to the default
 * output and appends a newline.
 * @param buffer the character string to be printed
 */
void io_println(const char *buffer) {
	io_print(buffer);
//	io_print("\r\n");
	io_print("\n");
}

/**
 * Prints an array of bytes of arbitrary length to
 * a string buffer.
 * @param buffer the string buffer that is written to
 * @param value the value to be printed
 * @param length the length of the array
 */
void io_sprint_bytes_var(char *buffer, const uint8_t *value, const int length) {
	int i;
	for (i = length-1; i >= 0; i--) {
		*buffer++ = hex_lookup[(value[i] >> 4) & 0xF];
		*buffer++ = hex_lookup[(value[i] & 0xF)];
	}
	*buffer = '\0';
}

/**
 * Prints a big integer to the default output and appends a newline.
 * @param value the big integer to be printed
 * @param length the length of the big integer to be printed
 */
void io_print_bigint_var(const uint_t *value, const int length) {
	char buffer[BYTES_PER_GFP*4+WORDS_PER_GFP*2+10];
	bigint_print_var(buffer, value, length);
	io_println(buffer);
}

/**
 * Prints a byte array to the default output and appends a newline.
 * @param value the byte array to be printed
 * @param length the length of the byte array to be printed
 */
void io_print_bytes_var(const uint8_t *value, const int length) {
	char buffer[BYTES_PER_GFP*2+WORDS_PER_GFP+1];	// TODO: correct size
	io_sprint_bytes_var(buffer, value, length);
	io_println(buffer);
}

/**
 * Prints an integer to the default output and appends a newline.
 * @param value the integer to be printed
 */
void io_print_integer(const uint_t value) {
	char buffer[BYTES_PER_WORD*2+1];
	print_uint_to_buffer(buffer, value);
	buffer[2*BYTES_PER_WORD] = 0;
	io_println(buffer);
}
