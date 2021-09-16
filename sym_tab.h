#ifndef SYM_TAB_H
#define SYM_TAB_H

typedef struct _symtab_el {
	char *label;
	int addr;
} symtab_el;

extern symtab_el *sym_tab;	// The symbol table: a vector of couples (label, addr)
extern int sym_tab_size;	// The symbol table vector size

extern int cur_addr;
extern unsigned *program_code;

int find_sym (char *label);
void add_sym (char *label, int addr);
void set_addr (char *label, int addr);

#endif /* SYM_TAB_H */
