/*
 * arch.h
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

#ifndef ARCH_H_
#define ARCH_H_

/** This is a list of possible architectures, on which ECC_IN_C could work.
 *  Avoid the value 0 (zero).
 */
#define ARCH_X86 1
#define ARCH_DSPIC30 2
#define ARCH_80C196 3
#define ARCH_ATMEGA 4
#define ARCH_ATMEGA_FOR_CROSSWORKS 4
#define ARCH_ATMEGA_FOR_GCC 5
#define ARCH_NEPTUN 6
#define ARCH_MSP430 7
#define ARCH_MSP430_FOR_IAR 7
#define ARCH_MSP430_FOR_GCC 8
#define ARCH_ARM 9
#define ARCH_ARM_CORTEX_A9 9
#define ARCH_ARM_CORTEX_M0_ARM_NONE_EABI_GCC 10

/** This actually defines the used architecture. */
#ifndef ARCHITECTURE
#define ARCHITECTURE ARCH_ARM_CORTEX_M0_ARM_NONE_EABI_GCC
#endif

#ifdef __i386__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_X86
#endif
#ifdef __i486__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_X86
#endif
#ifdef __i586__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_X86
#endif
#ifdef __i686__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_X86
#endif
#ifdef __amd64__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_X86
#endif
#ifdef __x86_64__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_X86
#endif

#ifdef __MSP430F1611__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_MSP430
#endif

#ifdef __CROSSWORKS
#ifdef __TARGET_PROCESSOR
#if __TARGET_PROCESSOR == ATmega128
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_ATMEGA_FOR_CROSSWORKS
#endif
#endif
#endif

#ifdef __GNUC__
#ifdef __AVR_ATmega128__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_ATMEGA_FOR_GCC
#endif

#ifdef __MSP430__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_MSP430_FOR_GCC
#endif

#ifdef __ARM_ARCH_6M__
#undef ARCHITECTURE
#define ARCHITECTURE ARCH_ARM_CORTEX_M0_ARM_NONE_EABI_GCC
#endif
#endif

#endif /* ARCH_H_ */
