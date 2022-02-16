#include "syscall.h"
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
        exit(registers[REGISTER_SET["$a0"]]);
        break;
    }
        default:
        break;
    };
}