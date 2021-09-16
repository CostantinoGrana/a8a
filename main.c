#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//#if (defined(__APPLE__) || defined(__MACH__))
//#include <malloc/malloc.h>
//#else
//#include <malloc.h>
//#endif

#include "sym_tab.h"

FILE* yyin;
int yyparse(void);

bool verbose = false;

void syntax()
{
	printf("\n");
	printf("SYNTAX: a8a [<options>] <input file name>\n");
	printf("\n");
	printf("Options:\n");
	printf("/v\tVerbose parser output\n");
	printf("/h\tGenerate hexadecimal format output file\n");
	printf("/b\tGenerate binary format output file\n");
	printf("\n");
	printf("If no error is found, the following files are created:\n");
	printf("logisim.txt     - Logisim format ready for memory loading\n");
	printf("hexadecimal.txt - Hexadecimal format for serial text mode transmission (if /h is specified)\n");
	printf("memory.bin      - Binary file with machine code as it would be in memory (if /b is specified)\n");
}

int main(int argc, char** argv)
{
	int argi = 1;
	int missing = 0;

	printf("A8A - ADE8 Assembly - Version 1.1\n\n");

	if (argc < 2) {
		printf("Error: insufficient parameters\n");
		syntax();
		return -1;
	}

	/* Options */
	bool hexadecimal = false, binary = false;
	while (argi < argc && argv[argi][0] == '/') {
		if (strcmp(argv[argi], "/v") == 0) {
			verbose = true;
		}
		else if (strcmp(argv[argi], "/h") == 0) {
			hexadecimal = true;
		}
		else if (strcmp(argv[argi], "/b") == 0) {
			binary = true;
		}
		else {
			printf("Error: unknown option %s\n", argv[argi]);
			syntax();
			return -1;
		}
		argi++;
	}

	yyin = fopen(argv[argi], "rb");
	if (!yyin) {
		printf("Error: cannot open file \"%s\"\n", argv[1]);
		return -1;
	}

	yyparse();
	fclose(yyin);

	printf("\n* Symbol table *\n");
	{
		int i;
		for (i = 0; i < sym_tab_size; i++) {
			printf("0x%02x\t%s\n", sym_tab[i].addr, sym_tab[i].label);
			if (sym_tab[i].addr == -1)
				missing = 1;
		}
	}
	printf("\n");

	if (missing) {
		printf("Error: missing label definitions\n");
		return -1;
	}

	program_code = malloc(cur_addr * sizeof(unsigned int));

	cur_addr = 0;
	yyin = fopen(argv[argi], "rb");
	yyparse();
	fclose(yyin);

	printf("\n* Program code *\n");
	{
		int i;
		for (i = 0; i < cur_addr; i++) {
			printf(i % 16 == 15 ? "%02x\n" : "%02x ", program_code[i] & 0xff);
		}
	}
	printf("\n");

	/* LogiSim output */
	{
		FILE* f = fopen("logisim.txt", "w");
		if (f == NULL) {
			printf("Error while creating output file logisim.txt\n");
			return 1;
		}
		fprintf(f, "v2.0 raw\n");
		for (int i = 0; i < cur_addr; i++) {
			fprintf(f, i % 16 == 15 ? "%02x\n" : "%02x ", program_code[i] & 0xff);
		}
		fclose(f);
	}

	/* Hexadecimal output */
	if (hexadecimal) {
		FILE* f;
		f = fopen("hexadecimal.txt", "w");
		if (f == NULL) {
			printf("Error while creating output file hexdecimal.txt\n");
			return 1;
		}
		for (int i = 0; i < cur_addr; i++) {
			fprintf(f, i % 16 == 15 ? "0x%02x\n" : "0x%02x ", program_code[i] & 0xff);
		}
		fclose(f);
	}

	/* Binary output */
	if (binary) {
		FILE* f;
		f = fopen("memory.bin", "wb");
		if (f == NULL) {
			printf("Error while creating output file memory.bin\n");
			return 1;
		}
		for (int i = 0; i < cur_addr; i++) {
			fputc(program_code[i] & 0xff, f);
		}
		fclose(f);
	}

	return 0;
}