#include "SymbolTable.h"
#include "Code.h"
#include "TokenId.h"

static int currentField;

extern bool struct_defining;
//#define __DEBUG_SYMBOLTABLE__

void addRead() {
	TreeNode *read_func = createNode();
	TokenInfo *info = &(read_func->token_info);

    editTokenInfo(info, ID, -1, (void *)("read"));

    // no argument
    SymbolList* head = createListNode();
    editFuncType(&(info->id_type), head);

    ItemInfo *item = fillIn(read_func);
    // return type
    Type return_type;
    editBasicType(&return_type, INTEGER);
	copyType( &(item->return_type), &return_type );
}
void addWrite() {
	TreeNode *write_func = createNode();
	TokenInfo *info = &(write_func->token_info);

    editTokenInfo(info, ID, -1, (void *)("write"));

    // an int argument
    SymbolList* head = createListNode();
    Type arg_t;
    editBasicType(&arg_t, INTEGER);
    addListNode(head, "write_arg", &arg_t);
    editFuncType(&(info->id_type), head);

    ItemInfo *item = fillIn(write_func);

    // return type
    Type return_type;
    editBasicType(&return_type, INTEGER);
	copyType( &(item->return_type), &return_type );
}

/* set the 'last' and 'next' of Table's heads null */
void initTable() { 
	int i;
	for(i = 0; i < TABLE_LENGTH; i ++) {
		Table[i].last = Table[i].next = NULL;
		Table[i].field_next = NULL;
	}
    addRead();
    addWrite();
}

/* set the 'next' of FieldStack's heads null */
void initFieldStack() {
    currentField = 0;
	
	int i;
	for(i = 0; i < FIELD_STACK_LENGTH; i ++) {
		FieldStack[i].field_next = NULL;
	}
}

static unsigned getIndex(char* name) {
	unsigned i, index = 0;
	for(; *name; ++name) {
		index = (index << 2) + *name;
		if(i = index & ~TABLE_LENGTH) 
			index = (index ^ (i >> 12)) & TABLE_LENGTH;
	}

	return index;
}

// if the symbol is defined, report error and return false
bool checkRepeat(unsigned index, TokenInfo* token_info) {
	char *name = token_info->str;

	// cheak the symbol chain
	TableItem *cur_item = Table[index].next;
	while(cur_item != NULL) {
		ItemInfo* cur_info = &(cur_item->item_info);
		char *cur_name = cur_info->name;
		// compare name
		if(strcmp(cur_name, name) == 0) {
			// they are in the same field
			// or one is STRUCTURE
			if(cur_item->field == currentField || checkIfStructTag(&(cur_info->id_type)) ) {
				// report error
				IDKind k = (token_info->id_type).kind;
				if(k == FUNCTION) {
					printf("error type 4 at line %d: redefined function '%s'\n", 
							token_info->line, name);
				}
				else if(k == STRUCTURE || k == STRUCTTAG) {
					printf("error type 16 at line %d: dulplicated name '%s'\n",
							token_info->line, name);
				}
				else if(struct_defining == true) {
					printf("error type 15 at line %d: redefined field '%s'\n",
						   token_info->line, name);
				}
				else {
					printf("error type 3 at line %d: redefined variable '%s'\n",
							token_info->line, name);
				}
				return false;
			}

			break; // find a same name, then no need to continue
		}

		// next
		cur_item = cur_item->next;
	}

	return true;
}
					
// link a new Item to current Field
static void linkStackItem(TableItem* item) {
	item->field = currentField;

	item->field_next = FieldStack[currentField].field_next;
	FieldStack[currentField].field_next = item;
}

// if the id is not defined, return NULL
// else return the ItemInfo*
ItemInfo* checkDefine(TreeNode* id) {
	TokenInfo* id_info = &(id->token_info); 
	char *name = id_info->str;

	// get the position in Table of the name
	unsigned index = getIndex(name);

	// search the symbol chain
	TableItem *cur_item = Table[index].next;
	while(cur_item != NULL) {
		ItemInfo* iteminfo = &(cur_item->item_info);
		char *cur_name = iteminfo->name;

		// compare name
		if(strcmp(cur_name, name) == 0) return iteminfo;
		
		cur_item = cur_item->next;
	}

	return NULL;
}

// new a TableItem and init
static TableItem* createItem() {
	TableItem* p = (TableItem*)malloc(sizeof(TableItem));
	p->item_info.tempVar = NULL;
	return p;
}

static void editItemInfo(ItemInfo* item_info, TokenInfo* token_info) {
	Type *id_type = &(token_info->id_type);
	char* name = token_info->str;

	// copy name
	strcpy(item_info->name, name);

	//copy Type
	copyType(&(item_info->id_type), id_type);
}

TableItem* fillIn(TreeNode* symbol) {
	// get the token_info of the TreeNode
	 TokenInfo* token_info = &(symbol->token_info);
	 char* name = token_info->str;

	 // dulplication checking
	 unsigned index = getIndex(name);
	 if(checkRepeat(index, token_info) == false) return NULL;

	 // build a new TableItem
	 TableItem* item = createItem();
	 editItemInfo(&(item->item_info), token_info);

	 // insert to Table
	 TableItem* temp = Table[index].next;
	 Table[index].next = item;
	 item->last = &(Table[index]);
	 item->next = temp;
	 if(temp != NULL) {
		 temp->last = item;
	 }

	 // link to current field
	 linkStackItem(item);

	 return item;
}

Operand* getIdTempVar(TreeNode* id) {
	ItemInfo *p = checkDefine(id); // id must be there
	Operand *v = p->tempVar;
	return v;
}

void setIdTempVar(TreeNode* id, Operand* s) {
	ItemInfo *p = checkDefine(id);
	p->tempVar = s;
}

static void print_item(ItemInfo* info);

char* getIdName(TreeNode* func) {
	ItemInfo *p = checkDefine(func);
	return p->name;
}

static void removeAItem(TableItem* item) {
	if(item == NULL) {
		printf("cannot remove NULL!\n");
		return;
	}

	item->last->next = item->next;
	if(item->next != NULL) {
		item->next->last = item->last;
	}

	freeType(&(item->item_info.id_type));
	
	free(item);
}

void enterField() {
	currentField ++;
}

void leaveField() {
	TableItem* item = FieldStack[currentField].field_next;
	while(item != NULL) {
		TableItem* temp = item;
		item = temp->field_next;

		removeAItem(temp);
	}
	FieldStack[currentField].field_next = NULL;
	currentField --;
}

// return currentField--
int getCurrentField() {
	return currentField;
}
// set currentField f
void setCurrentField(int f) {
	currentField = f;
}

static void print_item(ItemInfo* info) {
	printf("\nA Item begin: \n");
    printf("name: %s \n", info->name);
	printf("Type: ");
	print_type( &(info->id_type) );
	printf("return_type: ");
	print_type( &(info->return_type) );
	printf("\nA item end.\n");
}

void print_table() {
	printf("\nprinting table:\n");

	int i;
	for(i = 0; i < TABLE_LENGTH; i ++) {
		print_table_slot(i);
	}
}

void print_table_slot(int index) {
	TableItem* item = Table[index].next;
	while(item != NULL) {
		print_item( &(item->item_info) );
		item = item->next;
	}
}



	

