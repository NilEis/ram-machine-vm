%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <inttypes.h>
    #include "bison.tab.h"
    extern int yylex();
    void yyerror (char const *err)
    {
        fprintf (stderr, "Line %d: %s\n", yylineno, err);
    }
%}
%option noyywrap
%option case-insensitive
%option yylineno
DIGIT [0-9]
WHITESPACE [ \t\r]
ALPHABET [a-zA-Z0-9_\\\%:;,. ]
%%

EXIT|QUIT                 {return INSTRUCTION_EXIT;                                                                       }
GET                       {return INSTRUCTION_GET;                                                                        }
STEP                      {return INSTRUCTION_STEP;                                                                       }
RUN                       {return INSTRUCTION_RUN;                                                                        }
PRINT                     {return INSTRUCTION_PRINT;                                                                      }
clear|cls                 {return INSTRUCTION_CLEAR;                                                                      }
REGISTER|REG              {return PRINT_REGISTER;                                                                         }
LINE                      {return PRINT_PC_LINE;                                                                          }
PC                        {return PRINT_PC;                                                                               }
PROGRAM|PROG              {return PRINT_PROG;                                                                             }
\=                        {return EQ;                                                                                     }
\<\=                      {return LEQ;                                                                                    }
\>\=                      {return GEQ;                                                                                    }
\<                        {return LE;                                                                                     }
\>                        {return GE;                                                                                     }
\(                        { return BRACKET_LEFT;                                                                          }
\)                        { return BRACKET_RIGHT;                                                                         }
\n                        { return LINE_SEPARATOR;                                                                        }
{DIGIT}+                  { yylval.i = atoi(yytext); return INT;                                                          }
{WHITESPACE}              ;
.                         ;
%%