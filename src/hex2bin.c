#include <stdio.h>
#include "trace.h"

int main(int argc, char* args[])
{
  FILE* in_file;
  FILE* out_file;
  TRACE(TRACE_DEBUG, "Program %s started", args[0]);
  if (argc==3 && ((in_file=fopen(args[1], "r"))!=NULL) && ((out_file=fopen(args[2], "w"))!=NULL))
  {
    unsigned int word;
    unsigned char buffer[2];
    while (fscanf(in_file, "%04X", &word)!=EOF)
    {
      buffer[0]=(word & 0xFF00) >> 8;
      buffer[1]=word & 0x00FF;
        fwrite(&buffer, 1, 2, out_file);
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
