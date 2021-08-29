/*
 * Copyright 2021 NSG650
 * Copyright 2021 Sebastian
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "serial.h"
#include "../cpu/ports.h"
#include "../klibc/lock.h"

DECLARE_LOCK(spinlock);
DECLARE_LOCK(spinlock2);

int serial_install_port(uint16_t PORT) {
	port_byte_out(PORT + 1, 0x00); // Disable all interrupts
	port_byte_out(PORT + 3, 0x80); // Enable DLAB (set baud rate divisor)
	port_byte_out(PORT + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
	port_byte_out(PORT + 1, 0x00); //                  (hi byte)
	port_byte_out(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
	port_byte_out(PORT + 2,
				  0xC7); // Enable FIFO, clear them, with 14-byte threshold
	port_byte_out(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
	port_byte_out(PORT + 4, 0x1E); // Set in loopback mode, test the serial chip
	port_byte_out(PORT + 0, 0xAE); // Test serial chip (send byte 0xAE and check
								   // if serial returns same byte)

	port_byte_in(PORT + 0);
	port_byte_out(PORT + 4, 0x0F);

	return 0;
}

void serial_install(void) {
	serial_install_port(COM1);
}

int is_transmit_empty(uint16_t PORT) {
	return port_byte_in(PORT + 5) & 0x20;
}

void write_serial(char *word) {
	LOCK(spinlock);
	asm volatile("cli");
	write_serial_port(COM1, word);
	UNLOCK(spinlock);
	asm volatile("sti");
}

void write_serial_port(uint16_t PORT, char *word) {
	while (is_transmit_empty(PORT) == 0)
		;

	while (*word != '\0') {
		write_serial_port_char(PORT, *word);
		word++;
	}
}

void write_serial_port_char(uint16_t PORT, char word) {
	while (is_transmit_empty(PORT) == 0)
		;

	if ((uint8_t)word == 0xFF) {
		write_serial("[Error in character]\n");
	}
	port_byte_out(PORT, word);
}

void write_serial_char(char word) {
	LOCK(spinlock2);
	write_serial_port_char(COM1, word);
	UNLOCK(spinlock2);
}

int serial_received(uint16_t PORT) {
	return port_byte_in(PORT + 5) & 1;
}

char read_serial(uint16_t PORT) {
	while (serial_received(PORT) == 0)
		;

	return port_byte_in(PORT);
}
