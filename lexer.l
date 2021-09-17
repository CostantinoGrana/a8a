%{

#if defined(WIN32)  || defined(_WIN32) 
	#define fileno _fileno
#else
	#define _strdup strdup
#endif

#include <stdlib.h>
#include <string.h>
#include "parser.h"
void   yyerror(const char *);

char string_buf[255];
char *string_buf_ptr;

%}

%option noyywrap
%option pointer
%option yylineno

/* This defines the exclusive condition ASCII, to handle sequences of literals */
%x ASCII	

%%

";"[^\n]*	/* Ignore assembly comments */
[ \n\r\t]+	/* Ignore whitespace */

ADD			{ yylval.val = 0x40; return INSTRUCTION1; }
ADDi		{ yylval.val = 0x42; return INSTRUCTION1; }
ADDs		{ yylval.val = 0x43; return INSTRUCTION1; }
ADDsi		{ yylval.val = 0x44; return INSTRUCTION1; }
ADDv		{ yylval.val = 0x41; return INSTRUCTION1; }
AND			{ yylval.val = 0x20; return INSTRUCTION1; }
ANDi		{ yylval.val = 0x22; return INSTRUCTION1; }
ANDs		{ yylval.val = 0x23; return INSTRUCTION1; }
ANDsi		{ yylval.val = 0x24; return INSTRUCTION1; }
ANDv		{ yylval.val = 0x21; return INSTRUCTION1; }
CALL		{ yylval.val = 0x82; return INSTRUCTION1; }
CMP			{ yylval.val = 0x60; return INSTRUCTION1; }
CMPi		{ yylval.val = 0x62; return INSTRUCTION1; }
CMPs		{ yylval.val = 0x63; return INSTRUCTION1; }
CMPsi		{ yylval.val = 0x64; return INSTRUCTION1; }
CMPv		{ yylval.val = 0x61; return INSTRUCTION1; }
JC			{ yylval.val = 0x94; return INSTRUCTION1; }
JE			{ yylval.val = 0x91; return INSTRUCTION1; }
JLs			{ yylval.val = 0x95; return INSTRUCTION1; }
JLu			{ yylval.val = 0x96; return INSTRUCTION1; }
JMP			{ yylval.val = 0x90; return INSTRUCTION1; }
JN			{ yylval.val = 0x92; return INSTRUCTION1; }
JO			{ yylval.val = 0x93; return INSTRUCTION1; }
JZ			{ yylval.val = 0x91; return INSTRUCTION1; }
LA			{ yylval.val = 0x01; return INSTRUCTION1; }
LAs			{ yylval.val = 0x86; return INSTRUCTION1; }
LD			{ yylval.val = 0x00; return INSTRUCTION1; }
LDi			{ yylval.val = 0x02; return INSTRUCTION1; }
LDs			{ yylval.val = 0x03; return INSTRUCTION1; }
LDsi		{ yylval.val = 0x04; return INSTRUCTION1; }
LDv			{ yylval.val = 0x01; return INSTRUCTION1; }
NEG			{ yylval.val = 0x70; return INSTRUCTION0; }
NOT			{ yylval.val = 0x71; return INSTRUCTION0; }
OR			{ yylval.val = 0x30; return INSTRUCTION1; }
ORi			{ yylval.val = 0x32; return INSTRUCTION1; }
ORs			{ yylval.val = 0x33; return INSTRUCTION1; }
ORsi		{ yylval.val = 0x34; return INSTRUCTION1; }
ORv			{ yylval.val = 0x31; return INSTRUCTION1; }
POP			{ yylval.val = 0x81; return INSTRUCTION0; }
PUSH		{ yylval.val = 0x80; return INSTRUCTION0; }
REL			{ yylval.val = 0x85; return INSTRUCTION1; }
RES			{ yylval.val = 0x84; return INSTRUCTION1; }
RET			{ yylval.val = 0x83; return INSTRUCTION0; }
SETv		{ yylval.val = 0x01; return INSTRUCTION1; }
ST			{ yylval.val = 0x10; return INSTRUCTION1; }
STi			{ yylval.val = 0x12; return INSTRUCTION1; }
STs			{ yylval.val = 0x13; return INSTRUCTION1; }
STsi		{ yylval.val = 0x14; return INSTRUCTION1; }
SUB			{ yylval.val = 0x50; return INSTRUCTION1; }
SUBi		{ yylval.val = 0x52; return INSTRUCTION1; }
SUBs		{ yylval.val = 0x53; return INSTRUCTION1; }
SUBsi		{ yylval.val = 0x54; return INSTRUCTION1; }
SUBv		{ yylval.val = 0x51; return INSTRUCTION1; }

[_a-zA-Z][_a-zA-Z0-9]* { yylval.str = _strdup(yytext); return IDENTIFIER; }

(\+|\-)?[0-9]+ { yylval.val = atoi(yytext); return NUMBER_DEC; }

(\+|\-)?[0-9][0-9a-fA-F]*h {
	if (yytext[0]=='-')
		yylval.val = -strtol(yytext+1,NULL,16); 
	else
		yylval.val = strtol(yytext,NULL,16); 
	return NUMBER_HEX; 
	}

":"		{ return COLON; }

'			{	/* Begin of ASCII sequence */
				BEGIN(ASCII);
				string_buf_ptr = string_buf;
			}
<ASCII>''	{ /* Escape of ASCII sequence terminator symbol */
				*string_buf_ptr++ = '\'';
			}
<ASCII>'	{ /* End of ASCII sequence */
				BEGIN(INITIAL);
				*string_buf_ptr++ = '\0';
				yylval.str = _strdup(string_buf); 
				return ASCII_CHARS;
			}
<ASCII>.	{ /* Get next char in ASCII sequence */
				*string_buf_ptr++ = *yytext;
			}

.		{ yyerror("Unrecognized token"); }

%%

void yyerror(const char * message)
{
	printf("%s at line %d: %s\n", message, yylineno, yytext);
	exit(EXIT_FAILURE);
}

