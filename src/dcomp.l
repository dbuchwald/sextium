%{
#include <stdio.h>
#include "asm_types.h"
#include "dcomp.tab.h"
%}

%s COMMENT

%%

<INITIAL>{
else                  return TOKEN_ELSE;
false                 return TOKEN_FALSE;
if                    return TOKEN_IF;
read                  return TOKEN_READ;
skip                  return TOKEN_SKIP;
true                  return TOKEN_TRUE;
while                 return TOKEN_WHILE;
write                 return TOKEN_WRITE;
"+"                   return TOKEN_PLUS;
"-"                   return TOKEN_MINUS;
"*"                   return TOKEN_ASTERISK;
"/"                   return TOKEN_SLASH;
"%"                   return TOKEN_PERCENT;
"=="                  return TOKEN_EQUALS;
"!="                  return TOKEN_NOT_EQUALS;
"<="                  return TOKEN_LESS_THAN_EQUALS;
">="                  return TOKEN_GREATER_THAN_EQUALS;
"<"                   return TOKEN_LESS_THAN;
">"                   return TOKEN_GREATER_THAN;
"!"                   return TOKEN_NOT;
"&&"                  return TOKEN_AND;
"||"                  return TOKEN_OR;
"="                   return TOKEN_ASSIGN;
";"                   return TOKEN_SEMICOLON;
"("                   return TOKEN_LEFTP;
")"                   return TOKEN_RIGHTP;
[a-zA-Z][a-zA-Z0-9_]* yylval.string = strdup(yytext); return TOKEN_LABEL;
[0-9]+                yylval.number = atoi(yytext); return TOKEN_NUMBER;
[ \t]+                /* skip whitespace */
\n                    /* skip newlines */
  "/*"                BEGIN(COMMENT);  /* skip comments */
}

<COMMENT>{                           
  "*/"                                BEGIN(INITIAL);
  [^*\n]                              ;
  "*"[^/]                             ;
  \n                                  ;
}

