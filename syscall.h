#ifndef SYSCALL_H
#define SYSCALL_H
#include "project.h"
void sys_call(uint32_t* registers,unsigned char* realm,unsigned char* &sc,fstream &_in, fstream &_out);
#endif