/*
 * io_posix.c
 *
 *  Created on: 20.03.2014
 *      Author: Erich Wenger
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
#include "io_posix.h"

FILE *default_output_stream = NULL;
FILE *default_input_stream = NULL;

/**
 * Initializes the POSIX-compatible IO and hereby sets
 * the default input and output streams.
 * @param default_in the input stream to be set to default
 * @param default_out the output stream to be set to default
 */
void io_posix_init(FILE *default_in, FILE *default_out) {
	default_input_stream = default_in;
	default_output_stream = default_out;
}

/**
 * Reads a byte from the default input stream.
 * @return the byte read
 */
uint8_t io_posix_read_byte() {
	return fgetc(default_input_stream);
}

/**
 * Writes a byte / character to the default output stream.
 * @param byte the byte / character that shall be written
 */
void io_posix_write_byte(uint8_t byte) {
	fputc(byte, default_output_stream);
}
