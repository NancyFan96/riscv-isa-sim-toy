//
//  system.h
//  RISCV_ISA_SIM
//
//  THIS IS RISCV R64IFM
//
//  Created by Nancy Fan on 16/11/4.
//  Copyright © 2016年 Nancy Fan. All rights reserved.
//

#ifndef system_h
#define system_h

#include <stdio.h>

#define RRXLEN 64		// the rrx registers are all 64 bits wide
const float FDIFF = 0.00001;
const int HOW_MANY_INSTS = 120;


typedef unsigned long int   dword;
typedef unsigned int	 	word;
typedef unsigned short	 	halfword;
typedef unsigned char       byte;

typedef unsigned long int	reg64;
typedef unsigned int	 	reg32;
typedef unsigned short int  reg16;
typedef unsigned char       reg8;
typedef unsigned char       regID;

typedef float	f32;
typedef double	f64;

typedef signed long int     signed64;
typedef signed int          signed32;
typedef signed short int    signed16;
typedef signed char         signed8;

typedef unsigned int        ins;
typedef signed long int     imm;
typedef unsigned char       xcode;
typedef unsigned char       insType;
typedef unsigned long int   memAddress;


#endif /* system_h */
