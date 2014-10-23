/*
 * parse.c
 *
 *  Created on: Mar 20, 2014
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
 
#include "parse.h"

/**
 * Parses the integer that is contained in buffer.
 * @param buffer the buffer containing an integer in string format
 * @return the integer
 */
int parse_integer(const char *buffer) {
    int negative = 0;
    int number = 0;

    if (*buffer == '-') {
        negative = 1;
        buffer++;
    }

    while ((*buffer >= '0') && (*buffer <= '9')) {
    	number *= 10;
    	number += *buffer - '0';
    	buffer++;
    }

    if (negative) {
        number *= -1;
    }

    return number;
}

/**
 * Gets a byte from hexadecimal string representation.
 * @param buffer buffer[0] is MSB, buffer[1] is LSB of the byte to be read
 * @return the byte
 */
uint8_t parse_hex_byte(const char *buffer) {
    uint8_t byte = 0;

    if ((buffer[0] >= '0') && (buffer[0] <= '9')) {
        byte |= (buffer[0] - '0') << 4;
    } else if ((buffer[0] >= 'A') && (buffer[0] <= 'F')) {
        byte |= (buffer[0] - 'A' + 10) << 4;
    } else if ((buffer[0] >= 'a') && (buffer[0] <= 'f')) {
        byte |= (buffer[0] - 'a' + 10) << 4;
    }
    if ((buffer[1] >= '0') && (buffer[1] <= '9')) {
        byte |= buffer[1] - '0';
    } else if ((buffer[1] >= 'A') && (buffer[1] <= 'F')) {
        byte |= buffer[1] - 'A' + 10;
    } else if ((buffer[1] >= 'a') && (buffer[1] <= 'f')) {
        byte |= buffer[1] - 'a' + 10;
    }

    return byte;
}

/**
 * Parses a hexadecimal message in the buffer and stores
 * it to the array message. Note that the array message
 * needs to be of appropriate size.
 * @param message the parsed message
 * @param max_message_length the size of the allocated message buffer
 * @param buffer the buffer containing the hexadecimal message
 * @param buffer_length the number of characters stored in buffer
 * @return the length of the message
 */
int parse_hex_message(char *message, const int max_message_length, const char *buffer, const int buffer_length) {
    int num_words = 0;
    int _buffer_length_ = buffer_length;
    uint8_t byte;

    while (_buffer_length_ > 1) {
        byte = parse_hex_byte(buffer);
        *message = byte;
        message++;
        num_words++;
        buffer += 2;
        _buffer_length_ -= 2;
        if (num_words == max_message_length)
        	break;
    }

    return num_words;
}
