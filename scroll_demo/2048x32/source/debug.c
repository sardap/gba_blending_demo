#include "debug.h"

void init_debug() {
	*REG_DEBUG_ENABLE = 0xC0DE;
}

void write_to_log(log_level_e level, const char* ptr,...) {
	va_list args;
	va_start(args, ptr);
	vsnprintf(REG_DEBUG_STRING, 0x100, ptr, args);
	va_end(args);
	*REG_DEBUG_FLAGS = (u16)level | 0x100;
}