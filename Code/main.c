#include <stdio.h>
#include "syntax.tab.h"
#include "ParseTree.h"
#include "ParseStack.h"
#include "SymbolTable.h"
#include "Code.h"

void yyrestart(FILE *input_file);
int yyparse();

extern int yycolumn;
extern int yylineno;
extern int yydebug;
extern int A_error, B_error;
extern TreeNode *root;
extern bool struct_defining;
extern void printProgramInstrs(Codes *head, char *dest_file);

void init() {
	initStack();
	initFieldStack();
	initTable();
	initCodes();

	root = NULL;
	A_error = 0;
	B_error = 0;
	struct_defining = false;
	newFuncFrame();
}

int main(int argc, char** argv) {
	if(argc <= 1) return 1;
	FILE* f = fopen(argv[1], "r");
	if(!f) {
		perror(argv[1]);
		return 1;
	}
	//init
	init();

	if(A_error == 1) return 0;

	// parse
	yyrestart(f);
	yylineno = 1;
	yyparse();

	//print parser tree
	/*if(A_error != 1 && B_error != 1 && root != NULL) {
		preTraversal(root, 0);
    }*/

    //free nodes
    freeTree(root);

    // print codes
    printProgramCodes(codes_head);

    // print instructions
    printProgramInstrs(codes_head, argv[2]);

    // global field
	leaveField();

    printf("\n");
	return 0;
}
