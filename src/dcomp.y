%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "asm_types.h"
#include "dcomp_types.h"
#include "trace.h"

int yydebug=0;

extern FILE *yyin;

void yyerror(const char *str)
{
  fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
  return 1;
}

int parse(FILE* in_file)
{
  yyin=in_file;
  TRACE(TRACE_DEBUG, "Invoking yyparse");
  int parse_result = yyparse();
  if (parse_result != 0)
  {
    TRACE(TRACE_ERROR, "Parse failed");
    return parse_result;
  }

  TRACE(TRACE_DEBUG, "Parse successful");
}

int main(int argc, char* args[])
{
  FILE* in_file;
  FILE* out_file;
  TRACE(TRACE_DEBUG, "Program %s started", args[0]);
  if (argc==3 && ((in_file=fopen(args[1], "r"))!=NULL) && ((out_file=fopen(args[2], "w"))!=NULL))
  {
    if (parse(in_file))
    {
      close(in_file);
      close(out_file);
      return 1;
    }
    close(in_file);
    close(out_file);

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

%token TOKEN_ELSE TOKEN_FALSE TOKEN_IF TOKEN_READ TOKEN_SKIP TOKEN_TRUE TOKEN_WHILE TOKEN_WRITE
%token TOKEN_PLUS TOKEN_MINUS TOKEN_ASTERISK TOKEN_SLASH TOKEN_PERCENT TOKEN_EQUALS TOKEN_NOT_EQUALS
%token TOKEN_LESS_THAN_EQUALS TOKEN_GREATER_THAN_EQUALS TOKEN_LESS_THAN
%token TOKEN_GREATER_THAN TOKEN_NOT TOKEN_AND TOKEN_OR TOKEN_ASSIGN TOKEN_SEMICOLON TOKEN_LEFTP
%token TOKEN_RIGHTP

%union
{
  char* string;
  int number;
  INSTRUCTION instruction;
  INSTRUCTION_LIST instruction_list;
}

%token <string> TOKEN_LABEL
%token <number> TOKEN_NUMBER
%type <instruction_list> program

%type <instruction_list> single_instruction
%type <instruction_list> multiple_instructions

%type <instruction> instruction
%type <instruction> simple_instruction
%type <instruction_list> instruction_list
%type <instruction_list> complete_instruction_list
%type <instruction_list> single_instruction_list
%type <instruction_list> single_complete_instruction_list
%type <instruction_list> nested_program
%type <instruction> skip_instruction
%type <instruction> read_instruction
%type <instruction> write_instruction
%type <instruction> assign_instruction
%type <instruction> if_statement
%type <instruction> if_else_statement
%type <instruction> while_statement
%type <instruction> complete_instruction
%type <instruction> complete_if_else_statement
%type <instruction> complete_while_statement

%%

program:
       single_instruction |
       multiple_instructions

/*
       instruction |
       program instruction
*/


single_instruction:
                  instruction
                  {
                    INSTRUCTION_LIST list;
                    list = createInstructionList();
                    if (list == null)
                    {
                      exit(1);
                    }
                    addInstruction(list, $1);
                    $$ = list;
                  }

multiple_instructions:
                     program instruction
                     {
                       INSTRUCTION_LIST list = $1;
                       addInstruction(list, $2);
                       $$ = list;
                     }

instruction:
           simple_instruction |
           if_statement |
           if_else_statement |
           while_statement

simple_instruction:
                  skip_instruction |
                  read_instruction |
                  write_instruction |
                  assign_instruction 

nested_program:
              TOKEN_LEFTP program TOKEN_RIGHTP
              {
                $$ = $2;
              }

skip_instruction:
                TOKEN_SKIP TOKEN_SEMICOLON
                {
                  printf("Skip instruction\n");
                  INSTRUCTION instruction = createSkipInstruction();
                  if (instruction == null)
                  {
                    exit(1);
                  }
                }

read_instruction:
                TOKEN_READ TOKEN_LABEL TOKEN_SEMICOLON
                {
                  printf("Read instruction for label <%s>\n", $2);
                  printf(">> CONST %s\n", $2);
                  printf(">> SWAPA\n");
                  printf(">> READ\n");
                  printf(">> STORE\n");
                  VARIABLE label = createVariable($2);
                  if (label == null)
                  {
                    exit(1);
                  }
                  INSTRUCTION instruction = createReadInstruction(label);
                  if (instruction == null)
                  {
                    exit(1);
                  }
                  $$ = instruction;
                }

write_instruction:
                 TOKEN_WRITE arithmetic_expression TOKEN_SEMICOLON
                 {
                   printf("Write instruction\n");
                   printf(">> WRITE\n");
                   INSTRUCTION instruction = createWriteInstruction($2);
                   if (instruction == null)
                   {
                     exit(1);
                   }
                   $$ = instruction;
                 }

assign_instruction:
                  TOKEN_LABEL TOKEN_ASSIGN arithmetic_expression TOKEN_SEMICOLON
                  {
                    printf("Assign instruction for label <%s>\n", $1);
                    printf(">> SWAPD\n");
                    printf(">> CONST %s\n", $1);
                    printf(">> SWAPA\n");
                    printf(">> SWAPD\n");
                    printf(">> STORE\n");
                    VARIABLE label = createVariable($1);
                    if (label == null)
                    {
                      exit(1);
                    }
                    INSTRUCTION instruction = createAssignInstruction(label, $3);
                    if (instruction == null)
                    {
                      exit(1);
                    }
                    $$ = instruction;
                  }

if_statement:
            TOKEN_IF TOKEN_LEFTP conditional_expression TOKEN_RIGHTP instruction_list
            {
              printf("If instruction\n");
            }

if_else_statement:
                 TOKEN_IF TOKEN_LEFTP conditional_expression TOKEN_RIGHTP complete_instruction_list TOKEN_ELSE instruction_list
                 {
                   printf("If-else instruction\n");
                 }

while_statement:
               TOKEN_WHILE TOKEN_LEFTP conditional_expression TOKEN_RIGHTP instruction_list
               {
                 printf("While instruction\n");
               }

complete_instruction:
                    simple_instruction |
                    complete_if_else_statement |
                    complete_while_statement

complete_if_else_statement:
                          TOKEN_IF TOKEN_LEFTP conditional_expression TOKEN_RIGHTP complete_instruction_list TOKEN_ELSE complete_instruction_list
                          {
                            printf("Complete if-else instruction\n");
                          }

complete_while_statement:
                        TOKEN_WHILE TOKEN_LEFTP conditional_expression TOKEN_RIGHTP complete_instruction_list
                        {
                          printf("Complete while instruction\n");
                        }

instruction_list:
                single_instruction_list |
                nested_program

single_instruction_list:
                       instruction
                       {
                         INSTRUCTION_LIST list;
                         list = createInstructionList();
                         if (list == null)
                         {
                           exit(1);
                         }
                         addInstruction(list, $1);
                         $$ = list;
                       }

complete_instruction_list:
                         single_complete_instruction_list |
                         nested_program

single_complete_instruction_list:
                                complete_instruction
                                {
                                  INSTRUCTION_LIST list;
                                  list = createInstructionList();
                                  if (list == null)
                                  {
                                    exit(1);
                                  }
                                  addInstruction(list, $1);
                                  $$ = list;
                                }                                  

factor:
      factor_label |
      factor_number |
      negative_factor |
      TOKEN_LEFTP arithmetic_expression TOKEN_RIGHTP

factor_label:
            TOKEN_LABEL
            {
              printf("Factor with label <%s> loaded\n", $1);
              printf(">> CONST %s\n", $1);
              printf(">> SWAPA\n");
              printf(">> LOAD\n");
            }

factor_number:
             TOKEN_NUMBER
             {
               printf("Factor with value <%d> loaded\n", $1);
               printf(">> CONST %d\n", $1);
             }

negative_factor:
               TOKEN_MINUS TOKEN_NUMBER
               {
                 printf("Negative factor loaded\n");
                 printf(">> CONST -%d\n", $2);
               }

component:
         factor |
         mul_operation |
         div_operation |
         mod_operation
/*         component multiplicative_operator factor */

mul_operation:
             component TOKEN_ASTERISK factor
             {
               printf("Mul operation loaded\n");
             }

div_operation:
             component TOKEN_SLASH factor
             {
               printf("Div operation loaded\n");
             }

mod_operation:
             component TOKEN_PERCENT factor
             {
               printf("Mod operation loaded\n");
             }


arithmetic_expression:
                     component |
                     add_operation |
                     sub_operation
/*                     arithmetic_expression additive_operator component */

add_operation:
             arithmetic_expression TOKEN_PLUS component
             {
               printf("Add operation loaded\n");
             }

sub_operation:
             arithmetic_expression TOKEN_MINUS component
             {
               printf("Sub operation loaded\n");
             }

relational_expression:
                     arithmetic_expression relational_operator arithmetic_expression |
                     TOKEN_LEFTP conditional_expression TOKEN_RIGHTP |
                     TOKEN_NOT relational_expression |
                     TOKEN_TRUE |
                     TOKEN_FALSE

logical_component:
                 relational_expression |
                 logical_component TOKEN_AND relational_expression

conditional_expression:
                      logical_component |
                      conditional_expression TOKEN_OR logical_component
/*
multiplicative_operator:
                       TOKEN_ASTERISK | TOKEN_SLASH | TOKEN_PERCENT

additive_operator:
                 TOKEN_PLUS | TOKEN_MINUS
*/
relational_operator:
                   TOKEN_EQUALS | TOKEN_NOT_EQUALS | TOKEN_LESS_THAN_EQUALS | TOKEN_GREATER_THAN_EQUALS |
                   TOKEN_LESS_THAN | TOKEN_GREATER_THAN
