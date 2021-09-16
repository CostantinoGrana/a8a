#include "sym_tab.h"
#include <string.h>
#include <stdlib.h>

symtab_el* sym_tab = 0;
int sym_tab_size = 0;

int cur_addr = 0;
unsigned* program_code = 0;

int find_sym(char* label)
{
	int i;
	for (i = 0; i < sym_tab_size; i++) {
		if (strcmp(sym_tab[i].label, label) == 0) {
			return i;
		}
	}
	return -1;
}

void add_sym(char* label, int addr)
{
	sym_tab_size++;
	sym_tab = realloc(sym_tab, sym_tab_size * sizeof(symtab_el));
	sym_tab[sym_tab_size - 1].label = label;
	sym_tab[sym_tab_size - 1].addr = addr;
}

void set_addr(char* label, int addr)
{
	int pos = find_sym(label);
	if (pos < 0)
		add_sym(label, addr);
	else
		sym_tab[pos].addr = addr;
}
