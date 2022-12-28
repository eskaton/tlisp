#ifndef _HEX_DUMP_H_
#define _HEX_DUMP_H_

void hex_dump(void *buf, size_t len, long addr);
void hex_dump_to(FILE *dest, void *buf, size_t len, long addr);

#endif
