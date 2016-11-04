//
//  decode.cpp
//  RISCV_ISA_SIM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#include "decode.hpp"

bool instruction::getType(){
    opcode = inst&OPCODE;
    switch(opcode)
    {
    case 0x33: // b0110011
        optype =  R_TYPE;
            return true;
        
    case 0x67: // b1100111
    case 0x03: // b0000011
    case 0x13: // b0010011
    case 0x73: // b1110011
        optype =  I_TYPE;
            return true;
        
    case 0x23: // b0100011
        optype =  S_TYPE;
            return true;
        
    case 0x63: // b1100011
        optype =  SB_TYPE;
            return true;
        
    case 0x37: // b0110111
    case 0x17: // b0010111
        optype =  U_TYPE;
            return true;
        
    case 0x6F: // b1101111
        optype =  UJ_TYPE;
            return true;
        
    default:
            return false;
    }
}
