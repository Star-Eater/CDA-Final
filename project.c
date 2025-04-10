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

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    if(ALUOp == 0x0){ // Addition is performed
        // Signed or not signed
    }    
    else if(ALUOp == 0x1){ // Subtraction is performed
        // Signed or not signed
    }
    else if(ALUOp == 0x2){ // SLT is performed

    }
    else if (ALUOp == 0x3){ //SLT is performed but unsigned

    }
    else if (ALUOp == 0x4){ // And is performed

    }
    else if (ALUOp == 0x5){ // OR is performed

    }
    else if (ALUOp == 0x6){ // Shift left is performed

    }
    else if (ALUOp == 0x7){ // NOT(?) is performed

    }
    else{
        return 1; // Halt flag
    }
    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    // read the register addressed at r1 and assign the value to data1
    *data1 = Reg[r1];

    // read the register addressed at r2 and assign the value to data2
    *data2 = Reg[r2];

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    // check if the 16th bit is 1 (indicates it's negative)
    // if so, make the rest of the bits after the 16th bit 1
    // else, keep the on bits and make the rest 0
    if ((offset & 0x8000) == 0x8000) *extended_value = offset | 0xFFFF0000;
    else *extended_value = offset & 0x0000FFFF;
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{

}

