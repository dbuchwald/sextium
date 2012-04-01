%{
#include <stdio.h>
#include "asm_types.h"
#include "sextium_asm.tab.h"
%}

%%

NOP      return TOKEN_NOP;
LOAD     return TOKEN_LOAD;
STORE    return TOKEN_STORE;
READ     return TOKEN_READ;
WRITE    return TOKEN_WRITE;
SWAPA    return TOKEN_SWAPA;
SWAPD    return TOKEN_SWAPD;
JUMP     return TOKEN_JUMP;
BRANCHZ  return TOKEN_BRANCHZ;
BRANCHP  return TOKEN_BRANCHP;
CONST    return TOKEN_CONST;
ADD      return TOKEN_ADD;
SUB      return TOKEN_SUB;
MUL      return TOKEN_MUL;
DIV      return TOKEN_DIV;
HALT     return TOKEN_HALT;
DATA     return TOKEN_DATA;
:        return TOKEN_COLON;
[a-zA-Z][a-zA-Z0-9]* yylval.string = strdup(yytext); return TOKEN_LABEL;
[0-9]+               yylval.number = atoi(yytext); return TOKEN_NUMBER;
[ \t]+
\n
#.*$
