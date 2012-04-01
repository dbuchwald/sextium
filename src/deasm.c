#include <stdio.h>
#include "trace.h"
#define _SEXTIUM_MEMORY_SIZE 64*1024

typedef unsigned short int word;

int main (int argc, char* args[])
{
  FILE* input_file;
  FILE* output_file;
  if (argc==3 && ((input_file=fopen(args[1], "rt"))!=NULL) && ((output_file=fopen(args[2], "w"))!=NULL))
  {
    TRACE(TRACE_INFO, "Program %s started", args[0]);
    unsigned char current_command;
    word current_word;
    word memory[_SEXTIUM_MEMORY_SIZE];
    unsigned short int const_count=0;
    unsigned char input_file_buffer[2]; 
    unsigned short int address=0; 
    unsigned short int program_size;
    while (fread(input_file_buffer, 1, 2, input_file))
    {
      memory[address++] = input_file_buffer[0]*256+input_file_buffer[1];
      if (address>=_SEXTIUM_MEMORY_SIZE)
      {
        TRACE(TRACE_ERROR, "Program too large for memory");
        return 1;
      }
    }
    fclose(input_file);

    program_size=address;
    address=0;
    while (address<program_size)
    {
      fprintf(output_file, "# %04X:\n", address);
      current_word = memory[address];
      if (current_word == 0)
      {
        fprintf(output_file, "       DATA\n");
      }
      else
      {
        int i;
        for (i=0; i<4; i++)
        {
          current_command=(current_word & 0xF000) >> 12;
          current_word=(current_word << 4) & 0xFFF0;
          fprintf(output_file, "       ");
          switch (current_command)
          {
            case 0:
              fprintf(output_file, "NOP");
              break;
            case 1:
              fprintf(output_file, "LOAD");
              break;
            case 2:
              fprintf(output_file, "STORE");
              break;
            case 3:
              fprintf(output_file, "READ");
              break;
            case 4:
              fprintf(output_file, "WRITE");
              break;
            case 5:
              fprintf(output_file, "SWAPA");
              break;
            case 6:
              fprintf(output_file, "SWAPD");
              break;
            case 7:
              fprintf(output_file, "JUMP");
              break;
            case 8:
              fprintf(output_file, "BRANCHZ");
              break;
            case 9:
              fprintf(output_file, "BRANCHP");
              break;
            case 10:
              address++;
              fprintf(output_file, "CONST %d # %04X", memory[address], memory[address]);
              break;
            case 11:
              fprintf(output_file, "ADD");
              break;
            case 12:
              fprintf(output_file, "SUB");
              break;
            case 13:
              fprintf(output_file, "MUL");
              break;
            case 14:
              fprintf(output_file, "DIV");
              break;
            case 15:
              fprintf(output_file, "HALT");
              break;
          }
          fprintf(output_file, "\n");
        }
      }
      address++;
    }
    close(input_file);
    close(output_file);
      
    TRACE(TRACE_INFO, "Program execution completed successfully");
    return 0;
  }
  else 
  {
    TRACE(TRACE_ERROR, "Usage: %s <input_file>", args[0]);
    return 1;
  }
}

