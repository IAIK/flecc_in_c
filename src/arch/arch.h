/****************************************************************************
**
** Copyright (C) 2014 Stiftung Secure Information and 
**                    Communication Technologies SIC and
**                    Graz University of Technology
** Contact: http://opensource.iaik.tugraz.at
**
** This file is part of <product_name>.
**
** $BEGIN_LICENSE:DEFAULT$
** Commercial License Usage
** Licensees holding valid commercial licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and SIC. For further information
** contact us at http://opensource.iaik.tugraz.at.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** This software is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this software. If not, see http://www.gnu.org/licenses/.
**
** $END_LICENSE:DEFAULT$
**
****************************************************************************/

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
