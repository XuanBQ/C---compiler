#include "DefineId.h"
#include "TokenId.h"
#include "CheckExpErrors.h"
#include "common.h"
//#define __DEBUG__DEFINEID__

// about the topVarDec's child id
// if type not NULL, give the id id_type and fill in Table and return id
// if type NULL, the id's already in the table , here just return the id
TreeNode* fillVarDec(Type* type, TreeNode* top_var) {
	// find the ID, and fill it in Table
	TreeNode* node = top_var->firstChild;
	TreeNode* lowest_var = top_var; // find the lowest VarDec(ID's parent node)
	while((node->token_info).id != ID) {
		lowest_var = node;
		node = node -> firstChild;
	}
    
    // type is NULL, return id
    if(type == NULL) return node;

	int dim = (top_var->token_info).int_value;

    // finally decide the id's type
    Type *top_var_type = &((top_var->token_info).id_type);
	if(dim > 0) { // array
		// element type
		Type* lowest_var_type = &((lowest_var->token_info).id_type);
        copyType(lowest_var_type, type);
	}
	else copyType(top_var_type, type);

	// the id's type
	copyType(&((node->token_info).id_type), top_var_type);

	if(fillIn(node) == NULL) return NULL;
	else return node; 

}
		
static void linkField(TreeNode* declist, Type* type_info, SymbolList* head) {
	if(declist == NULL) return;

	TreeNode* node = declist; 
	TokenInfo* info = &(node->token_info);

	// top VarDec(s)
	if(info->id == VARDEC) {
		TreeNode* id = fillVarDec(type_info, node);
        if(id == NULL) return;

		TokenInfo* id_info = &(id->token_info); 
		char* id_name = id_info->str;
		Type* id_t = &(id_info->id_type);

		// checking error
		// about Dec -> VarDec ASSIGNOP Exp
		TreeNode* assignop = node->nextSibling;
		if( assignop != NULL ) { // has assignop
		   printf("error type 15 at line %d: initialize variables in struct\n", 
		   	       (assignop->token_info).line);
		}
		
		// add a field info to FieldList 
		if(id != NULL) addListNode(head, id_name, id_t);

		return;
	}
	
    // visiting children
	linkField(node->firstChild, type_info, head);
	linkField(node->nextSibling, type_info, head);
}

// DefList in " StructSpecifier -> STRUCT OptTag LC DefList RC "
void buildFieldList(TreeNode* def_list, SymbolList* head) {
	if(def_list == NULL) return;
	
	TreeNode* node = def_list;
	TokenInfo* info = &(node->token_info);

	// the first 'DecList' of def_list's children has Type
	// the first 'DecList' is SPECIFIER's first brother
	if(info->id == SPECIFIER) {
		TreeNode* declist = node->nextSibling;
		Type* typeinfo = &((declist->token_info).id_type); 

		// to link fields
		linkField(node->nextSibling, typeinfo, head); // let DecList complete the task
		return;
	}
	else if(info->id == NOTHING) return; // empty

	// visiting children
	buildFieldList(node -> firstChild, head);
	buildFieldList(node -> nextSibling, head);
}

// define ID:  OptTag -> ID | NOTHING
bool fillOptTag(TreeNode* opt_tag) {
	TreeNode* node = opt_tag->firstChild;
	TokenInfo* info = &(node->token_info);

	if(info->id != ID) return true; // NOTHING

	copyType(&(info->id_type), &((opt_tag->token_info).id_type));
	if( fillIn(node) == NULL) return false;
	else return true;
}

// get struct ID's FieldList from Table
// if not exists, return NULL
SymbolList* getFieldList(TreeNode* tag) {
	TokenInfo* tag_info = &(tag->token_info);
	TreeNode* id = tag->firstChild;
	TokenInfo* id_info = &(id->token_info);

	ItemInfo* p = checkDefine(id);

	if(p == NULL) return NULL;
	else {
	    // tag type
        copyType(&(tag_info->id_type), &(p->id_type));
		return (p->id_type).list;
	}
}

void fillExtDecList(TreeNode* ext_dec_list) {

	fillDecList(ext_dec_list, &((ext_dec_list->token_info).id_type));

}

static void linkArg(TreeNode* var_dec, SymbolList* head) { 
	TreeNode* node = var_dec; 

	TreeNode* id = fillVarDec(NULL, node); // id already has type_info
	TokenInfo id_info = id->token_info; 
	char* id_name = id_info.str;
	Type* id_t = &(id_info.id_type);

	// add a field info to FieldList 
	if(id != NULL) addListNode(head, id_name, id_t);
}

static void buildArgsList(TreeNode* var_list, SymbolList* head) {
	if(var_list == NULL) return;

	TreeNode* node = var_list;
	TokenInfo* info = &(node->token_info);

	// ParamDec -> Specifier VarDec
	if(info->id == SPECIFIER) {
		TreeNode* var_dec = node->nextSibling;

		// don't fill it in Table 
		linkArg(var_dec, head); 
		return;
	}

	// visiting children
	buildArgsList(node -> firstChild, head);
	buildArgsList(node -> nextSibling, head);
}

void checkReturn(TreeNode* compst, Type* t) {
	if(compst == NULL) return;

    // Stmt -> RETURN Exp SEMI
	if(compst->token_info.id == RETURN) {
	   TreeNode* exp = compst->nextSibling;
       TokenInfo* exp_info = &(exp->token_info);
       Type* exp_type = &(exp_info->id_type);

       // compare
       if( compareType(exp_type, t) == false ) {
       	 printf("error type 8 at line %d: type mismatched for return\n",
       	 	    exp_info->line);
       	 // don't return, there maybe some returns
       }
       return;
	}

	checkReturn(compst->firstChild, t);
	checkReturn(compst->nextSibling, t);
}

// define function and build its args list
// id in: FunDec -> ID LP VarList RP | ID LP RP
// and id with its return_type
void fillFunDec(TreeNode* id, TreeNode* var_list) {
	// get id
    Type* func_type =  &(id->token_info.id_type);
	
	// return_type
	Type return_type;
	copyType( &return_type, func_type ); 

	// build args list (if exists)
	SymbolList* head = createListNode();
	if(var_list != NULL) { // has var_list
	      buildArgsList(var_list, head);
	}

	// id type
	editFuncType(&(id->token_info.id_type), head);

    // fill in Table
    int arg_field = getCurrentField();
    // fill it in the previous field
    setCurrentField(arg_field - 1);
	TableItem* item = fillIn(id);
	setCurrentField(arg_field);

	if( item == NULL) {
		freeList(head);
		return;
	}
	else {
		// return type
		copyType( &(item->item_info.return_type), &return_type );
	}
}

// define variables (if struct_defining = false)
// ids under DecList or ExtDecList
void fillDecList(TreeNode* dec_list, Type* type_info) {
	if(dec_list == NULL) return;
	
	TreeNode* node = dec_list; 
	TokenInfo* info = &(node->token_info);

	// top VarDec(s)
	if(info->id == VARDEC) {
		TreeNode* id = fillVarDec(type_info, node);
		if(id == NULL) return;
		
		// checking errors
		// about Dec -> VarDec ASSIGNOP Exp
        TreeNode* assignop = node->nextSibling;
		if( assignop != NULL ) { // has assignop
		   TreeNode* exp = assignop->nextSibling;
		   id->token_info.int_value = CAN_ASSIGN;
		   checkExpOpExp(exp, id, NULL, ASSIGNOP);
		}
		
		return;
	}
	
    // visiting children
	fillDecList(node->firstChild, type_info);
	fillDecList(node->nextSibling, type_info);
}

// this function isn't used
// CompSt -> LC DefList StmtList RC 
// define ids under DefList 
void fillDefList(TreeNode* def_list) {
	if(def_list == NULL) return;

	TreeNode* node = def_list;
	TokenInfo* info = &(node->token_info);

	// the first 'DecList' of def_list's children has Type, and
	// the first 'DecList' is SPECIFIER's first brother
	if(info->id == SPECIFIER) {
		TreeNode* declist = node->nextSibling;
		Type* typeinfo = &((declist->token_info).id_type); 

		// fill ids in Table
		fillDecList(node->nextSibling, typeinfo); // let DecList complete the task
		return;
	}
	else if(info->id == NOTHING) return; // empty

	// visiting children
	fillDefList(node -> firstChild);
	fillDefList(node -> nextSibling);
}
