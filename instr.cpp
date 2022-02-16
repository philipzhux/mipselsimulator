#include "instr.h"
#include "utils.h"
void addu(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    //cout<<"addu started"<<endl;
    registers[rd]=registers[rs]+registers[rt];
     //cout<<"addu ended"<<endl;
}

void add(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint32_t pre_rt = (registers[rs]>>31)&1;
    uint32_t pre_rs = (registers[rt]>>31)&1;
    uint32_t pre_rd = ((registers[rs]+registers[rt])>>31)&1;
    if((~(pre_rt^pre_rs))&(pre_rd^pre_rt)){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
        return;
    }
    registers[rd]=registers[rs]+registers[rt];
    return;
}

void addiu(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    registers[rt]=registers[rs]+imm;
    //cout<<"addiu called: rt="<<registers[rt]<<" rs="<<registers[rs]<<" imm="<<imm<<endl;
    return;
}


void addi(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    uint32_t pre_rs = (registers[rs]>>31)&1;
    uint32_t pre_imm = (imm>>31)&1;
    uint32_t pre_rt = ((registers[rs]+imm)>>31)&1;
    if((~(pre_rs^pre_imm))&(pre_rs^pre_rt)){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
        return;
    }
    registers[rt]=registers[rs]+imm;
    return;
}

void oand(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rs]&registers[rt];
}

void andi(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rt]=registers[rs]&imm;
    return;
}
void clo(uint32_t rd, uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint32_t temp; uint32_t res = 32;
    uint32_t inver = ~registers[rs];
    temp = (inver) >>16;
    if (temp != 0) {res-=16; inver = temp;}
    temp = inver >> 8;
    if (temp != 0) {res-=8; inver = temp;}
    temp = inver >> 4;
    if (temp != 0) {res-=4; inver = temp;}
    temp = inver >> 2;
    if (temp != 0) {res-=2; inver = temp;}
    temp = inver >> 1;
    if (temp != 0) registers[rd] = res - 2;
    else registers[rd] = res - inver;
}//clo converted to counting the leading zeros of its complement
void clz(uint32_t rd, uint32_t rs, uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint32_t temp; uint32_t res = 32;
    temp = registers[rs] >>16;
    if (temp != 0) {res-=16; registers[rs] = temp;}
    temp = registers[rs] >> 8;
    if (temp != 0) {res-=8; registers[rs] = temp;}
    temp = registers[rs] >> 4;
    if (temp != 0) {res-=4; registers[rs] = temp;}
    temp = registers[rs] >> 2;
    if (temp != 0) {res-=2; registers[rs] = temp;}
    temp = registers[rs] >> 1;
    if (temp != 0) registers[rd] = res - 2;
    else registers[rd] = res - registers[rs];
}//couting leading zeros with an recursive mindset with bitwise operation


void odivu(uint32_t rs, uint32_t rt, uint32_t* registers,unsigned char* &pc,  unsigned char* realm){

    uint32_t quotient = registers[rs]/registers[rt];
    uint32_t remainder = registers[rs]-registers[rt]*registers[rs];
    registers[32]=quotient; registers[33]=remainder;

}

void odiv(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint32_t quotient = int32_t(registers[rs])/int32_t(registers[rt]);
    uint32_t remainder = int32_t(registers[rs])-(int32_t(registers[rs])/int32_t(registers[rt]));
    registers[32]=quotient; registers[33]=remainder;
    return;
}

void mult(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint64_t res = uint64_t(int64_t(int32_t(registers[rs]))*int64_t(int32_t(registers[rt])));
    uint32_t hi = uint32_t((res>>32)&0xFFFFFFFF);
    uint32_t lo = uint32_t(res&0xFFFFFFFF);
    registers[33]=hi; registers[32]=lo;
    return;
}

void multu(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint64_t res = uint64_t(registers[rs])*uint64_t(registers[rt]);
    uint32_t hi = uint32_t((res>>32)&0xFFFFFFFF);
    uint32_t lo = uint32_t(res&0xFFFFFFFF);
    registers[33]=hi; registers[32]=lo;
    return;
}

void mul(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rs]*registers[rt];
}

void madd(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    int64_t res = int64_t(int32_t(registers[rs]))*int64_t(int32_t(registers[rt]));
    uint64_t prehi = uint64_t(registers[HID])<<32;
    uint64_t prelo = uint64_t(registers[LOD]);
    int64_t pre = int64_t(prehi|prelo);
    uint64_t sum = uint64_t(pre+res);
    uint32_t hi = uint32_t((sum>>32)&0xFFFFFFFF);
    uint32_t lo = uint32_t(sum&0xFFFFFFFF);
    registers[33]=hi; registers[32]=lo;
}

void maddu(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint64_t res = uint64_t(registers[rs])*uint64_t(registers[rt]);
    uint64_t prehi = uint64_t(registers[HID])<<32;
    uint64_t prelo = uint64_t(registers[LOD]);
    uint64_t pre = prehi|prelo;
    uint64_t sum = pre+res;
    uint32_t hi = uint32_t((sum>>32)&0xFFFFFFFF);
    uint32_t lo = uint32_t(sum&0xFFFFFFFF);
    registers[33]=hi; registers[32]=lo;
}

void msub(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    int64_t res = int64_t(int32_t(registers[rs]))*int64_t(int32_t(registers[rt]));
    uint64_t prehi = uint64_t(registers[HID])<<32;
    uint64_t prelo = uint64_t(registers[LOD]);
    int64_t pre = int64_t(prehi|prelo);
    uint64_t sum = uint64_t(pre-res);
    uint32_t hi = uint32_t((sum>>32)&0xFFFFFFFF);
    uint32_t lo = uint32_t(sum&0xFFFFFFFF);
    registers[33]=hi; registers[32]=lo;
}

void msubu(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint64_t res = uint64_t(registers[rs])*uint64_t(registers[rt]);
    uint64_t prehi = uint64_t(registers[HID])<<32;
    uint64_t prelo = uint64_t(registers[LOD]);
    res = ~res+1;
    uint64_t pre = prehi|prelo;
    uint64_t sum = pre+res;
    uint32_t hi = uint32_t((sum>>32)&0xFFFFFFFF);
    uint32_t lo = uint32_t(sum&0xFFFFFFFF);
    registers[33]=hi; registers[32]=lo;
}

void nor(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=~(registers[rs]|registers[rt]);
}

void oor(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rs]|registers[rt];
}

void ori(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rt]=registers[rs]|imm;
    return;
}

void sll(uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rt]<<shamt;
    return;
}

void sllv(uint32_t rs,uint32_t rt,uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rt]<<registers[rs];
    return;
}

void sra(uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=uint32_t(int32_t(registers[rt])>>shamt);
    return;
}

void srav(uint32_t rs,uint32_t rt,uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=uint32_t(int32_t(registers[rt])>>registers[rs]);
    return;
}

void srl(uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rt]>>shamt;
    return;
}

void srlv(uint32_t rs,uint32_t rt,uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rt]>>registers[rs];
    return;
}

void subu(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint32_t to_add = (~registers[rt])+1; //add negative rt
    registers[rd]=registers[rs]+to_add;
}

void sub(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    uint32_t to_add = (~registers[rt])+1;//add negative rt
    uint32_t pre_rt = (registers[rs]>>31)&1;
    uint32_t pre_rs = (to_add>>31)&1;
    uint32_t pre_rd = ((registers[rs]+to_add)>>31)&1;
    if((~(pre_rt^pre_rs))&(pre_rd^pre_rt)){
        exit(EXIT_FAILURE);
        return;
    }
    registers[rd]=registers[rs]+to_add;
    return;
}

void oxor(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rs]^registers[rt];
}
void xori(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rt]=registers[rs]^imm;
    return;
}
void lui(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rt]=imm<<16;
    return;
}
void sltu(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=registers[rs]<registers[rt]?1:0;
}
void slt(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=int32_t(registers[rs])<int32_t(registers[rt])?1:0;
}
void sltiu(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    registers[rt]=registers[rs]<imm?1:0;
    return;
}
void slti(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    registers[rt]=int32_t(registers[rs])<int32_t(imm)?1:0;
    return;
}
void beq(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rt]==registers[rs]) pc+=(int16_t(offset)<<2);
    return;
}
void bgez(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])>=0) pc+=(int16_t(offset)<<2);
    return;
}

void bgezal(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])>=0){
        registers[31]=get_faked(pc,realm);
        if(offset>0x7fff) pc-=(((~offset)+1)<<2);
        else  pc+=(offset<<2);
    }
    return;
}

void blez(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])<=0) pc+=(int16_t(offset)<<2);
    return;
}

void bltzal(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])<0){
        registers[31]=get_faked(pc,realm);
        pc+=(offset<<2);
    }
    return;
}
void bltz(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])<0) pc+=(int16_t(offset)<<2);
    return;
}
void bgtz(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])>0) pc+=(int16_t(offset)<<2);
    return;
}

void bne(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rt]!=registers[rs]) pc+=(offset<<2);
    return;
}

void j(uint32_t target,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    pc = get_real_mem((get_faked(pc,realm)&0xF0000000)|(target<<2),realm);
    return;
}

void jal(uint32_t target,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[31] = get_faked(pc,realm);
    pc = get_real_mem((get_faked(pc,realm)&0xF0000000)|(target<<2),realm);
    return;
}

void jalr(uint32_t rs, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd]=get_faked(pc,realm);
    pc = get_real_mem(registers[rs],realm);
}

void jr(uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    pc = get_real_mem(registers[rs],realm);
}

void teq(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rs]==registers[rt]){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void teqi(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rs]==imm){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tne(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rs]!=registers[rt]){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tnei(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rs]!=imm){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}

void tgeu(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rs]>=registers[rt]){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tgeiu(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    if(registers[rs]>=imm){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tge(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])>=int32_t(registers[rt])){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tgei(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    if(int32_t(registers[rs])>=int32_t(imm)){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}

void tltu(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(registers[rs]<=registers[rt]){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tltiu(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    if(registers[rs]<=imm){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tlt(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    if(int32_t(registers[rs])<=int32_t(registers[rt])){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}
void tlti(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    imm = uint32_t(int32_t(int16_t(imm)));
    if(int32_t(registers[rs])<=int32_t(imm)){
        cout<<"Exception Raised"<<endl;
        exit(EXIT_FAILURE);
    }
}

void lb(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by lb"<<endl;
        exit(EXIT_FAILURE);
    }
    unsigned char temp = *(get_real_mem(offset+registers[rs],realm));
    uint32_t res = uint32_t(temp);
    if((res>>7)==1) res = res|0xFFFFFF00;
    registers[rt]=res;
}

void lbu(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by lbu"<<endl;
        exit(EXIT_FAILURE);
    }
    unsigned char temp = *(get_real_mem(offset+registers[rs],realm));
    uint32_t res = uint32_t(temp);
    registers[rt]=res;
}

void lh(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]+1>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by lh"<<endl;
        exit(EXIT_FAILURE);
    }
    unsigned char first = *(get_real_mem(offset+registers[rs],realm)+1);
    unsigned char second = *(get_real_mem(offset+registers[rs],realm));
    uint32_t res = uint32_t(first)<<8|uint32_t(second);
    if((res>>15)==1) res = res|0xFFFF0000;
    registers[rt]=res;
}

void lhu(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]+1>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by lhu"<<endl;
        exit(EXIT_FAILURE);
    }
    unsigned char first = *(get_real_mem(offset+registers[rs],realm)+1);
    unsigned char second = *(get_real_mem(offset+registers[rs],realm));
    uint32_t res = uint32_t(first)<<8|uint32_t(second);
    registers[rt]=res;
}

void lw(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]+3>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by lw"<<endl;
        exit(EXIT_FAILURE);
    }
    //cout<<"lw offset="<<offset<<endl;
    //cout<<"lw addr="<<hex<<offset+registers[rs]<<endl;
    unsigned char* rm = get_real_mem(offset+registers[rs],realm);
    uint32_t res = ((*(rm+3))<<24) | ((*(rm+2))<<16) | ((*(rm+1))<<8) | (*(rm+0));
    registers[rt]=res;
}

void ll(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]+3>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by ll"<<endl;
        exit(EXIT_FAILURE);
    }
    unsigned char* rm = get_real_mem(offset+registers[rs],realm);
    uint32_t res = ((*(rm+3))<<24) | ((*(rm+2))<<16) | ((*(rm+1))<<8) | (*(rm+0));
    registers[rt]=res;
}

void sb(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by sb"<<endl;
        exit(EXIT_FAILURE);
    }
    *(get_real_mem(offset+registers[rs],realm)) = (unsigned char)(registers[rt]&0xFF);
    return;
}

void sh(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]+1>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by sh"<<endl;
        exit(EXIT_FAILURE);
    }
     *(get_real_mem(offset+registers[rs],realm)+1) = (unsigned char)((registers[rt] >> 8) & 0xFF);
     *(get_real_mem(offset+registers[rs],realm)) = (unsigned char)(registers[rt] & 0xFF);
    return;
}

void sw(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    if(offset+registers[rs]+3>0xa00000||offset+registers[rs]<0x400000){
        cout<<"Segmentation Fault Raised by sw"<<endl;
        exit(EXIT_FAILURE);
    }
    //cout<<offset<<endl;
    //cout<<"$sp="<<hex<<registers[rs]<<endl;
     *(get_real_mem(offset+registers[rs],realm)+3) = (unsigned char)((registers[rt] >> 24) & 0xFF); //little endian
     *(get_real_mem(offset+registers[rs],realm)+2) = (unsigned char)((registers[rt] >> 16) & 0xFF);
     *(get_real_mem(offset+registers[rs],realm)+1) = (unsigned char)((registers[rt] >> 8) & 0xFF);
     *(get_real_mem(offset+registers[rs],realm)+0) = (unsigned char)(registers[rt] & 0xFF);

    return;
}

void swl(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    uint32_t address = offset+registers[rs];
    uint32_t remainder = address & 3;
    uint32_t base = address - remainder;
    string rt_org = bitset<32>(registers[rt]).to_string();
    string res;
    for(uint32_t i=0;i<=remainder;i++){
        *get_real_mem(base+i,realm) = stoi(rt_org.substr(i*8,8),nullptr,2);
    }
    return;
}

void swr(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    uint32_t address = offset+registers[rs];
    uint32_t remainder = address & 3;
    uint32_t base = address - remainder;
    string rt_org = bitset<32>(registers[rt]).to_string();
    string res;
    for(uint32_t i=remainder;i<=3;i++){
        *get_real_mem(base+3-i,realm) = stoi(rt_org.substr(24-i*8,8),nullptr,2);
    }
    return;
}

void lwl(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    uint32_t address = offset+registers[rs];
    uint32_t remainder = address & 3;
    uint32_t base = address - remainder;
    string rt_org = bitset<32>(registers[rt]).to_string();
    string res;
    for(uint32_t i=0;i<=remainder;i++){
        res = bitset<8>(*get_real_mem(base+i,realm)).to_string()+ res;
    }
    res = res + rt_org.substr(res.length());
    registers[rt] = stol(res,nullptr,2);
    return;
}

void lwr(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
    uint32_t address = offset+registers[rs];
    uint32_t remainder = address & 3;
    uint32_t base = address - remainder;
    string rt_org = bitset<32>(registers[rt]).to_string();
    string res;
    for(uint32_t i=remainder;i<=3;i++){
        res = bitset<8>(*get_real_mem(base+i,realm)).to_string()+ res;
    }
    res = rt_org.substr(0,32-res.length()) + res;
    registers[rt] = stol(res,nullptr,2);
    return;
}

void sc(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    offset = uint32_t(int32_t(int16_t(offset)));
     *(get_real_mem(offset+registers[rs],realm)+3) = (unsigned char)((registers[rt] >> 24) & 0xFF);
     *(get_real_mem(offset+registers[rs],realm)+2) = (unsigned char)((registers[rt] >> 16) & 0xFF);
     *(get_real_mem(offset+registers[rs],realm)+1) = (unsigned char)((registers[rt] >> 8) & 0xFF);
     *(get_real_mem(offset+registers[rs],realm)+0) = (unsigned char)(registers[rt] & 0xFF);
    unsigned char temp = (unsigned char)(registers[rt]&0xFF);
    *(get_real_mem(offset+registers[rs],realm)) = temp;
    return;
}

void mfhi(uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd] = registers[HID];
    return;
}

void mflo(uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[rd] = registers[LOD];
    return;
}

void mthi(uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[HID] = registers[rs];
    return;
}

void mtlo(uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm){
    registers[LOD] = registers[rs];
    return;
}