#ifndef DASM_H
#define DASM_H
#include "project.h"
void process(string ins,vector<string> argv,string &code,uint32_t &address,string &label, map<string,uint32_t> &record);
void process(string ins,vector<string> argv,string &code,uint32_t address,map<string,uint32_t> &record);
void assembler(map<string,uint32_t> &label_record,vector<mip_t> &mips_struct_set,vector<var_t> &vars, fstream &_in);
#endif