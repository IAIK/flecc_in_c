/*
 * io_gen.h
 *
 *  Created on: Mar 19, 2014
 *      Author: tunterluggauer
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
 
#ifndef IO_GEN_H_
#define IO_GEN_H_

#include <stdio.h>
#include "../types.h"

void io_gen_write(const char *buffer, const int length);
int  io_gen_read(char *buffer, const int length);

void io_print(const char *buffer);
void io_println(const char *buffer);

void io_print_integer(const uint_t value);
void io_print_bigint_var(const uint_t *value, const int length);

void io_sprint_bytes_var(char *buffer, const uint8_t *value, const int length);
void io_print_bytes_var(const uint8_t *value, const int length);

#endif /* IO_GEN_H_ */
