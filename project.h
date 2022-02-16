#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <bitset>
#include <cstdint>
#include <algorithm>
#include <stdlib.h>
#include <cstddef>
#include <string.h>
#include <fstream>
#include <cctype>
#include <string.h>
#include <stdio.h>
#include <unistd.h> 
#include <fcntl.h> 

using namespace std;

//structs
typedef struct mip{
    vector<string> argv;
    string ins;
    uint32_t address;
    string code;
} mip_t;

typedef struct var{
    string name;
    string type;
    string content;
} var_t;

const uint32_t HID=33; const  uint32_t LOD=32;
const string DQUOTESYMBOL = "!^&*D1O2U3B4L5E213i7sadkfhl6*7@8Q9UOTE^^%*";
const string SQUOTESYMBOL = "12ki23hrlwbfjg21837r84276&2@#%Y";
const bool AS_TEST=false; //true for asssembler test
const bool MC_ENABLE=false; //true to output machine code
unsigned char* get_real_mem(uint32_t faked,unsigned char*  realm);
uint32_t get_faked(unsigned char* real,  unsigned char* realm);
void output_mc(vector<mip_t> mips_struct_set);

#ifdef MAIN_ENT
map<string,int> REGISTER_SET = {{"$0",0},{"$zero",0},{"$at",1},{"$v0",2},{"$v1",3},{"$a0",4},{"$a1",5},{"$a2",6},{"$a3",7},
{"$t0",8},{"$t1",9},{"$t2",10},{"$t3",11},{"$t4",12},{"$t5",13},{"$t6",14},{"$t7",15},{"$s0",16},{"$s1",17},{"$s2",18},{"$s3",19},
{"$s4",20},{"$s5",21},{"$s6",22},{"$s7",23},{"$t8",24},{"$t9",25},{"$k0",26},{"$k1",27},{"$gp",28},{"$sp",29},{"$fp",30},{"$ra",31}};

map<string,unsigned char> INSTRUCT_ENUM = {{"add",0},{"addu",1},{"addi",2},{"addiu",3},
{"and",4},{"andi",5},{"clo",6},{"clz",7},{"div",8},{"divu",9},{"mult",10},{"multu",11},
{"mul",12},{"madd",13},{"msub",14},{"maddu",15},{"msubu",16},{"nor",17},{"or",18},{"ori",19},
{"sll",20},{"sllv",21},{"sra",22},{"srav",23},{"srl",24},{"srlv",25},{"sub",26},{"subu",27},
{"xor",28},{"xori",29},{"lui",30},{"slt",31},{"sltu",32},{"slti",33},{"sltiu",34},{"beq",35},
{"bgez",36},{"bgezal",37},{"bgtz",38},{"blez",39},{"bltzal",40},{"bltz",41},{"bne",42},{"j",43},
{"jal",44},{"jalr",45},{"jr",46},{"teq",47},{"teqi",48},{"tne",49},{"tnei",50},{"tge",51},
{"tgeu",52},{"tgei",53},{"tgeiu",54},{"tlt",55},{"tltu",56},{"tlti",57},{"tltiu",58},{"lb",59},
{"lbu",60},{"lh",61},{"lhu",62},{"lw",63},{"lwc1",60},{"lwl",65},{"lwr",66},{"ll",67},
{"sb",68},{"sh",69},{"sw",70},{"swl",71},{"swr",72},{"sc",73},{"mfhi",74},{"mflo",75},{"mthi",76},{"mtlo",77},{"syscall",78},{"li",79},{"move",80}};

#else
extern map<string,int> REGISTER_SET;
extern map<string,unsigned char> INSTRUCT_ENUM;
#endif

#endif