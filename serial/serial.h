#pragma once

void serial_initialize();
void serial_send(char* s);
void serial_receive(char* s, u32 size);
