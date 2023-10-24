%{

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <malloc.h>

#define YYPTRDIFF_T ptrdiff_t

#include "sym_tab.h"

extern int verbose;

int yylex (void);
void yyerror (const char *message);
extern int yylineno;

%}

%union {
unsigned	val;
char		*str;
}

%token <val>	INSTRUCTION0 INSTRUCTION1
%token <str>	IDENTIFIER ASCII_CHARS
%token <val>	NUMBER_DEC NUMBER_HEX
%token			COLON

%type <val>		number 
%type <str>		label_def

%start program

%%

program
	: stmt
	| program stmt
	;

stmt
	: label_def			{
							int pos = find_sym($1);
							if (!program_code && pos >= 0 && sym_tab[pos].addr != -1) {
								fprintf(stderr, "ERROR: duplicated label definition '%s' at line %d\n", sym_tab[pos].label, yylineno);
								exit(EXIT_FAILURE);
							}

							set_addr ($1,cur_addr);
							if (verbose) {
								printf ("Label definition: %s\n", $1); 
							}
						}
	| IDENTIFIER		{
							int pos = find_sym($1);
							int addr = -1;
							if (pos < 0) {
								add_sym($1,addr);
							}
							else {
								addr = sym_tab[pos].addr;
							}

							if (verbose) {
								printf ("0x%02x - Label_use: %s, pos=%d, addr=0x%02x\n",cur_addr,$1,pos,addr); 
							}

							if (program_code) {
								program_code[cur_addr] = addr;
							}
							cur_addr++;
						}
	| INSTRUCTION0		{
							if (verbose) {
								printf ("0x%02x - Instruction without parameters: 0x%02x\n",cur_addr,$1); 
							}

							if (program_code) {
								program_code[cur_addr] = $1;
								program_code[cur_addr+1] = 0;
							}
							cur_addr += 2;
						}
	| INSTRUCTION1		{
							if (verbose) {
								printf ("0x%02x - Instruction with 1 parameter: 0x%02x\n",cur_addr,$1);
							}

							if (program_code) {
								program_code[cur_addr] = $1;
							}
							cur_addr++;
						}
	| number			{
							if (verbose) {
								printf ("0x%02x - Number: %d\n",cur_addr,$1); 
							}

							if (program_code) {
								program_code[cur_addr] = $1;
							}
							cur_addr++;
						}
	| ASCII_CHARS		{
							char *sequence = $1;

							if (verbose) {
								printf ("0x%02x - ASCII Sequence: %s\n",cur_addr,sequence);
							}

							if (program_code) {
								size_t i;
								for (i = 0; i < strlen(sequence); ++i)
									program_code[cur_addr + i] = sequence[i];
							}
							cur_addr += (int)strlen(sequence);
							free(sequence);
						}
	;
	
label_def
	: IDENTIFIER COLON	{ $$ = $1; }
	;

number
	: NUMBER_DEC		{ $$ = $1; }
	| NUMBER_HEX		{ $$ = $1; }
	;
	
%%