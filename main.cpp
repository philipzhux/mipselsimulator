#define MAIN_ENT
#include "project.h"
#include "assembler.h"
#include "simulator.h"

//main function
int main(int argc,const char* argv[]) {
    if(argc!=4){
        cout<<"Invalid Argument Syntax";
        return 0;
    }
    fstream _asm(argv[1]);
    fstream _in(argv[2]);
    fstream _out(argv[3],ios::out);
    map<string,uint32_t> label_record; unsigned char* realm; unsigned char* pc; uint32_t* registers; vector<mip_t> mips_struct_set;
    //label_record to store mapping between label and address; realm is actually "base"

    /* pointer to the top of the heap (for .data section in simulator) */
    unsigned char* sc;

    /* for variables in .data section in simulator
     * usage: label_record.insert(pair<string, int>("label", int_address)) */
    vector<var_t> vars;
    
    /* assembler part */
    assembler(label_record,mips_struct_set,vars,_asm);
    /* if not using the dis-assembler, please put machine code directly to mips_struct_set[i].code 
    and their address to mips_struct_set[i].addr */
    /* also the variables defined in assembly is parsed in assembler part, if splitting
    it into two projects do remember to take that part out as well */
    simu_init(realm,pc,sc,mips_struct_set,registers,vars); //simu_init for simulator
    while(true){
        uint32_t ins = (((*(pc+3))<<24) | ((*(pc+2))<<16) | ((*(pc+1))<<8) | (*(pc)));
        if(ins==0)
            /* exit */
            break;
        pc+=4;
        simulate(ins,registers,pc,realm,sc,_in,_out);
    }
}

unsigned char* get_real_mem(uint32_t faked,unsigned char*  realm){
    unsigned char* res;
    res=realm+faked-0x400000;
    return res;
}

uint32_t get_faked(unsigned char* real,  unsigned char* realm){
    return uint32_t(real-realm)+0x400000;
}


//machine code output: not required in the instruction, just for the use of debug
void output_mc(vector<mip_t> mips_struct_set){
    for(size_t i=0;i<mips_struct_set.size();i++){
        cout<<mips_struct_set[i].code<<endl;
    }
}

