#ifndef SIMU_H
#define SIMU_H
#include "project.h"
void simu_init(unsigned char* &real_mem, unsigned char* &pc,unsigned char* &sc,  vector<mip_t> mips_struct_set,uint32_t* &registers,vector<var_t> &vars);
void simulate(uint32_t ins,uint32_t* registers,unsigned char* &pc, unsigned char* realm,unsigned char* &sc,fstream &_in, fstream &_out);
void op_ins(uint32_t op,uint32_t rs,uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t funct,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void j_ins(uint32_t op,uint32_t target,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void i_ins(uint32_t op, uint32_t rs, uint32_t rt, uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
#endif