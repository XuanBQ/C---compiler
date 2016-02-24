#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "common.h"
#include "TypeInfo.h"
#include "ParseTree.h"

typedef struct ItemInfo_ ItemInfo;
struct ItemInfo_ {
	Type id_type; // id's type
	Type return_type;
	char name[NAME_LENGTH];
	Operand *tempVar;
};

typedef struct TableItem_ TableItem;
struct TableItem_ {
	ItemInfo item_info;

	int field;  

	TableItem* last; // head's last is null
	TableItem* next; // tail's next is null

	TableItem* field_next; // next symbol in the current field
};

#define TABLE_LENGTH 200
TableItem Table[TABLE_LENGTH]; // heads, to link symbols with 'last' 'next' pointer

void initTable();

// if the name exists, return false
bool checkRepeat(unsigned index, TokenInfo* token_info);

ItemInfo* checkDefine(TreeNode* id);

TableItem* fillIn(TreeNode* symbol);

Operand* getIdTempVar(TreeNode* id);
void setIdTempVar(TreeNode* id, Operand* s);
char* getIdName(TreeNode* func);

void print_table();
void print_table_slot(int index);

/* field stack */
#define FIELD_STACK_LENGTH 100

TableItem FieldStack[FIELD_STACK_LENGTH]; // heads, to link symbols with 'next' pointer

void initFieldStack();

// push
// enter a new field
void enterField();

// pop
// leave current field
void leaveField();

// return currentField
int getCurrentField();
// set currentField f
void setCurrentField(int f);

#endif

