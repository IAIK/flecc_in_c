/*
 * io.h
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
 
#ifndef IO_H_
#define IO_H_

#include "../arch/arch.h"
#include "io_gen.h"

#include "../arch/posix/io/io_posix.h"

#define io_init(default_in, default_out)    io_posix_init(default_in, default_out)
#define io_read_byte()             			io_posix_read_byte()
#define io_write_byte(byte)        			io_posix_write_byte(byte)
#define io_read(buffer,len)   				io_gen_read(buffer,len)
#define io_write(buffer,len)  				io_gen_write(buffer,len)

#if (ARCHITECTURE == ARCH_ATMEGA_FOR_GCC)
/* automatically redefines io_init, io_read_byte, and io_write_byte*/
#include "../arch/avr_gcc/io/serial.h"
#endif


#endif /* IO_H_ */
