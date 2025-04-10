#include "spimcore.h"

// Disclaimer I am merely guessing what needs to happen in this whole thing

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // Somebody peer-check this
    if (ALUControl == 0x0){      // Z = A + B      
        *ALUresult = A + B;
    }
    else if (ALUControl == 0x1){ // Z = A – B
        *ALUresult = A - B;
    }
    else if (ALUControl == 0x2){ // if A < B, Z = 1; otherwise, Z = 0

    }
    else if (ALUControl == 0x3){ // if A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)

    }
    else if (ALUControl == 0x4){ // Z = A AND B
        *ALUresult = A & B;
    }
    else if (ALUControl == 0x5){ // Z = A OR B
        *ALUresult =  A | B;
    }
    else if (ALUControl == 0x6){ // Z = Shift B left by 16 bits
        *ALUresult = B << 16;
    }
    else if (ALUControl == 0x7){ // Z = NOT A
        *ALUresult = ~A;
    }
    
    return;
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{   
    //Somebody peer-check this entire function
    
    if (PC % 4 == 0){ // Check for word alignment(?)
        *instruction = Mem[PC >> 2]; // PC >> 2 to get the actual index
        return 0;
    }
    return 1; // 1 indiciates returning halt flag
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    //Somebody peer-check this function
    
    *op = (instruction >> 26) & ~(~0 << 32 - 26+1); // or it could be: op = (instruction >> 26) & 0x3F; 0x3F being the 27-32 bits and adding to it op variable's range (i think)
    *r1 = (instruction << 21) & ~(~0 << 25-21+1); // Formula: field << (lsb in range) & ~(~0 << msb in range - lsb in range + 1)
    *r2 = (instruction << 16) & ~(~0 << 20-16+1);
    *r3 = (instruction << 11) & ~(~0 << 15-11+1);
    *funct = (instruction << 5) & ~(~0 << 5-0+1);
    *offset = (instruction << 15) & ~(~0 << 15-0+1);
    *jsec =  (instruction << 25) & ~(~0 << 25-0+1);

    return;
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

    switch(op) {
        case 0x0:  // R-type
            controls->RegDst = 1;
            controls->RegWrite = 1;
            controls->ALUOp = 7;
            break;
        case 0x23:  // lw
            controls->MemRead = 1;
            controls->MemtoReg = 1;
            controls->ALUSrc = 1;
            controls->RegWrite = 1;
            break;
        case 0x2B:  // sw
            controls->MemWrite = 1;
            controls->ALUSrc = 1;
            break;
        case 0x4:   // beq
            controls->Branch = 1;
            controls->ALUOp = 1;
            break;
        case 0x2:   // jump
            controls->Jump = 1;
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
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    // check if the 16th bit is 1 (indicates it's negative)
    // if so, make the rest of the bits after the 16th bit 1
    // else, keep the on bits and make the rest 0
    if ((offset & 0x8000) == 0x8000) *extended_value = offset | 0xFFFF0000;
    else *extended_value = offset & 0x0000FFFF;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    if (ALUOp == 7) {  // R-type
        switch(funct) {
            case 0x20: ALU(data1, B, 0, ALUresult, Zero); break;  // add
            case 0x22: ALU(data1, B, 1, ALUresult, Zero); break;  // sub
            case 0x24: ALU(data1, B, 4, ALUresult, Zero); break;  // and
            case 0x25: ALU(data1, B, 5, ALUresult, Zero); break;  // or
            case 0x2A: ALU(data1, B, 2, ALUresult, Zero); break;  // slt
            default: return 1;
        }
    } else {
        ALU(data1, B, ALUOp, ALUresult, Zero);
    }
    
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    if (ALUresult % 4 != 0) return 1;  // Check alignment
    if (ALUresult > 0xFFFF) return 1;  // Check bounds
    
    if (MemRead) {
        *memdata = Mem[ALUresult >> 2];
    }
    if (MemWrite) {
        Mem[ALUresult >> 2] = data2;
    }
    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    if (RegWrite) {
        unsigned write_reg = RegDst ? r3 : r2;
        if (write_reg != 0) {  // Cannot write to $0
            Reg[write_reg] = MemtoReg ? memdata : ALUresult;
        }
    }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    if (Jump) {
        *PC = ((*PC & 0xF0000000) | (jsec << 2));
    } else if (Branch && Zero) {
        *PC += (extended_value << 2);
    }
    // PC+4 is handled in the main simulation loop
}

