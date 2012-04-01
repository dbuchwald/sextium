#include <stdio.h>
#include "trace.h"
#define _SEXTIUM_MEMORY_SIZE 64*1024

typedef unsigned short int word;

int main (int argc, char* args[])
{
  FILE* input_file;
  if (argc==2 && ((input_file=fopen(args[1], "rt"))!=NULL))
  {
    TRACE(TRACE_INFO, "Program %s started", args[0]);
    word memory[_SEXTIUM_MEMORY_SIZE];
    word ACC, DR, AR, PC, BR, Temp;
    unsigned char current_command;
    unsigned char halt_reason=0;
    unsigned char input_file_buffer[2]; 
    int address=0;  
    while (fread(input_file_buffer, 1, 2, input_file))
    {
      memory[address]=input_file_buffer[0]*256+input_file_buffer[1];
      address++;
      if (address>=_SEXTIUM_MEMORY_SIZE)
      {
        TRACE(TRACE_ERROR, "Program too large for memory");
        return 1;
      }
    }
    fclose(input_file);
    TRACE(TRACE_INFO, "Program %s (size %#06X) loaded", args[1], address);
    ACC=0;
    DR=0;
    AR=0;
    PC=0;
    BR=0;
    while (!halt_reason)
    {
      TRACE(TRACE_DEBUG, "Registers ACC:[%#06X|%d] DR:[%#06X|%d] AR:[%#06X] PC:[%#06X] BR:[%#06X]", ACC, ACC, DR, DR, AR, PC, BR);
      
      if (BR == 0)
      {
        TRACE(TRACE_DEBUG, "Loading new commands: [%#06X]", memory[PC]);
        BR=memory[PC++];
      }

      current_command=(BR & 0xF000) >> 12;
      BR = (BR << 4) & 0xFFF0;

      switch (current_command)
      {
      case 1:  /* LOAD */
        TRACE(TRACE_DEBUG, "Executing LOAD ADDR:[%#06X] VAL:[%#06X|%d]", AR, memory[AR], memory[AR]);
        ACC=memory[AR];
        break;
      case 2:  /* STORE */
        TRACE(TRACE_DEBUG, "Executing STORE ADDR:[%#06X] VAL:[%#06X|%d] OLDVAL:[%#06X|%d]", AR, ACC, ACC, memory[AR], memory[AR]);
        memory[AR] = ACC;
        break;
      case 3:  /* READ */
        fscanf(stdin, "%hi", &ACC);
        TRACE(TRACE_DEBUG, "Executed READ VAL:[%#06X|%d]", ACC, ACC);
        break;        
      case 4:  /* WRITE */
        fprintf(stdout, "%i\n", ACC);
        TRACE(TRACE_DEBUG, "Executed WRITE VAL:[%#06X|%d]", ACC, ACC);
        break;
      case 5:  /* SWAPA */
        TRACE(TRACE_DEBUG, "Executing SWAPA ACC:[%#06X|%d] AR:[%#06X|%d]", ACC, ACC, AR, AR);
        Temp = ACC;
        ACC = AR;
        AR = Temp;
        break;
      case 6:  /* SWAPD */
        TRACE(TRACE_DEBUG, "Executing SWAPD ACC:[%#06X|%d] DR:[%#06X|%d]", ACC, ACC, DR, DR);
        Temp = ACC;
        Temp = ACC;
        ACC = DR;
        DR = Temp;
        break;
      case 7:  /* JUMP */
        TRACE(TRACE_DEBUG, "Executing JUMP ACC:[%#06X]", ACC);
        BR = 0;
        PC = ACC;
        break;
      case 8:  /* BRANCHZ */
        TRACE(TRACE_DEBUG, "Executing BRANCHZ ACC:[%#06X|%d] AR:[%#06X]", ACC, ACC, AR);
        if (ACC == 0)
        {
          BR = 0;
          PC = AR;
        }
        break;
      case 9:  /* BRANCHP */
        TRACE(TRACE_DEBUG, "Executing BRANCHP ACC:[%#06X|%d] AR:[%#06X]", ACC, ACC, AR);
        if (ACC > 0)
        {
          BR = 0;
          PC = AR;
        }
        break;
      case 10: /* CONST */
        TRACE(TRACE_DEBUG, "Executing CONST VAL:[%#06X|%d]", memory[PC], memory[PC]);
        ACC = memory[PC++];
        break;
      case 11: /* ADD */
        TRACE(TRACE_DEBUG, "Executing ADD ACC:[%#06X|%d] DR:[%#06X|%d]", ACC, ACC, DR, DR);
        ACC += DR;
        break;
      case 12: /* SUB */
        TRACE(TRACE_DEBUG, "Executing SUB ACC:[%#06X|%d] DR:[%#06X|%d]", ACC, ACC, DR, DR);
        ACC -= DR;
        break;
      case 13: /* MUL */
        TRACE(TRACE_DEBUG, "Executing MUL ACC:[%#06X|%d] DR:[%#06X|%d]", ACC, ACC, DR, DR);
        ACC *= DR;
        break;
      case 14: /* DIV */
        TRACE(TRACE_DEBUG, "Executing DIV ACC:[%#06X|%d] DR:[%#06X|%d]", ACC, ACC, DR, DR);
        if (DR == 0) 
        {
          halt_reason=2;
        }
        else
        {
          ACC = ACC/DR;
        }
        break;
      case 15: /* HALT */
        TRACE(TRACE_DEBUG, "Executing HALT");
        halt_reason=1;
        break;
      }
    }
    if (halt_reason==1)
    {
      TRACE(TRACE_INFO, "Program execution completed successfully");
      return 0;
    }
    else if (halt_reason==2) 
    {
      TRACE(TRACE_ERROR, "Abnormal program termination- divide by zero.");
      return 1;
    } 
  }
  else 
  {
    TRACE(TRACE_ERROR, "Usage: %s <input_file>", args[0]);
    return 1;
  }
}

