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
//static datas
const uint32_t HID=33; const  uint32_t LOD=32;
const string DQUOTESYMBOL = "!^&*D1O2U3B4L5E213i7sadkfhl6*7@8Q9UOTE^^%*";
const string SQUOTESYMBOL = "12ki23hrlwbfjg21837r84276&2@#%Y";
const bool AS_TEST=false; //true for asssembler test
const bool MC_ENABLE=false; //true to output machine code
static map<string,int> REGISTER_SET = {{"$0",0},{"$zero",0},{"$at",1},{"$v0",2},{"$v1",3},{"$a0",4},{"$a1",5},{"$a2",6},{"$a3",7},
{"$t0",8},{"$t1",9},{"$t2",10},{"$t3",11},{"$t4",12},{"$t5",13},{"$t6",14},{"$t7",15},{"$s0",16},{"$s1",17},{"$s2",18},{"$s3",19},
{"$s4",20},{"$s5",21},{"$s6",22},{"$s7",23},{"$t8",24},{"$t9",25},{"$k0",26},{"$k1",27},{"$gp",28},{"$sp",29},{"$fp",30},{"$ra",31}};
static map<string,unsigned char> INSTRUCT_ENUM = {{"add",0},{"addu",1},{"addi",2},{"addiu",3},
{"and",4},{"andi",5},{"clo",6},{"clz",7},{"div",8},{"divu",9},{"mult",10},{"multu",11},
{"mul",12},{"madd",13},{"msub",14},{"maddu",15},{"msubu",16},{"nor",17},{"or",18},{"ori",19},
{"sll",20},{"sllv",21},{"sra",22},{"srav",23},{"srl",24},{"srlv",25},{"sub",26},{"subu",27},
{"xor",28},{"xori",29},{"lui",30},{"slt",31},{"sltu",32},{"slti",33},{"sltiu",34},{"beq",35},
{"bgez",36},{"bgezal",37},{"bgtz",38},{"blez",39},{"bltzal",40},{"bltz",41},{"bne",42},{"j",43},
{"jal",44},{"jalr",45},{"jr",46},{"teq",47},{"teqi",48},{"tne",49},{"tnei",50},{"tge",51},
{"tgeu",52},{"tgei",53},{"tgeiu",54},{"tlt",55},{"tltu",56},{"tlti",57},{"tltiu",58},{"lb",59},
{"lbu",60},{"lh",61},{"lhu",62},{"lw",63},{"lwc1",60},{"lwl",65},{"lwr",66},{"ll",67},
{"sb",68},{"sh",69},{"sw",70},{"swl",71},{"swr",72},{"sc",73},{"mfhi",74},{"mflo",75},{"mthi",76},{"mtlo",77},{"syscall",78},{"li",79},{"move",80}};

//structs
struct mip{
    vector<string> argv;
    string ins;
    uint32_t address;
    string code;
};

struct variable{
    string name;
    string type;
    string content;
};
//prototypes
void replaceAll(string& str, const string& from, const string& to);
void output_mc(vector<mip> mips_set);
void op_ins(uint32_t op,uint32_t rs,uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t funct,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void j_ins(uint32_t op,uint32_t target,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void i_ins(uint32_t op, uint32_t rs, uint32_t rt, uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void assembler(map<string,uint32_t> &label_record,vector<mip> &mips_set);
unsigned char* get_real_mem(uint32_t faked,unsigned char*  realm);
uint32_t get_faked(unsigned char* real,  unsigned char* realm);
void process(string ins,vector<string> argv,string &code,uint32_t &address,string &label, map<string,uint32_t> &record);
void assembler(map<string,uint32_t> &label_record,vector<mip> &mips_set,vector<variable> &vars, fstream &_in);
void process(string ins,vector<string> argv,string &code,uint32_t address,map<string,uint32_t> &record);
void initialize(unsigned char* &real_mem, unsigned char* &pc,unsigned char* &sc,  vector<mip> mips_set,uint32_t* &registers,vector<variable> &vars);
void operate(uint32_t ins,uint32_t* registers,unsigned char* &pc, unsigned char* realm,unsigned char* &sc,fstream &_in, fstream &_out);
void addu(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void add(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void addiu(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void addi(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void oand(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void andi(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void clo(uint32_t rd, uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void clz(uint32_t rd, uint32_t rs, uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void odivu(uint32_t rs, uint32_t rt, uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void odiv(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void mult(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void multu(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void mul(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void madd(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void maddu(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void msub(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void msubu(uint32_t rs, uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void nor(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void oor(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void ori(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sll(uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sllv(uint32_t rs,uint32_t rt,uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sra(uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void srav(uint32_t rs,uint32_t rt,uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void srl(uint32_t rt,uint32_t rd,uint32_t shamt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void srlv(uint32_t rs,uint32_t rt,uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void subu(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sub(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void oxor(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void xori(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lui(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sltu(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void slt(uint32_t rs, uint32_t rt, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sltiu(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void slti(uint32_t rs,uint32_t rt,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void beq(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void bgez(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void bgezal(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void bgtz(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void blez(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void bltzal(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void bltz(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void bne(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void j(uint32_t target,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void jal(uint32_t target,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void jalr(uint32_t rs, uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void jr(uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void teq(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void teqi(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tne(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tnei(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tgeu(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tgeiu(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tge(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tgei(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tltu(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tltiu(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tlt(uint32_t rs,uint32_t rt,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void tlti(uint32_t rs,uint32_t imm,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lb(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lbu(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lh(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lhu(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lw(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void ll(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sb(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sh(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sw(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void swl(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void swr(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lwl(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void lwr(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sc(uint32_t rs,uint32_t rt,uint32_t offset,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void mfhi(uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void mflo(uint32_t rd,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void mthi(uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void mtlo(uint32_t rs,uint32_t* registers,unsigned char* &pc,  unsigned char* realm);
void sys_call(uint32_t* registers,unsigned char* realm,unsigned char* &sc,fstream &_in, fstream &_out);


//main function
int main(int argc,const char* argv[]) {
    if(argc!=4){
        cout<<"Invalid Argument Syntax";
        return 0;
    }

    fstream _asm(argv[1]);
    fstream _in(argv[2]);
    fstream _out(argv[3],ios::out);
    map<string,uint32_t> label_record; unsigned char* realm; unsigned char* pc; uint32_t* registers; vector<mip> mips_set;
    //label_record to store mapping between label and address; realm is actually "base"
    unsigned char* sc; //sc: pointer to the top of the heap (for .data section in simulator)
    vector<variable> vars; //for variables in .data section in simulator
    //label_record.insert(pair<string, int>("label", int_address))
    assembler(label_record,mips_set,vars,_asm);//assembler in here
    initialize(realm,pc,sc,mips_set,registers,vars); //initialize for simulator
    if(!AS_TEST){ //this if frame of "AS_TEST" is only for the use of debug, ignore it.
        while(true){
            uint32_t ins = (((*(pc+3))<<24) | ((*(pc+2))<<16) | ((*(pc+1))<<8) | (*(pc)));
            if(ins==0) {
                //cout<<"EXIT!!!!"<<ins<<endl;
                break;}
            //"<<(get_faked(pc,realm)-0x400000)/4<<": "<<std::bitset<32>(ins)<<endl;
            pc+=4;
            operate(ins,registers,pc,realm,sc,_in,_out);
        }
    }

}

//machine code output: not required in the instruction, just for the use of debug
void output_mc(vector<mip> mips_set){
    for(size_t i=0;i<mips_set.size();i++){
        cout<<mips_set[i].code<<endl;
    }
}

//assembling part
void assembler(map<string,uint32_t> &label_record,vector<mip> &mips_set,vector<variable> &vars, fstream &_in) {
    uint32_t address=0x400000; bool text_start=AS_TEST; bool data_start=false; string src; string label; string mips;
    string name; string type; string content; string path;
    //label_record.insert(pair<string, int>("label", int_address))
    //text_start=true if AS_TEST is ture (for Schrodinger Zhu's test)
    //text_start should be initially set to be false, AS_TEST is only for the use of debug
    while (getline(_in, src)) {
        //dealing with comments
        size_t spos = src.find("#");
        if (spos==0) continue; //skip if starting with #
        if (spos!=string::npos) src = src.substr(0,spos);
        if(all_of(src.begin(),src.end(),::isspace)) continue;// skip with only ws before #
        if(src.find(".text")!=string::npos){
            if(data_start) data_start = false;
            text_start = true; //flag text_start means the .text section begins
            src=src.substr(src.find(".text")+5,src.length()-(src.find(".text")+5));
            if(all_of(src.begin(),src.end(),::isspace)) continue;
        }
        if(src.find(".data")!=string::npos){
            if(text_start){
                text_start = false;
                //for(size_t i=0;i<mips_set.size();i++) process(mips_set[i].ins,mips_set[i].argv,mips_set[i].code,mips_set[i].address,label_record);
                //output_mc(mips_set);
            }
            data_start = true;
            src=src.substr(src.find(".data")+5,src.length()-(src.find(".data")+5));
            if(all_of(src.begin(),src.end(),::isspace)) continue;
        }

        if(data_start){ //handling the .data part
            replaceAll(src,"\\\"",DQUOTESYMBOL);
            replaceAll(src,"\\\'",SQUOTESYMBOL);
            /**
            * what is this for: 
            * if there is a quote["] inside a string enclosed by quote,
            * it will be a headache to handle, however, the quote inside a string
            * should be expressed as [\"], so I replace [\"] to a long arbitary string
            * to avoid messing up with the enclosing quote. [\"] in C++ shoud be express as [\\\"]
            * \\ for [\] and \" for [""]. Finally after I handle the parsing, I will replace
            * it back to quote
            **/
            istringstream str_s(src);
            string temp;
            unsigned counter = 0;
            string segment;
            stringstream stream_input(src);
            while(std::getline(stream_input, temp, '\"'))
            {
                ++counter;
                if (counter % 2 == 0)
                {
                    if (!temp.empty())
                        {

                            if(name.length()>0&&type.length()>0){
                                content = temp;
                                variable v; v.name=name; v.type=type; v.content=content;
                                name.clear(); type.clear(); content.clear();
                                vars.push_back(v); //cout<<"pushing into mem:"<<v.content<<endl;
                            }
                        }
                }
                else
                {
                    std::stringstream stream_temp(temp);
                    while(stream_temp>>temp)
                        if (!temp.empty()){
                            //cout<<temp<<endl;
                            if(temp.find(":")==temp.length()-1&&content.length()==0){
                                name=temp.substr(0,temp.find(":"));
                                continue;
                            }
                            if(temp.find(".")==0 && name.length()>0&&content.length()==0){
                                type=temp.substr(temp.find(".")+1);
                                continue;
                            }

                            if(name.length()>0&&type.length()>0){
                                content = temp;
                                variable v; v.name=name; v.type=type; v.content=content;
                                //cout<<"data: name="<<name<<" type="<<type<<" content="<<content<<endl;
                                name.clear(); type.clear(); content.clear();
                                vars.push_back(v); //cout<<"pushing into mem:"<<v.content<<endl;
                            }
                        }
                }
            }

            
        }
        if(text_start){
            istringstream str_s(src); //tokenize each line
            string temp;
            int loop_count = 0;
            mip mc;
            //reading the line word by word
            while(str_s>>temp){
                temp.erase(remove_if(temp.begin(), temp.end(), ::isspace), temp.end());//remove all ws in the single element
                if(temp.empty()) continue; //skip if empty
                if(temp.find(":")!=string::npos){
                    label = temp.substr(0,temp.find(":")); //if label exists, store it in variable label
                    continue;
                }
                if(loop_count==0){
                    mc.ins = temp;
                    mc.address = address;
                }
                else{
                    //temp is a C++ string variable
                    char* ctemp = new char[temp.length()+1];
                    strcpy (ctemp, temp.c_str());
                    char* token = strtok(ctemp, " ,");
                    while( token != NULL ) {
                        mc.argv.push_back(string(token));
                        //cout<<token<<endl;
                        token = strtok(NULL,  " ,");
                    }
                    delete[] ctemp;
                   
                    
                }
                loop_count ++;
            }
            //process the single line
            if(all_of(mc.ins.begin(),mc.ins.end(),::isspace)) continue; //skip if empty instruction
            if(label.length()>0){
                    label_record.insert(pair<string, uint32_t>(label,address)); //if label is not empty
                    label.clear();//reset label
            }
            mips_set.push_back(mc);
            address+=4;
        }
    }
    if(label.length()>0){
        label_record.insert(pair<string, uint32_t>(label,address)); //if label is not empty
        label.clear();//reset label
    }
    
    for(size_t i=0;i<mips_set.size();i++) process(mips_set[i].ins,mips_set[i].argv,mips_set[i].code,mips_set[i].address,label_record);
    if(MC_ENABLE) output_mc(mips_set);
 }

void process(string ins,vector<string> argv,string &code,uint32_t address,map<string,uint32_t> &record){
     if(!INSTRUCT_ENUM.count(ins)){
         cout<<"Terminated with Error: Unsupported Instruction <"<<ins<<">"<<endl;
         exit(EXIT_FAILURE);
     }
     switch (INSTRUCT_ENUM.at(ins))
     {
     case 0: //add
     {

        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x20).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 1: //addu
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x21).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 2: //addi
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(8).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(stoi(argv[2],nullptr,0)).to_string();
        //cout<<"imm of addi is 0x"<<hex<<stoi(argv[2],nullptr,0)<<endl;
        code = op+rs+rt+imm;
        break;
     }
     case 3: //addiu
     {
         if(argv.size()!=3){
             cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
         }
         string op = bitset<6>(9).to_string();
         string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
         string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
         string imm = bitset<16>(stoi(argv[2],nullptr,0)).to_string();
         code = op+rs+rt+imm;
         break;
     }

     case 4: //and
     {  if(argv.size()!=3){
             cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
         }
         string op = bitset<6>(0).to_string();
         string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
         string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
         string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
         string shamt = bitset<5>(0).to_string();
         string funct = bitset<6>(0x24).to_string();
         code = op+rs+rt+rd+shamt+funct;
         break;
     }

     case 5://"andi"
     {  if(argv.size()!=3){
             cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
         }
         string op = bitset<6>(0xc).to_string();
         string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
         string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
         string imm = bitset<16>(stoi(argv[2],nullptr,0)).to_string();
         code = op+rs+rt+imm;
         break;
     }

     case 6://"clo"
     {if(argv.size()!=2){
             cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
         }
         string op = bitset<6>(0x1c).to_string();
         string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
         string rt = bitset<5>(0).to_string();
         string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
         string shamt = bitset<5>(0).to_string();
         string funct = bitset<6>(0x21).to_string();
         code = op+rs+rt+rd+shamt+funct;
         break;
     }
     case 7://"clz"
     {
         if(argv.size()!=2){
             cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
         }
         string op = bitset<6>(0x1c).to_string();
         string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
         string rt = bitset<5>(0).to_string();
         string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
         string shamt = bitset<5>(0).to_string();
         string funct = bitset<6>(0x20).to_string();
         code = op+rs+rt+rd+shamt+funct;
         break;
     }
     case 8://"div"
     {

        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(0x1a).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 9://"divu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(0x1b).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 10://"mult"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(0x18).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 11://"multu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(0x19).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 12://"mul"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x1c).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(2).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 13://"madd"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x1c).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(0).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 14://"msub"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x1c).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(4).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 15://"maddu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x1c).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(1).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 16://"msubu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x1c).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string dnw = bitset<10>(0).to_string();  //10 zeros of dont-know-what
        string end = bitset<6>(5).to_string();
        code = op+rs+rt+dnw+end;
        break;
     }
     case 17://"nor"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x27).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 18://"or"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x25).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 19://"ori"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0xd).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(stoi(argv[2],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 20://"sll"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(0).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(stoi(argv[2],nullptr,0)).to_string();
        string funct = bitset<6>(0).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 21://"sllv"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(4).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 22://"sra"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(0).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(stoi(argv[2],nullptr,0)).to_string();
        string funct = bitset<6>(3).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 23://"srav"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(7).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }

     case 24://"srl"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(0).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(stoi(argv[2],nullptr,0)).to_string();
        string funct = bitset<6>(2).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 25://"srlv"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(6).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 26://"sub"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x22).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 27://"subu"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x23).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 28://"xor"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x26).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 29://"xori"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0xe).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(stoi(argv[2],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 30://"lui"
     {
         if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0xf).to_string();
        string rs = bitset<5>(0).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 31://"slt"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x2a).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 32://"sltu"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[2])).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x2b).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 33://"slti"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0xa).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(stoi(argv[2],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 34://"sltiu"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0xb).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(stoi(argv[2],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
       }

     case 35://"beq"
     {
        if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = strtol(argv[2].c_str(), &p, 0);
        if(*p != '\0') {
                 if(!record.count(argv[2])){
                    cout<<"Terminated with Error: Jumping to an Unspecified Label <"<<argv[2]<<">"<<endl;
                    exit(EXIT_FAILURE);
                }
                converted = long(record.at(argv[2]));
        }
        string op = bitset<6>(4).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string offset = bitset<16>((converted-address)/4-1).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 36://"bgez"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long int convert = strtol(argv[1].c_str(), &p, 0);
        if(*p != '\0') {
                 if(!record.count(argv[2])){
                    cout<<"Terminated with Error: Jumping to an Unspecified Label <"<<argv[2]<<">"<<endl;
                    exit(EXIT_FAILURE);
                }
                convert = long(record.at(argv[2]));
        }
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(1).to_string();
        bitset<16> a(convert);
        string offset=a.to_string();//= bitset<16>(convert).to_string();                                                                                                                                                                                                                                                                                                                                                                                            QQQQ).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 37://"bgezal"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long convert = strtol(argv[1].c_str(), &p, 0);
        if(*p != '\0') {
                 if(!record.count(argv[2])){
                    cout<<"Terminated with Error: Jumping to an Unspecified Label <"<<argv[2]<<">"<<endl;
                    exit(EXIT_FAILURE);
                }
                convert = long(record.at(argv[2]));
        }
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0x11).to_string();
        string offset = bitset<16>((convert-address)/4-1).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 38://"bgtz"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = strtol(argv[1].c_str(), &p, 0);
        if(*p != '\0') converted = long(record.at(argv[1]));
        string op = bitset<6>(7).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0).to_string();
        string offset = bitset<16>((converted-address)/4-1).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 39://"blez"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = strtol(argv[1].c_str(), &p, 0);
        if(*p != '\0') converted = long(record.at(argv[1]));
        string op = bitset<6>(6).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0).to_string();
        string offset = bitset<16>((converted-address)/4-1).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 40://"bltzal"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = strtol(argv[1].c_str(), &p, 0);
        if(*p != '\0') converted = long(record.at(argv[1]));
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0x10).to_string();
        string offset = bitset<16>((converted-address)/4-1).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 41://"bltz"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = strtol(argv[1].c_str(), &p, 0);
        if(*p != '\0') converted = long(record.at(argv[1]));
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0).to_string();
        string offset = bitset<16>((converted-address)/4-1).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 42://"bne"
     {
         if(argv.size()!=3){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = strtol(argv[2].c_str(), &p, 0);
        if(*p != '\0') converted = long(record.at(argv[2]));
        string op = bitset<6>(5).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string offset = bitset<16>((converted-address)/4-1).to_string();
        code = op+rs+rt+offset;
        break;
     }
     case 43://"j"
     {
         if(argv.size()!=1){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = (strtol(argv[0].c_str(), &p, 0)&0xffffffc)>>2;
        if(*p != '\0') converted = long((record.at(argv[0])&0xffffffc)>>2);
        string op = bitset<6>(2).to_string();
        string target = bitset<26>(converted).to_string();
        code = op+target;
        break;
     }
     case 44://"jal"
     {
         if(argv.size()!=1){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        char* p;
        long converted = (strtol(argv[0].c_str(), &p, 0)&0xffffffc)>>2;
        if(*p != '\0') converted = long((record.at(argv[0])&0xffffffc)>>2);
        string op = bitset<6>(3).to_string();
        string target = bitset<26>(converted).to_string();
        code = op+target;
        break;
     }
     case 45://"jalr"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(9).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 46://"jr"
     {
        if(argv.size()!=1){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(8).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
        }
     case 47://"teq"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x34).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 48://"teqi"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0xc).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 49://"tne"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x36).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
         }
     case 50://"tnei"
     {

        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0xe).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 51://"tge"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x30).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
        }
     case 52://"tgeu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x31).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
        }
     case 53://"tgei"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(8).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
        }
     case 54://"tgeiu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(9).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 55://"tlt"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x32).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 56://"tltu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x33).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 57://"tlti"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0xa).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 58://"tltiu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(1).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0xb).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;
     }
     case 59://"lb"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x20).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 60://"lbu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x24).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 61://"lh"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x21).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 62://"lhu"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x25).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 63://"lw"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x23).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        //cout<<"stoi("<<address_org<<")"<<endl;
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 64://"lwc1"
     {
         cout<<"coprocessor unsupported, line skipped"<<endl;
         break;
     }
     case 65://"lwl"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x22).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 66://"lwr"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x26).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 67://"ll"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x30).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 68://"sb"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x28).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 69://"sh"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x29).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 70://"sw"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x2b).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 71://"swl"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x2a).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 72://"swr"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x2e).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 73://"sc"
     {
        if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0x38).to_string();
        string rs_org = argv[1].substr(argv[1].find("(")+1,argv[1].find(")")-argv[1].find("(")-1);
        string address_org = argv[1].substr(0,argv[1].find("("));
        string rs = bitset<5>(REGISTER_SET.at(rs_org)).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string address = bitset<16>(stoi(address_org,nullptr,0)).to_string();
        code = op+rs+rt+address;
        break;
     }
     case 74://"mfhi"
     {
        if(argv.size()!=1){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(0).to_string();
        string rt = bitset<5>(0).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x10).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 75://"mflo"
     {
        if(argv.size()!=1){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(0).to_string();
        string rt = bitset<5>(0).to_string();
        string rd = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x12).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 76://"mthi"
     {
        if(argv.size()!=1){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x11).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 77://"mtlo"
     {
        if(argv.size()!=1){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(0).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string rt = bitset<5>(0).to_string();
        string rd = bitset<5>(0).to_string();
        string shamt = bitset<5>(0).to_string();
        string funct = bitset<6>(0x13).to_string();
        code = op+rs+rt+rd+shamt+funct;
        break;
     }
     case 78://"syscall"
     {
        code = "00000000000000000000000000001100";
        break;
     }
     case 79://"li"
     {
         if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(8).to_string();
        string rs = bitset<5>(REGISTER_SET.at("$zero")).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(stoi(argv[1],nullptr,0)).to_string();
        code = op+rs+rt+imm;
        break;

     }
     case 80://"move"
     {
         if(argv.size()!=2){
            cout<<"Syntax error: too much or too less arguments for <"<<ins<<">"<<endl;
            exit(EXIT_FAILURE);
        }
        string op = bitset<6>(8).to_string();
        string rs = bitset<5>(REGISTER_SET.at(argv[1])).to_string();
        string rt = bitset<5>(REGISTER_SET.at(argv[0])).to_string();
        string imm = bitset<16>(0).to_string();
        code = op+rs+rt+imm;
        break;

     }
      
     default: break;
     }
     return;
 }

//simulating part
void initialize(unsigned char* &real_mem, unsigned char* &pc,unsigned char* &sc,  vector<mip> mips_set,uint32_t* &registers,vector<variable> &vars){
    /**
     * the stimulated memory block [begins with/is denoted by] real_mem
     * Access with faked address: real_mem[FAKED-0x400000] or *(get_real_mem(FAKED))
     * START OF TEXT SEGMENT : real_mem[0]
     * START OF STATIC DATA : real_mem[0x100000]
     * START OF STACK/END OF MEM BLOCK : real_mem[0x600000-1]
    **/

    //initialize memory block
    real_mem = new unsigned char[0x600000+1];
    for(size_t i=0;i<0x600000;i++) real_mem[i]=0;
    //store machine code into text segment
    for(size_t i=0;i<mips_set.size();i++){
        uint32_t n = uint32_t(stol((mips_set[i]).code,nullptr,2));
        *get_real_mem(mips_set[i].address+3,real_mem) = (unsigned char)((n >> 24) & 0xFF);
        *get_real_mem(mips_set[i].address+2,real_mem) = (unsigned char)((n >> 16) & 0xFF);
        *get_real_mem(mips_set[i].address+1,real_mem) = (unsigned char)((n >> 8) & 0xFF);
        *get_real_mem(mips_set[i].address+0,real_mem) = (unsigned char)(n & 0xFF);
    }

    //initialize registers
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

unsigned char* get_real_mem(uint32_t faked,unsigned char*  realm){
    unsigned char* res;
    res=realm+faked-0x400000;
    return res;
}

uint32_t get_faked(unsigned char* real,  unsigned char* realm){
    return uint32_t(real-realm)+0x400000;
}

void operate(uint32_t ins,uint32_t* registers,unsigned char* &pc, unsigned char* realm,unsigned char* &sc,fstream &_in, fstream &_out){
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

void sys_call(uint32_t* registers,unsigned char* realm,unsigned char* &sc,fstream &_in, fstream &_out){
    //cout<<"Sys_call: the value of $v0 is:"<<registers[REGISTER_SET["$v0"]]<<endl;
    //cout<<"Sys_call: the value of $a0 is:"<<registers[REGISTER_SET["$a0"]]<<endl;
    //cout<<"the memeory value @a0 is:"<<(*get_real_mem(registers[REGISTER_SET["$a0"]],realm))<<endl;
    switch(registers[REGISTER_SET["$v0"]]){
        case 1: //print int
    {
        _out<<*(registers+REGISTER_SET["$a0"]);
        _out.flush();
         break;
    }
        case 4: //print string
    {
        if(registers[REGISTER_SET["$a0"]]>0xa00000||registers[REGISTER_SET["$a0"]]<0x400000){
            cout<<"Segmentation Fault: raised by print string"<<endl;
            cout<<"Address attempted to access:0x"<<hex<<registers[REGISTER_SET["$a0"]]<<endl;
             exit(EXIT_FAILURE);
        }
         unsigned char* init = get_real_mem(registers[REGISTER_SET["$a0"]],realm);
         //cout<<"getting string from address 0x"<<hex<<registers[REGISTER_SET["$a0"]]<<endl;
         //cout<<"the first char to output is "<<(*init)<<endl;
         while((*init)!='\0') _out<<*init++;
         _out.flush();
         break;
    }
        case 5: //read int
    {
        uint32_t i;
        string _in_str;
        getline(_in,_in_str);
        i = uint32_t(stoi(_in_str,nullptr,0));
        //cout<<"int read:"<<i<<endl;
        *(registers+REGISTER_SET["$v0"])=i;
         break;
    }
        case 8: //read string
    {
            if(registers[REGISTER_SET["$a0"]]>0xa00000||registers[REGISTER_SET["$a0"]]<0x400000){
            cout<<"Segmentation Fault: raised by read string"<<endl;
            cout<<"Address attempted to access:0x"<<hex<<registers[REGISTER_SET["$a0"]]<<endl;
             exit(EXIT_FAILURE);
        }

         uint32_t len = *(registers+REGISTER_SET["$a1"]);//length
         uint32_t i=0;
         char c;
         unsigned char* base = get_real_mem(*(registers+REGISTER_SET["$a0"]),realm);
         while(i<len-1){
            _in.get(c);
            *(base+i)=(unsigned char)(c);
            i++;
            if(c=='\n') break;
         }
         if(len>0) *(base+i)=(unsigned char)('\0');
         break;
    }
        case 9: //sbrk (similar to malloc)
    {
            uint32_t amount = *(registers+REGISTER_SET["$a0"]);
            *(registers+REGISTER_SET["$v0"]) = get_faked(sc,realm);
            sc+=amount;
            break;
    }
        case 10: //exit
    {
            exit(1);
            break;
    }

        case 11: //print char
    {
            char c=0;
            c+=*(registers+REGISTER_SET["$a0"]);
            _out<<c;
            _out.flush();
            break;
    }
        case 12: //read char
    {
            char c ;//= getchar()
            _in.get(c);
            *(registers+REGISTER_SET["$v0"]) = uint32_t(c);
            break;
    }
        case 13: //open file
    {
            if(registers[REGISTER_SET["$a0"]]>0xa00000||registers[REGISTER_SET["$a0"]]<0x400000){
            cout<<"Segmentation Fault: raised by open file"<<endl;
            cout<<"Address attempted to access:0x"<<hex<<registers[REGISTER_SET["$a0"]]<<endl;
             exit(EXIT_FAILURE);
        }
        uint32_t mode = registers[REGISTER_SET["$a2"]];
        uint32_t flags = registers[REGISTER_SET["$a1"]];
            //fstream* f_ptr = new fstream;
            unsigned char* init = get_real_mem(registers[REGISTER_SET["$a0"]],realm);
            ostringstream oss;
            while(*init) oss<<*init++;
            *(registers+REGISTER_SET["$v0"]) = open (oss.str().c_str(),flags,mode);
            //f_ptr->open(oss.str().c_str());
            //f_ptrs.push_back(f_ptr);
            break;

        }

        case 14: //read file
    {
        uint32_t fd = registers[REGISTER_SET["$a0"]];
        uint32_t len = registers[REGISTER_SET["$a2"]];
        uint32_t buffer = registers[REGISTER_SET["$a1"]];
        //uint32_t read_len = 0;
        unsigned char* buffer_ptr=get_real_mem(buffer,realm);
        registers[REGISTER_SET["$v0"]] = read(fd, buffer_ptr, len);
        /**
         * char c;
        while (read_len<len && f_ptrs[fd]->get(c))
        {
            *buffer_ptr++ = (unsigned char)(c);
            read_len++;
        }
        **/
        break;
    }
        case 15: //write file
    {   uint32_t fd = registers[REGISTER_SET["$a0"]];
        uint32_t len = registers[REGISTER_SET["$a2"]];
        uint32_t buffer = registers[REGISTER_SET["$a1"]];
        //uint32_t written_len = 0;
        unsigned char* buffer_ptr=get_real_mem(buffer,realm);
        //while (written_len<len) (*(f_ptrs[fd]))<<*buffer_ptr++;
        registers[REGISTER_SET["$v0"]] = write (fd, buffer_ptr, len);
        break;
    }
        case 16: //close file
    {
        uint32_t fd = registers[REGISTER_SET["$a0"]];
        close(fd); 
        //f_ptrs[fd]->close();
        break;
    }
        case 17:
    {
        exit( registers[REGISTER_SET["$a0"]]);
        break;
    }
        default:
        break;
    };
}



void replaceAll(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
