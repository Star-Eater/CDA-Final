#include "spimcore.h"
// $ gcc -o spimcore spimcore.c project.c
// $ ./spimcore <filename>.asc


/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    switch (ALUControl) {

        // Addition: Z = A + B 
        case 0x0: 
            *ALUresult = A + B;
            break;

        // Subtraction: Z = A – B
        case 0x1: 
            *ALUresult = A - B;
            break;

        // Set Less Than: if A < B, Z = 1; else, Z = 0
        case 0x2: 
            if ((int)A < (int)B) *ALUresult = 1;
            else *ALUresult = 0;
            break;

        // Set Less Than Unsigned: if A < B, Z = 1; else, Z = 0 (A and B are unsigned ints)
        case 0x3: 
            if (A < B) *ALUresult = 1;
            else *ALUresult = 0;
            break;

        // AND: Z = A AND B
        case 0x4: 
            *ALUresult = A & B;
            break;

        // OR: Z = A OR B
        case 0x5: 
            *ALUresult = A | B;
            break;

        // Shift Left 16: Z = Shift B 16 bits to the left
        case 0x6: // Shift Left 16
            *ALUresult = B<<16;
            break;

        // NOT: Z = NOT A
        case 0x7: 
            *ALUresult = ~A;
            break;
        
        default:
            exit(1);
    }

    if (*ALUresult == 0) *Zero = 1;
    else *Zero = 0;
    
}


/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    // returns 0 if word is aligned correctly
    // is a multiple of 4
    if (PC % 4 == 0) {
        *instruction = Mem[PC>>2];
        return 0;
    }
   
    // returns 1 if halt condition occurs - jumps too an address that is not word-aligned 
    // not a multiple of 4
    return 1;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{   
    // instruction [31-26] = 1111 1100 0000 0000 0000 0000 0000 0000
    *op = (instruction & 0xFC000000)>>26;

    // instruction [25-21] = 0000 0011 1110 0000 0000 0000 0000 0000
    *r1 = (instruction & 0x03E00000)>>21; 

    // instruction [20-16] = 0000 0000 0001 1111 0000 0000 0000 0000
    *r2 = (instruction & 0x001F0000)>>16; 

    // instruction [15-11] = 0000 0000 0000 0000 1111 1000 0000 0000
    *r3 = (instruction & 0x0000F800)>>11; 

    // instruction [5-0] = 0000 0000 0000 0000 0000 0000 0011 1111
    *funct = (instruction & 0x0000003F)>>0; 

    // instruction [15-0] = 0000 0000 0000 0000 1111 1111 1111 1111
    *offset = (instruction & 0x0000FFFF)>>0; 

    // instruction [25-0] = 0000 0011 1111 1111 1111 1111 1111 1111
    *jsec = (instruction & 0x03FFFFFF)>>0; 
}


/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;

    // add (add)
    // subtract (sub) 
    // add immediate (addi) 
    // and (and) 
    // or (or) 
    // load word (lw) 
    // store word (sw) 
    // load upper immedidate (lui) 
    // branch on equal (beq) 
    // set on less than (slt) 
    // set less than immediate (slti) 
    // set less than unsigned (sltu) 
    // set less than immediate unsigned (sltiu)
    // jump (j) 

    switch(op) {

        // R-type: add, sub, and, or, slt, sltu
        case 0x0: 
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUOp = 7;
            break;

        // addi
        case 0x8: 
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        // lw
        case 0x23:  
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        // sw
        case 0x2B:  
            controls->ALUOp = 1;
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            controls->RegDst = 2;
            break;

        // lui
        case 0xF: 
            controls->ALUOp = 6;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        // beq
        case 0x4:   
            controls->Branch = 1;
            controls->ALUOp = 1;
            controls->RegDst = 2;
            controls->MemtoReg = 2;
            break;

        // slti
        case 0xA:  
            controls->ALUOp = 7;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        // sltiu
        case 0xB:  
            controls->ALUOp = 7;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;

        // jump
        case 0x2:   
            controls->Jump = 1;
            controls->RegDst = 2;
            controls->MemtoReg = 2;
            controls->ALUOp = 2;
            controls->ALUSrc = 2;
            break;

        default:
            return 1;
    }

    return 0;
}


/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    // read the register addressed at r1 and assign the value to data1
    *data1 = Reg[r1];

    // read the register addressed at r2 and assign the value to data2
    *data2 = Reg[r2];

}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    if ((offset & 0x8000)) *extended_value = offset | 0xFFFF0000;
    else *extended_value = offset & 0x0000FFFF;   
}


/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{ 
    unsigned B;
    if (ALUSrc) B = extended_value;
    else B = data2;

    if (ALUOp < 0 || ALUOp > 7) return 1;
    
    // R-type
    if (ALUOp == 7) {  
        
        switch(funct) {

            // add
            case 0x20: 
                ALU(data1, B, 0, ALUresult, Zero); 
                break;  
            
            // sub
            case 0x22: 
                ALU(data1, B, 1, ALUresult, Zero); 
                break;  
            
            // and
            case 0x24: 
                ALU(data1, B, 4, ALUresult, Zero); 
                break;  
            
            // or
            case 0x25: 
                ALU(data1, B, 5, ALUresult, Zero); 
                break; 
                
            // slt
            case 0x2A: 
                ALU(data1, B, 2, ALUresult, Zero); 
                break;  
            
            // sltu    
            case 0x2B: 
                ALU(data1, B, 3, ALUresult, Zero); 
                break;  

            default: 
                return 1;
        }
    } 
     
    else ALU(data1, B, ALUOp, ALUresult, Zero);
     
    return 0;
}


/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // assign data2 to memory with ALUresult
    if (MemWrite){
        if(ALUresult % 4 == 0) Mem[ALUresult>>2] = data2;
        else return 1;
    }

    // read to memdata the content of memory with ALUresult 
    if (MemRead){
        if(ALUresult % 4 == 0) *memdata = Mem[ALUresult >> 2];
        else return 1;
    }

    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    unsigned write_reg;
    if (RegWrite) {
        if (RegDst) write_reg = r3;
        else write_reg = r2;

        if (write_reg != 0) {
            if (MemtoReg) Reg[write_reg] = memdata;
            else Reg[write_reg] = ALUresult;
        }
    }
}


/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    *PC += 4;
    
    if (Branch && Zero) *PC += (extended_value<<2);

    if (Jump) *PC = ((*PC & 0xF0000000) | (jsec<<2));
}
