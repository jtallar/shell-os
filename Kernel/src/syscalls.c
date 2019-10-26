// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <process.h>
#include <stdint.h>
#include <lib.h>
#include <keyboard.h>
#include <timelib.h>
#include <console.h>
#include <naiveConsole.h>
#include <sound.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <interrupts.h>
#include <fileDescriptors.h>

#include <syscalls.h>

extern void hang(); // Ubicada en loader.asm

void read_handler(uint64_t fd, char * buff, uint64_t count) {
    read(fd, buff, count);
}

void write_handler(uint64_t fd, const char * buff, uint64_t count) {
    write(fd, buff, count);
}

uint64_t time_handler() {
    return ticks_elapsed();
}

void clear_handler() {
    clear_console();
	// ncClear();
}

uint64_t rtc_handler(uint8_t reg){
    write_port(0x70, reg);
    uint64_t aux = read_port(0x71);
    return (aux >> 4) * 10 + (aux & 0x0F);
}

void sleep_handler(uint64_t millis){
    sleep(millis);
}

void beep_handler(uint16_t frequency, uint64_t state) {
    switch (state)
    {
        case 1: // PLAY
            play_sound(frequency);
            break;
        case 0: // STOP
            no_sound();
            break;
        default:
            break;
    }
}

void exit_handler() {
    hang();
}

void pixel_handler(uint64_t x, uint64_t y, uint64_t rgb) {
    Vector2 auxPos = {x, y};
    Color auxColor = {(rgb & 0xFF0000) >> 16, (rgb & 0x00FF00) >> 8, rgb & 0x0000FF};
    draw_pixel(auxPos, auxColor);
}

/* ---------------------------- */

void * malloc_handler(uint64_t bytes) {
    return malloc(bytes);
}

void free_handler(void * ptr) {
    free(ptr);
}

void printStatus_handler() {
    printStatus();
}

uint64_t create_handler(void * entryPoint, char * name, level context) {
    return create(entryPoint, name, context, 0, 0);
}

uint64_t kill_handler(uint64_t pid) {
    return kill(pid);
}

uint64_t getPid_handler() {
    return getPid();
}

void listAllProcess_handler() {
    listAll();
}

uint64_t setPriority_handler(uint64_t pid, uint8_t prio) {
    return setPriority(pid, prio);
}

uint64_t changeState_handler(uint64_t pid) {
    states state = getState(pid);
	switch (state) {
		case READY:
			return setState(pid, BLOCKED);
		case BLOCKED:
			// DEBERIA HACER ALGO MAS ACA (POR EG VER QUE PUEDA DESBLOQUEARSE)
			return setState(pid, READY);
		default: // RUNNING o UNDEFINED
			return 1;
	}
}

void halt_handler() {
    _hlt();
}