#include <stdio.h>
#include "trace.h"

char char_to_hex(char);

int main(int argc, char* args[])
{
  FILE* in_file;
  FILE* out_file;
  TRACE(TRACE_DEBUG, "Program %s started.", args[0]);
  if (argc==3 && ((in_file=fopen(args[1], "r"))!=NULL) && ((out_file=fopen(args[2], "w"))!=NULL))
  {
    unsigned char read[2];
    char counter=0;
    while (fread(read, 1, 2, in_file))
    {
      fprintf(out_file, "%04X", read[0]*256+read[1]);
      if (++counter<8)
      {
        fprintf(out_file, " ");
      }
      else
      {
        fprintf(out_file, "\n");
        counter=0;
      }
    }
    if (counter!=0)
    {
      fprintf(out_file, "\n");
    }
    fclose(in_file);
    fclose(out_file);
    TRACE(TRACE_DEBUG, "Program completed.");
    return 0;
  }
  else
  {
    fprintf(stderr, "Usage: %s <input_file> <output_file>\n", args[0]);
    return 1;
  }
}
