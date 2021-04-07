# Mipsel Simulator
A year-two course project of Computer Architecture (CSC3050) at CUHK-Shenzhen.
This is a project of assembler and simulator combined, specifc for assembly language based on MIPS little-endian architecture.
The project assemble the codes, simulate the main memory with a 6MB dynamically allocated memory and execute the machine codes.
At the time the project is written, the course has not yet covered the topic of coprocessor or atomic lock, therefore related instructions are ignored.
It is written mainly in procedural paradigm, and for the ease of testing it is written in a single cpp file, a **highly discourged** practice in common cases, and it is recommended to encapuslated the procedures into different classes and libraries in a more OOP way.
Tested on ArchLinux (x86) and Mac OS Catelina (x86). Some problems may arise when compiled with **MinGW** on Windows caused by different size of the type ```long``` in this case you may change it into ```long long```.

## I/O Specification
Pass arguments in CLI: ./simulator asm_path sys_call_input_path sys_call_output_path

## Testing
Some samples are provided for testing. As the project does not require to support pseudo-instruction, you may change the pseudo-instructions into non-pseudo instructions if you want to test with your own assembly code, an example is ```li```.

