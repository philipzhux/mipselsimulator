//assembling part
#include "assembler.h"
#include "utils.h"
void assembler(map<string,uint32_t> &label_record,vector<mip_t> &mips_struct_set,vector<var_t> &vars, fstream &_in) {
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
                //for(size_t i=0;i<mips_struct_set.size();i++) process(mips_struct_set[i].ins,mips_struct_set[i].argv,mips_struct_set[i].code,mips_struct_set[i].address,label_record);
                //output_mc(mips_struct_set);
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
                                var_t v; v.name=name; v.type=type; v.content=content;
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
                                var_t v; v.name=name; v.type=type; v.content=content;
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
            mip_t mc;
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
            mips_struct_set.push_back(mc);
            address+=4;
        }
    }
    if(label.length()>0){
        label_record.insert(pair<string, uint32_t>(label,address)); //if label is not empty
        label.clear();//reset label
    }
    
    for(size_t i=0;i<mips_struct_set.size();i++) process(mips_struct_set[i].ins,mips_struct_set[i].argv,mips_struct_set[i].code,mips_struct_set[i].address,label_record);
    if(MC_ENABLE) output_mc(mips_struct_set);
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

 