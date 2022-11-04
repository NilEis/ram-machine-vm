%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <inttypes.h>
    #include <ctype.h>
    #include "print_functions.h"
    int running = 0;
    extern int yylex();
    extern int yyparse();
    extern FILE *yyin;
    FILE* out;
    void yyerror (char const *err);
%}
%define parse.error detailed
%union {
  uint16_t i;
};


%token <i> INT

%token INSTRUCTION_PRINT
%token REGISTER
%token PRINT_PC_LINE
%token PRINT_PC
%token PRINT_PROG
%token INSTRUCTION_STEP
%token INSTRUCTION_GET
%token INSTRUCTION_SET
%token INSTRUCTION_RUN
%token INSTRUCTION_EXIT
%token INSTRUCTION_CLEAR
%token INSTRUCTION_BREAK
%token INSTRUCTION_HELP

%token EQ
%token LEQ
%token GEQ
%token LE
%token GE

%token BRACKET_LEFT
%token BRACKET_RIGHT
%token LINE_SEPARATOR

%type <i> VAL

%%
input: | op input
  ;

op: instruction | LINE_SEPARATOR;

instruction: print | step | run | exit | run_n | get | clear | set | break | help;

print: INSTRUCTION_PRINT REGISTER VAL           { print_register($3, $3);                    } |
       INSTRUCTION_PRINT REGISTER VAL VAL       { print_register($3, $4);                    } |
       INSTRUCTION_PRINT PRINT_PC_LINE VAL      { print_pc_line($3);                         } |
       INSTRUCTION_PRINT PRINT_PC_LINE          { print_current_line();                      } |
       INSTRUCTION_PRINT PRINT_PROG             { print_prog(0,get_prog_size());             } |
       INSTRUCTION_PRINT PRINT_PROG VAL VAL     { print_prog($3,$4);                         } |
       INSTRUCTION_PRINT PRINT_PC               { print_pc();                                };

set: INSTRUCTION_SET REGISTER VAL EQ VAL        { set_register($3, $5);                      };

step: INSTRUCTION_STEP                          { vm_step();                                 };

run: INSTRUCTION_RUN                            { while(vm_step()==0);                       };

run_n: INSTRUCTION_RUN VAL                      { for(int i = 0; i < $2 && !vm_step(); i++); };

exit: INSTRUCTION_EXIT                          { YYACCEPT;                                  };

get: INSTRUCTION_GET                            { printf("Not implemented\n");               };

clear: INSTRUCTION_CLEAR                        { vm_clear();                                };

break: INSTRUCTION_BREAK VAL                    { add_break_point($2);                       };

help: INSTRUCTION_HELP                          { print_help();                              };

VAL:  INT {$$=$1;}
;
%%