# Mipsel Simulator
A year-two course project of Computer Architecture (CSC3050) at CUHK.  

This is a project of assembler and simulator combined, specifc for assembly language based on MIPS little-endian architecture.
The project assemble the codes, simulate the main memory with a 6MB dynamically allocated memory and execute the machine codes. 

At the time the project is written, the course has not yet covered the topic of coprocessor or atomic lock, therefore related instructions are ignored.

## Specs:
* simulator.cpp/simulator.h: 
  * simu_init: initialise the memory layout and load the variables and machine code into the currect chunk of memory, with the stack pointer and pc initialised as well
  * simulate: simulate the execution of the machine code, already loaded in simu_init
  * op_ins: entry point for simulating op type instructions
  * j_ins: entry point for simulating j type instructions
  * i_ins: entry point for simulating i type instructions

* instr.cpp/instr.h:
  * (...)
    [the actuall implementation down to each instruction, called by op_ins/j_ins/i_ins in simulator.cpp]

* syscall.cpp/syscall.h:
  * implementation of simulating syscalls

* utils.cpp/utils.h: utility functions
  
* assembler.cpp/assembler.h:
  * assembler: entry point to translate the assembly code from ```fstream &_in``` to machine code put in ```vector<mip_t> &mips_struct_set``` (mips_struct_set[i].code is where machine code resides), at the same time parse the store the variables define in the aseembly into ```vector<mip_t> &vars```
  * process: does the dirty parsing job of assembler

* project.h:
  * mip_t: data structures composing:
    * argv: arguments of the assembly *(used for assembler)*
    * ins: instruction (e.g. "add") of the assembly *(used for assembler)*
    * address: the address of this assembly instruction (starting at 0x40'0000 according to project specs) *(set by assembler, used for simulator)*
    * code: the result of machine code after assembling *(set by assembler, used for simulator)*
  * var_t: data structure to carry the variables defined in assembly *(set by assembler, used for simulator)*
  * REGISTER_SET: used to translate register name into register number
  * INSTRUCT_ENUM: used to translate assembly instruction into numeric index (for uses of switch)


## I/O Specification
Pass arguments in CLI: ./simulator asm_path sys_call_input_path sys_call_output_path

## Testing
Tested on ArchLinux (x86) and Mac OS Catelina (x86). Some problems may arise when compiled with **MinGW** on Windows caused by different size of the type ```long``` in this case you may change it into ```long long```. 

Some samples are provided for testing. As the project does not require to support pseudo-instruction, you may change the pseudo-instructions into non-pseudo instructions if you want to test with your own assembly code, an example is ```li```.

