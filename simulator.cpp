//simulating part
#include "instr.h"
#include "simulator.h"
#include "utils.h"
#include "syscall.h"

void simu_init(unsigned char* &real_mem, unsigned char* &pc,unsigned char* &sc,  vector<mip_t> mips_struct_set,uint32_t* &registers,vector<var_t> &vars){
    /**
     * the stimulated memory block [begins with/is denoted by] real_mem
     * Access with faked address: real_mem[FAKED-0x400000] or *(get_real_mem(FAKED))
     * START OF TEXT SEGMENT : real_mem[0]
     * START OF STATIC DATA : real_mem[0x100000]
     * START OF STACK/END OF MEM BLOCK : real_mem[0x600000-1]
    **/

    //simu_init memory block
    real_mem = new unsigned char[0x600000+1];
    for(size_t i=0;i<0x600000;i++) real_mem[i]=0;
    //store machine code into text segment
    for(size_t i=0;i<mips_struct_set.size();i++){
        uint32_t n = uint32_t(stol((mips_struct_set[i]).code,nullptr,2));
        *get_real_mem(mips_struct_set[i].address+3,real_mem) = (unsigned char)((n >> 24) & 0xFF);
        *get_real_mem(mips_struct_set[i].address+2,real_mem) = (unsigned char)((n >> 16) & 0xFF);
        *get_real_mem(mips_struct_set[i].address+1,real_mem) = (unsigned char)((n >> 8) & 0xFF);
        *get_real_mem(mips_struct_set[i].address+0,real_mem) = (unsigned char)(n & 0xFF);
    }

    //simu_init registers
    registers = new uint32_t[34]; //32registers + 2 simulated hi and lo
    for(size_t i=0;i<34;i++) registers[i]=0;
    registers[REGISTER_SET["$zero"]]=0; registers[REGISTER_SET["$sp"]]=0xA00000;
    registers[REGISTER_SET["$gp"]]=0x500000;

    //set the program counter to point to the start of code
    pc = real_mem;
    sc = real_mem+0x100000;
    unsigned char* prev;

    for(size_t i=0;i<vars.size();i++){
        vars[i].name.erase(remove_if(vars[i].name.begin(), vars[i].name.end(), ::isspace), vars[i].name.end()); //remove ws
        vars[i].type.erase(remove_if(vars[i].type.begin(), vars[i].type.end(), ::isspace), vars[i].type.end()); //remove ws
        if(vars[i].type=="asciiz"){ 
            string processed = vars[i].content;
            string slash = "\\";
            string d_quote = "\"";
            string s_quote = "\'";
            replaceAll(processed,DQUOTESYMBOL,"\"");
            replaceAll(processed,SQUOTESYMBOL,"\'");
            replaceAll(processed,"\\n","\n");
            replaceAll(processed,"\\a","\a");
            replaceAll(processed,"\\t","\t");
            replaceAll(processed,"\\v","\v");
            replaceAll(processed,"\\b","\b");
            replaceAll(processed,"\\r","\r");
            replaceAll(processed,"\\f","\f");
            replaceAll(processed,"\\\\","\\");
            replaceAll(processed,"\\0","\0");
            replaceAll(processed,slash+s_quote,s_quote);
            replaceAll(processed,slash+d_quote,d_quote);
            char * cstr = new char [processed.length()+1];
            strcpy (cstr, processed.c_str());
            for(size_t i=0;i<processed.length()+1;i++) *sc++=cstr[i];
            //cout<<"Complete bytes of "<<4-(sc-prev)%4<<endl;
            delete[] cstr;
        }
        if(vars[i].type=="ascii"){
            string processed = vars[i].content;
            string slash = "\\";
            string d_quote = "\"";
            string s_quote = "\'";
            replaceAll(processed,DQUOTESYMBOL,"\"");
            replaceAll(processed,SQUOTESYMBOL,"\'");
            replaceAll(processed,"\\n","\n");
            replaceAll(processed,"\\a","\a");
            replaceAll(processed,"\\t","\t");
            replaceAll(processed,"\\v","\v");
            replaceAll(processed,"\\b","\b");
            replaceAll(processed,"\\r","\r");
            replaceAll(processed,"\\f","\f");
            replaceAll(processed,"\\\\","\\");
            replaceAll(processed,"\\0","\0");
            replaceAll(processed,slash+s_quote,s_quote);
            replaceAll(processed,slash+d_quote,d_quote);
            char * cstr = new char [processed.length()+1];
            strcpy (cstr, processed.c_str());
            for(size_t i=0;i<processed.length();i++) *sc++=cstr[i];
            delete[] cstr;
        }
        if(vars[i].type=="word"){
            vars[i].content.erase(remove_if( vars[i].content.begin(),  vars[i].content.end(), ::isspace),  vars[i].content.end()); //remove ws
            char* ctemp = new char[vars[i].content.length()+1];
            strcpy (ctemp, vars[i].content.c_str());
            char* token = strtok(ctemp, ",");
            while( token != NULL ) {
                uint32_t proc = strtol(token,nullptr,0);
                *sc++ = (unsigned char)(proc & 0xFF);
                *sc++ = (unsigned char)((proc >> 8) & 0xFF);
                *sc++ = (unsigned char)((proc >> 16) & 0xFF);
                *sc++ = (unsigned char)((proc >> 24) & 0xFF);
                //cout<<token<<endl;
                token = strtok(NULL,  ",");
            }
            delete[] ctemp;
        }

        if(vars[i].type=="half"){
            vars[i].content.erase(remove_if( vars[i].content.begin(),  vars[i].content.end(), ::isspace),  vars[i].content.end()); //remove ws
            char* ctemp = new char[vars[i].content.length()+1];
            strcpy (ctemp, vars[i].content.c_str());
            char* token = strtok(ctemp, ",");
            while( token != NULL ) {
                uint32_t proc = strtol(token,nullptr,0);
                *sc++ = (unsigned char)(proc & 0xFF);
                *sc++ = (unsigned char)((proc >> 8) & 0xFF);
                //cout<<token<<endl;
                token = strtok(NULL,  ",");
            }
            delete[] ctemp;
        }

        if(vars[i].type=="byte"){
            vars[i].content.erase(remove_if( vars[i].content.begin(),  vars[i].content.end(), ::isspace),  vars[i].content.end()); //remove ws
            char* ctemp = new char[vars[i].content.length()+1];
            strcpy (ctemp, vars[i].content.c_str());
            char* token = strtok(ctemp, ",");
            while( token != NULL ) {
                uint32_t proc = strtol(token,nullptr,0);
                *sc++ = (unsigned char)(proc & 0xFF);
                //cout<<token<<endl;
                token = strtok(NULL,  ",");
            }
            delete[] ctemp;
        }
        sc=sc+(4-get_faked(sc,real_mem)%4)%4; //align by word


    }

}

void simulate(uint32_t ins,uint32_t* registers,unsigned char* &pc, unsigned char* realm,unsigned char* &sc,fstream &_in, fstream &_out){
    uint32_t op = (ins>>26) & 0x3f; // first six bits
    //cout<<"operate op:"<<ins<<endl;
    if(ins==12){
        sys_call(registers,realm,sc,_in,_out);
        return;
    }
    if(op==0||op==0x1c){
        uint32_t rs = (ins>>21) & 0x1f;
        uint32_t rt = (ins>>16) & 0x1f;
        uint32_t rd = (ins>>11) & 0x1f;
        uint32_t shamt = (ins>>6) & 0x1f;
        uint32_t funct = ins & 0x3f;
        op_ins(op,rs,rt,rd,shamt,funct,registers,pc,realm);
    }
    else if(op==2||op==3){
        uint32_t target = (ins&0x3ffffff);
        j_ins(op,target,registers,pc,realm);
    }
    else{
        uint32_t rs = (ins>>21) & 0x1f;
        uint32_t rt = (ins>>16) & 0x1f;
        uint32_t imm = ins & 0xffff;
        i_ins(op,rs,rt,imm,registers,pc,realm);
    }
}


void op_ins(uint32_t op,uint32_t rs,uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t funct,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(op==0x1c){
       switch (funct)
       {
       case 0x21:
       {
           clo(rd,rs,registers,pc,realm);
           break;
       }
       case 0x20:
       {
           clz(rd,rs,registers,pc,realm);
           break;
       }
       case 2:
       {
           mul(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0:
       {
           madd(rs,rt,registers,pc,realm);
           break;
       }
       case 1:
       {
           madd(rs,rt,registers,pc,realm);
           break;
       }
       case 4:{
           msub(rs,rt,registers,pc,realm);
           break;
       }
       case 5:
       {
           msubu(rs,rt,registers,pc,realm);
           break;
       }
       default:
           break;
       }

    }
    else{
       switch (funct)
       {
       case 0x20:
       {
           add(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x21:
       {
           addu(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x24:
       {
           oand(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x1a:
       {
           odiv(rs,rt,registers,pc,realm);
           break;
       }
       case 0x1b:
       {
           odivu(rs,rt,registers,pc,realm);
           break;
       }
       case 0x18:
       {
           mult(rs,rt,registers,pc,realm);
           break;
       }
       case 0x19:
       {
           multu(rs,rt,registers,pc,realm);
           break;
       }
       case 0x27:
       {
           nor(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x25:
       {
           oor(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 4:
       {
           sllv(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 7:
       {
           srav(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 6:
       {
           srlv(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x22:
       {
           sub(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x23:
       {
           subu(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x26:
       {
           oxor(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x2a:
       {
           slt(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 0x2b:
       {
           sltu(rs,rt,rd,registers,pc,realm);
           break;
       }
       case 9:
       {
           jalr(rs,rd,registers,pc,realm);
           break;
       }
       case 8:
       {
           jr(rs,registers,pc,realm);
           break;
       }
       case 0x34:
       {
           teq(rs,rt,registers,pc,realm);
           break;
       }
       case 0x36:
       {
           tne(rs,rt,registers,pc,realm);
           break;
       }
       case 0x32:
       {
           tlt(rs,rt,registers,pc,realm);
           break;
       }
       case 0x33:
       {
           tltu(rs,rt,registers,pc,realm);
           break;
       }
       case 0x30:
       {
           tge(rs,rt,registers,pc,realm);
           break;
       }
       case 0x31:
       {
           tgeu(rs,rt,registers,pc,realm);
       }
       case 0x10:
       {
           mfhi(rd,registers,pc,realm);
           break;
       }
       case 0x12:
       {
           mflo(rd,registers,pc,realm);
           break;
       }
       case 0x11:
       {
           mthi(rs,registers,pc,realm);
           break;
       }
       case 0x13:
       {
           mtlo(rs,registers,pc,realm);
           break;
       }
       case 0:
       {
           sll(rt,rd,shamt,registers,pc,realm);
           break;
       }
       case 3:
       {
           sra(rt,rd,shamt,registers,pc,realm);
           break;
       }
       case 2:
       {
           srl(rt,rd,shamt,registers,pc,realm);
           break;
       }
       default:
           break;
       }
    }

}

void j_ins(uint32_t op,uint32_t target,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(op==2) j(target,registers,pc,realm);
    else jal(target,registers,pc,realm);
    return;
}

void i_ins(uint32_t op, uint32_t rs, uint32_t rt, uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    switch (op)
    {
    case 8:
    {
        addi(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 9:
    {
        addiu(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 0xc:
    {
        andi(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 0xd:
    {
        ori(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 0xe:
    {
        xori(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 0xf:
    {
        lui(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 0xa:
    {
        slti(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 0xb:
    {
        sltiu(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 4:
    {
        beq(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 1:
    {

        switch (rt)
        {
        case 1:
        {
            bgez(rs,rt,imm,registers,pc,realm);
            break;
        }
        case 0x11:
        {
            bgezal(rs,rt,imm,registers,pc,realm);
            break;
        }
        case 0x10:
        {
            bltzal(rs,rt,imm,registers,pc,realm);
            break;
        }
        case 0:
        {
            bltz(rs,rt,imm,registers,pc,realm);
            break;
        }
        case 0xc:
        {
            teqi(rs,imm,registers,pc,realm);
            break;
        }
        case 0xe:
        {
            tnei(rs,imm,registers,pc,realm);
            break;
        }
        case 8:
        {
            tgei(rs,imm,registers,pc,realm);
            break;
        }
        case 9:
        {
             tgeiu(rs,imm,registers,pc,realm);
            break;
        }
        case 0xa:
        {
            tlti(rs,imm,registers,pc,realm);
            break;
        }
        case 0xb:
        {
            tltiu(rs,imm,registers,pc,realm);
            break;
        }
        default: break;
        }
        break;
    }
    case 7:
    {
        bgtz(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 6:
    {
        blez(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 5:
    {
        bne(rs,rt,imm,registers,pc,realm);
        break;
    }
    case 0x20:
        lb(rs,rt,imm,registers,pc,realm);
        break;
    case 0x24:
        lbu(rs,rt,imm,registers,pc,realm);
        break;
    case 0x21:
        lh(rs,rt,imm,registers,pc,realm);
        break;
    case 0x25:
        lhu(rs,rt,imm,registers,pc,realm);
        break;
    case 0x23:
        lw(rs,rt,imm,registers,pc,realm);
        break;
    case 0x22:
        lwl(rs,rt,imm,registers,pc,realm);
        break;
    case 0x26:
        lwr(rs,rt,imm,registers,pc,realm);
        break;
    case 0x30:
        ll(rs,rt,imm,registers,pc,realm);
        break;
    case 0x28:
        sb(rs,rt,imm,registers,pc,realm);
        break;
    case 0x29:
        sh(rs,rt,imm,registers,pc,realm);
        break;
    case 0x2b:
        sw(rs,rt,imm,registers,pc,realm);
        break;
    case 0x2a:
        swl(rs,rt,imm,registers,pc,realm);
        break;
    case 0x2e:
        swr(rs,rt,imm,registers,pc,realm);
        break;
    case 0x38:
        sc(rs,rt,imm,registers,pc,realm);
        break;
    default:
        break;
    }
}



/**
 * r type instructions:
 * shamt == 0:
 * 0x20 add rd,rs,rt
 * 0x21 addu rd,rs,rt
 * 0x24 and rd, rs, rt
 * (0x1c,0x21) clo rd,rs
 * (0x1c,0x20) clz rd,rs
 * 0x1a div rs,rt
 * 0x1b divu rs, rt
 * 0x18 mult rs, rt
 * 0x19 multu rs, rt
 * (0x1c,2) mul rd,rs,rt
 * (0x1c,0) madd rs, rt
 * (0x1c,1) maddu rs, rt
 * (0x1c,4) msub rs, rt
 * (0x1c,5) msubu rs, rt
 * 0x27 nor rd, rs, rt
 * 0x25 or rd, rs, rt
 * (shamt,0) sll rd, rt, shamt
 * 4 sllv rd, rt, rs
 * (shamt,3) sra rd, rt, shamt
 * 7 srav rd, rt, rs
 * (shamt,2) srl,rd,rt,shamt
 * 6 srlv rd, rt, rs
 * 0x22 sub rd, rs, rt
 * 0x23 subu rd, rs, rt
 * 0x26 xor rd, rs, rt
 * 0x2a slt rd, rs, rt
 * 0x2b sltu rd, rs, rt
 * 9 jalr rs,rd
 * 8 jr rs
 * 0x34 teq rs, rt
 * 0x36 tne rs,rt
 * 0x32 tlt rs,rt
 * 0x33 tltu rs, rt
 * 0x10 mfhi rd
 * 0x12 mflo fd
 * 0x11 mthi rs
 * 0x13 mtlo rs
 * 0xb movn rd,rs,rt
 * 0xa movz zd,rs,rt
 **/

/**
 * i type instruction:
 * 8 addi rt,rs,imm
 * 9 addiu rt,rs,imm
 * 0xc addi rt,rs,imm
 * 0xd ori rt,rs,imm
 * 0xe xori rs,rt,imm
 * 0xf lui 0,rt,imm
 * 0xa slti rs,rt,imm
 * 0xb sltiu rs,rt,imm
 * 4 beq rs,rt,label
 * 1(rt=1) bgez rs,label
 * 1(rt=0x11) bgezal rs, label
 * 7 bgtz rs,label
 * 6 blez rs,label
 * 1(rt=0x10) bltzal rs,label
 * 1(rt=0) bltz rs,label
 * 5 bne rs, rt, label
 * 1(rt=0xc) teqi rs,imm
 * 1(rt=0xe) teni rs,imm
 * 1(rt=8) tgei rs,imm
 * 1(rt=9) tgeiu rs,imm
 * 1(rt=a) tlti rs,imm
 * 1(rt=b) tltiu rs,imm
 * 0x20 lb rs,rt,offset
 * 0x24 lbu rs,rt,offset
 * 0x21 lh rs,rt,offset
 * 0x25 lhu rs,rt,offset
 * 0x23 lw rs,rt,offset
 * 0x22 lwl rs,rt,offset
 * 0x26 lwr rs,rt,offset
 * 0x30 ll rs,rt,offset
 * 0x28 sb rs,rt,offset
 * 0x29 sh rs,rt,offset
 * 0x2b sw rs,rt,offset
 * 0x31 rs,ft,offset
 * 0x3d rs,ft,offset
 * 0x2a rs,rt,offset
 * 0x2e rs,rt,offset
 * 0x38 rs,rt,offset
 **/

/**
 * j type
 * 2 j target
 * 3 jal target
**/








