#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if (defined(__APPLE__) || defined(__MACH__))
#include <malloc/malloc.h>
#else
#include <malloc.h>
#endif

#include "sym_tab.h"

FILE* yyin;
int yyparse(void);

int verbose = 0;

void syntax()
{
	printf("\n");
	printf("SYNTAX: a8a [<options>] <input file name>\n");
	printf("\n");
	printf("Options:\n");
	printf("/v\tVerbose parser output\n");
	printf("\n");
	printf("If no error is found, three output files are created:\n");
	printf("logisim.txt    - Logisim format ready for memory loading.\n");
	printf("hexdecimal.txt - Hexdecimal format for serial text mode transmission.\n");
	printf("memory.bin     - Binary file with the machine code as in memory.\n");
}

int main(int argc, char** argv)
{
	int argi = 0;
	int missing = 0;

	printf("A8A - ADE8 Assembly - Version 1.0\n\n");

	if (argc < 2) {
		printf("Error: insufficient parameters\n");
		syntax();
		return -1;
	}

	/* Options */
	argi = 1;
	while (argi < argc && argv[argi][0] == '/') {
		if (strcmp(argv[argi], "/v") == 0)
			verbose = 1;
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

	program_code = (unsigned*)malloc(cur_addr * sizeof(unsigned));

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

	/* LogiSim output*/
	{
		FILE* f;
		int i;

		/* This was used to allow setting the name of the output file name.
		argi++;
		if (argi<argc)
			f = fopen (argv[argi],"wt");
		else*/
		f = fopen("logisim.txt", "wt");
		if (f == NULL) {
			printf("Error while creating output file logisim.txt\n");
			return 1;
		}

		fprintf(f, "v2.0 raw\n");
		for (i = 0; i < cur_addr; i++) {
			fprintf(f, i % 16 == 15 ? "%02x\n" : "%02x ", program_code[i] & 0xff);
		}

		fclose(f);
	}

	/* Hexdecimal output*/
	{
		FILE* f;
		int i;

		f = fopen("hexdecimal.txt", "wt");
		if (f == NULL) {
			printf("Error while creating output file hexdecimal.txt\n");
			return 1;
		}

		for (i = 0; i < cur_addr; i++) {
			fprintf(f, i % 16 == 15 ? "0x%02x\n" : "0x%02x ", program_code[i] & 0xff);
		}

		fclose(f);
	}

	/* Binary output*/
	{
		FILE* f;
		int i;

		f = fopen("memory.bin", "wb");
		if (f == NULL) {
			printf("Error while creating output file memory.bin\n");
			return 1;
		}

		for (i = 0; i < cur_addr; i++) {
			fputc(program_code[i] & 0xff, f);
		}

		fclose(f);
	}

	return 0;
}