%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "asm_types.h"
#include "trace.h"

int yydebug=0;

extern FILE *yyin;

void yyerror(COMMAND_LIST output_list, const char *str)
{
  fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
  return 1;
}

void printCommand(const COMMAND command)
{
               printf("Command ");
               switch (command->type)
               {
                 case COMMAND_NOP:
                   printf("NOP");
                   break;
                 case COMMAND_LOAD:
                   printf("LOAD");
                   break;
                 case COMMAND_STORE:
                   printf("STORE");
                   break;
                 case COMMAND_READ:
                   printf("READ");
                   break;
                 case COMMAND_WRITE:
                   printf("WRITE");
                   break;
                 case COMMAND_SWAPA:
                   printf("SWAPA");
                   break;
                 case COMMAND_SWAPD:
                   printf("SWAPD");
                   break;
                 case COMMAND_JUMP:
                   printf("JUMP");
                   break;
                 case COMMAND_BRANCHZ:
                   printf("BRANCHZ");
                   break;
                 case COMMAND_BRANCHP:
                   printf("BRANCHP");
                   break;
                 case COMMAND_CONST:
                   printf("CONST");
                   break;
                 case COMMAND_ADD:
                   printf("ADD");
                   break;
                 case COMMAND_SUB:
                   printf("SUB");
                   break;
                 case COMMAND_MUL:
                   printf("MUL");
                   break;
                 case COMMAND_DIV:
                   printf("DIV");
                   break;
                 case COMMAND_HALT:
                   printf("HALT");
                   break;
                 case COMMAND_DATA:
                   printf("DATA");
                   break;
               }
               if (command->label != null)
               {
                 printf(" with label %s", getLabelString(command->label));
                 if (getLabelValue(command->label)!=-1)
                 {
                   printf(" (value: %d|%#06x)", getLabelValue(getCommandLabel(command)), getLabelValue(getCommandLabel(command)));
                 }
               }
               if (command->parameter != null)
               {
                 printf(" with parameter");
                 if (command->parameter->type == PARAMETER_VALUE)
                 {
                   printf(" value %d|%#06x", command->parameter->value, command->parameter->value);
                 }
                 else
                 {
                   printf(" label %s", getLabelString(command->parameter->label));
                   if (getLabelValue(getParameterLabel(getCommandParameter(command)))!=-1)
                   {
                     printf(" (value: %d|%#06x)", getLabelValue(getParameterLabel(getCommandParameter(command))),
                                                  getLabelValue(getParameterLabel(getCommandParameter(command))));
                   }
                 }
               }
               printf(" loaded\n");
}

int parse(FILE* in_file, COMMAND_LIST command_list)
{
  yyin=in_file;
  TRACE(TRACE_DEBUG, "Invoking yyparse");
  int parse_result = yyparse(command_list);
  if (parse_result != 0)
  {
    TRACE(TRACE_ERROR, "Parse failed");
    return parse_result;
  }

  TRACE(TRACE_DEBUG, "Parse successful");
  COMMAND_LIST_ITERATOR iterator=getIterator(command_list);
  int address=0;
  int address_offset=0;

  iterator=getIterator(command_list);
  while (iterator!=null)
  {
    if (getCommandLabel(getCommand(iterator))!=null)
    {
      setLabelValue(getCommandLabel(getCommand(iterator)), address);
    } 
    if (getCommandType(getCommand(iterator))==COMMAND_CONST)
    {
      address++;
    }
    if (getNext(iterator)!=null &&
        (getCommandLabel(getCommand(getNext(iterator)))!=null || getCommandType(getCommand(getNext(iterator)))==COMMAND_DATA) &&
        (address_offset+1)<4)
    {
      int temp_offset=address_offset+1;
      while (temp_offset<4)
      {
        COMMAND nop_command = createCommand(null, COMMAND_NOP, null);
        if (nop_command == null)
        {
          exit(1);
        }
        insertCommandAfter(command_list, iterator, nop_command);
        temp_offset++;
      }
    }
    iterator=getNext(iterator);
    address_offset++;
    if (address_offset>=4)
    {
      address_offset=0;
      address++;
    }
  }
  while ((address_offset % 4) != 0)
  {
    COMMAND nop_command = createCommand(null, COMMAND_NOP, null);
    if (nop_command == null)
    {
      exit(1);
    }
    addCommand(command_list, nop_command);
    address_offset++;
  }
}

static void writeWord(FILE* out_file, unsigned short int word)
{
  TRACE(TRACE_DEBUG, "Writing to output stream word [%04X]", word);
  char buffer[2];
  buffer[0]=(word & 0xFF00) >> 8;
  buffer[1]=word & 0x00FF;
  fwrite(&buffer, 1, 2, out_file);
}

void generateOutput(FILE* out_file, COMMAND_LIST command_list)
{
  COMMAND_LIST_ITERATOR iterator=getIterator(command_list);
  int address_offset=0;
  int const_counter=0;
  char buffer[5];
  PARAMETER_VALUE_TYPE const_buffer[4];

  buffer[4]=0;

  while(iterator!=null)
  {
    if (getCommandType(getCommand(iterator))==COMMAND_CONST)
    {
      const_buffer[const_counter++]=getParameterValue(getCommandParameter(getCommand(iterator)));
    }
    buffer[address_offset]=getCommandRepresentation(getCommandType(getCommand(iterator)));
    iterator=getNext(iterator);
    address_offset++;
    if (address_offset==4)
    {
      unsigned short int word;
      sscanf(buffer, "%04hX", &word);
      writeWord(out_file, word);
      int i;
      for (i=0; i<const_counter; i++)
      {
        writeWord(out_file, const_buffer[i]);
      }
      const_counter=0;
      address_offset=0;
    }
  }
}

int main(int argc, char* args[])
{
  FILE* in_file;
  FILE* out_file;
  TRACE(TRACE_DEBUG, "Program %s started", args[0]);
  if (argc==3 && ((in_file=fopen(args[1], "r"))!=NULL) && ((out_file=fopen(args[2], "w"))!=NULL))
  {
    COMMAND_LIST command_list = null;
    command_list = createCommandList();
    if (command_list == null)
    {
      return 1;
    }
    if (parse(in_file, command_list))
    {
      close(in_file);
      close(out_file);
      return 1;
    }
    generateOutput(out_file, command_list);
    close(in_file);
    close(out_file);
    destroyLabelPool();
    destroyCommandList(&command_list);
   
    TRACE(TRACE_DEBUG, "Program completed.");
    return 0;
  }
  else
  {
    fprintf(stderr, "Usage: %s <input_file> <output_file>\n", args[0]);
    return 1;
  }
}

%}

%token TOKEN_NOP TOKEN_LOAD TOKEN_STORE TOKEN_READ TOKEN_WRITE TOKEN_SWAPA TOKEN_SWAPD TOKEN_JUMP
%token TOKEN_BRANCHZ TOKEN_BRANCHP TOKEN_CONST TOKEN_ADD TOKEN_SUB TOKEN_MUL TOKEN_DIV TOKEN_HALT
%token TOKEN_DATA TOKEN_COLON

%union
{
  char* string;
  int number;
  LABEL_TYPE label;
  PARAMETER parameter;
  COMMAND command;
  COMMAND_LIST command_list;
}

%token <string> TOKEN_LABEL
%token <number> TOKEN_NUMBER
%type <parameter> parameter
%type <parameter> label_parameter
%type <parameter> value_parameter
%type <command_list> commands
%type <command> command_entry
%type <command> label_command
%type <command> command
%type <command> command_nop
%type <command> command_load
%type <command> command_store
%type <command> command_read
%type <command> command_write
%type <command> command_swapa
%type <command> command_swapd
%type <command> command_jump
%type <command> command_branchz
%type <command> command_branchp
%type <command> command_const
%type <command> command_add
%type <command> command_sub
%type <command> command_mul
%type <command> command_div
%type <command> command_halt
%type <command> command_data

%parse-param {COMMAND_LIST output_list}

%%

program:
       |
       commands

commands:
        commands command_entry
             {
               COMMAND_LIST list = $1;
               addCommand(list, $2);
               $$=list;
             }

commands:
        command_entry
             {
               addCommand(output_list, $1);
               $$ = output_list;
             }

command_entry:
             label_command | command

command:
       command_nop | command_load | command_store | command_read | command_write |
       command_swapa | command_swapd | command_jump | command_branchz | command_branchp |
       command_const | command_add | command_sub | command_mul | command_div | command_halt |
       command_data

label_command:
             TOKEN_LABEL TOKEN_COLON command
             {
               LABEL_TYPE label = createLabel($1);
               if (label == null)
               {
                 exit(1);
               }
               COMMAND result = $3;
               setCommandLabel(result, label);
               $$ = result;
             }

command_nop:
             TOKEN_NOP
             {
               COMMAND result = createCommand(null, COMMAND_NOP, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_load:
             TOKEN_LOAD
             {
               COMMAND result = createCommand(null, COMMAND_LOAD, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_store:
             TOKEN_STORE
             {
               COMMAND result = createCommand(null, COMMAND_STORE, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_read:
             TOKEN_READ
             {
               COMMAND result = createCommand(null, COMMAND_READ, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_write:
             TOKEN_WRITE
             {
               COMMAND result = createCommand(null, COMMAND_WRITE, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_swapa:
             TOKEN_SWAPA
             {
               COMMAND result = createCommand(null, COMMAND_SWAPA, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_swapd:
             TOKEN_SWAPD
             {
               COMMAND result = createCommand(null, COMMAND_SWAPD, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_jump:
             TOKEN_JUMP
             {
               COMMAND result = createCommand(null, COMMAND_JUMP, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_branchz:
             TOKEN_BRANCHZ
             {
               COMMAND result = createCommand(null, COMMAND_BRANCHZ, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_branchp:
             TOKEN_BRANCHP
             {
               COMMAND result = createCommand(null, COMMAND_BRANCHP, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_const:
             TOKEN_CONST parameter
             {
               COMMAND result = createCommand(null, COMMAND_CONST, $2);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

parameter:
             label_parameter | value_parameter

label_parameter:
             TOKEN_LABEL
             {
               LABEL_TYPE label = createLabel($1);
               if (label == null)
               {
                 exit(1);
               }
               PARAMETER result = createLabelParameter(label);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

value_parameter:
             TOKEN_NUMBER
             {
               PARAMETER result = createValueParameter($1);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_add:
             TOKEN_ADD
             {
               COMMAND result = createCommand(null, COMMAND_ADD, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_sub:
             TOKEN_SUB
             {
               COMMAND result = createCommand(null, COMMAND_SUB, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_mul:
             TOKEN_MUL
             {
               COMMAND result = createCommand(null, COMMAND_MUL, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_div:
             TOKEN_DIV
             {
               COMMAND result = createCommand(null, COMMAND_DIV, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_halt:
             TOKEN_HALT
             {
               COMMAND result = createCommand(null, COMMAND_HALT, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

command_data:
             TOKEN_DATA
             {
               COMMAND result = createCommand(null, COMMAND_DATA, null);
               if (result == null)
               {
                 exit(1);
               }
               $$ = result;
             }

