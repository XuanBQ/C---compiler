#include "SymbolTable.h"
#include "TypeInfo.h"
#include "CheckExpErrors.h"
#include "TokenId.h"
#include "common.h"
//#define __CHECKEXPERRORS_DEBUG__

static int ifCanAssign(Type* t) {
    if( checkIfFunc(t) == true ) 
    	return CANT_ASSIGN;
    else if( checkIfStructTag(t) == true )
    	return CANT_ASSIGN;
    else 
    	return CAN_ASSIGN;
}

// Exp -> Exp DOT ID
// check if 'DOT' valid
// ID get its type
bool checkDot(TreeNode* exp, TreeNode* id, TreeNode* parent) {
	Type* exp_type = &(exp->token_info.id_type);
	char* id_name = id->token_info.str;
    
    // check if struct
    if( checkIfStruct(exp_type) == false ) {
       printf("error type 13 at line %d: Illegal use of '.'\n", 
       	      exp->token_info.line );
       return false;
    }

	// check if the id in the struct  
	SymbolList* p = getListNode(exp_type->list, id_name); 
	if(p == NULL) { 
		printf("error type 14 at line %d: '%s' isn't a member of the struct\n", 
			    exp->token_info.line, id_name);
		return false;
	}
	else {
		// copy the field type to id
		Type* id_type = &(id->token_info.id_type);
		copyType(id_type, &(p->type) );
		copyType(&(parent->token_info.id_type), id_type );;
        parent->token_info.int_value = ifCanAssign(id_type);
		return true;
	}
}

// Exp -> Exp LB Exp RB
// if valid, type parent
bool checkLbRb(TreeNode* arr_exp, TreeNode* int_exp, TreeNode* parent) {
	// check if int_exp is INTEGER
	if(checkIfInt(&(int_exp->token_info.id_type)) == false) {
		printf("error type 12 at line %d: the number in '[]' should be an integer\n", 
				int_exp->token_info.line); 
		return false;
	}

	Type* arr_type = &(arr_exp->token_info.id_type);
	Type* elm_type = arr_type->array.elem;
	Type* parent_type = &(parent->token_info.id_type);

	// check if exp is Array
	int d = getDim(arr_type);
	if(d == 1) { 
		copyType(parent_type, elm_type);
		parent->token_info.int_value = CAN_ASSIGN;
		return true;
	}
	else if(d > 1) {
		// about parent's type 
	    copyType( parent_type, elm_type);
		return true;
	}
	else { // not array
		printf("error type 10 at line %d: Illegal use of '[ ]'\n",
		       arr_exp->token_info.line); 
		return false;
	}
}

void buildExpArgs(TreeNode* args, SymbolList* head) {
	if(args == NULL) return;

	TokenInfo* info = &(args->token_info);
	if(info->id == EXP) { // top Exp
		SymbolList* node = createListNode();
		addListNode(head, "no_name", &(info->id_type) );
		// Args -> Exp COMMA Args
		// other args are Exp's brothers
		buildExpArgs(args->nextSibling, head); 
		return;
	}

    // Args -> Exp COMMA Args | Exp
    buildExpArgs(args->nextSibling, head); // COMMA's nextSibling
	buildExpArgs(args->firstChild, head);  // Args's firstChild
}

bool checkArgs(SymbolList* head, TreeNode* args) {
	if(args == NULL) {
		if(head->next != NULL) {
			DEBUG_INFO(printf("no arg"));
			printf("errror type 9 at line %d: wrong arguments\n", args->token_info.line);
			return false;
		}
		else return true;
	}

	// link args
	SymbolList* args_head = createListNode();
	buildExpArgs(args, args_head);
	// compare
	SymbolList* arg0 = head;
	SymbolList* arg1 = args_head;

	while(arg0 != NULL && arg1 != NULL) {
		arg0 = arg0 -> next;
		arg1 = arg1 -> next;
		  
		if(arg0 == NULL && arg1 == NULL) return true;

		else if((arg0 == NULL && arg1 != NULL) 
		   || (arg0 != NULL && arg1 == NULL) ) {
			printf("errror type 9 at line %d: wrong arguments\n", args->token_info.line);
			return false;
		}
        
		if( compareType( &(arg0->type), &(arg1->type) ) == false ) {
			printf("errror type 9 at line %d: wrong arguments\n", args->token_info.line);
			return false;
		}
	}

	return true;
}
			
// Exp -> ID LP Args RP
bool checkFunc(TreeNode* id, TreeNode* args, TreeNode* parent) {
    // get funcItem
	ItemInfo* func_item = checkDefine(id);
	if(func_item == NULL) {
		printf("error type 2 at line %d: function '%s' not defined\n", 
				id->token_info.line, id->token_info.str);
		return false;
	}
	else {
		// check if func
	    Type* func_type = &(func_item->id_type);
	    
		if( func_type->kind != FUNCTION ) {
			printf("error type 11 at line %d: '%s' not a function\n", 
					id->token_info.line, id->token_info.str);
			return false;
		}

		// check args
		SymbolList* head = func_type->list;
	    if( checkArgs(head, args) == true ) {	
			// right and give parent return_type
		    copyType(&(parent->token_info.id_type), &(func_item->return_type));
		}
    }
}

// Exp -> Exp op Exp
bool checkExpOpExp(TreeNode* right_exp, TreeNode* left_exp, TreeNode* parent, int op) {
	// for Dec -> VarDec
	if(right_exp == NULL) return true;

	Type* right_type = &(right_exp->token_info.id_type);
	Type* left_type = &(left_exp->token_info.id_type);

	// int RELOP int
	if( op == RELOP || op == AND || op == OR) {
		if( checkIfInt(right_type) == false ) {
			printf("error type 7 at line %d: Type mismatched for operands\n", 
					right_exp->token_info.line);
			return false;
		}
		if( checkIfInt(left_type) == false ) {
			printf("error type 7 at line %d: Type mismatched for operands\n", 
					left_exp->token_info.line);
			return false;
		}

		if(parent != NULL) 
		   editBasicType(&(parent->token_info.id_type), INTEGER);
	}
	else if( op == ASSIGNOP ) {
		if( left_exp->token_info.int_value != CAN_ASSIGN ) {
			printf("error type 6 at line %d: The left-hand side of '=' must be a variable\n",
					left_exp->token_info.line );
			return false;
		}
		if( compareType(right_type, left_type) == false ) { 
			printf("error type 5 at line %d: Type mismatched for assignment\n",
		    		left_exp->token_info.line );
			return false;
		}

		if(parent != NULL)
		  copyType(&(parent->token_info.id_type), &(right_exp->token_info.id_type) );
	}
	else {
		// int or float
	   if( checkIfInt(right_type) == true && checkIfInt(left_type) == true ) {
	   	  if(parent != NULL)
	   	    copyType(&(parent->token_info.id_type), &(right_exp->token_info.id_type) );
	   }
	   else if( checkIfFloat(left_type) == true && checkIfFloat(right_type) == true ) {
          if(parent != NULL)
            copyType(&(parent->token_info.id_type), &(right_exp->token_info.id_type) );
	   }
	   else {
	       printf("error type 7 at line %d: Type mismatched for operands\n", 
		     		left_exp->token_info.line);
		   return false;
	   }
    }

    return true;
}
	
// Exp -> op Exp
bool checkOpExp(TreeNode* exp, TreeNode* parent, int op) {
    TokenInfo* info = &(exp->token_info);
	Type* exp_type = &(info->id_type);
	
	if(op == MINUS) {
		// int or float
	   if( checkIfInt(exp_type) != true && checkIfFloat(exp_type) != true ) {
	       printf("error type 7 at line %d: Type mismatched for operands\n", 
		     		info->line);
		   return false;
	   }
	   else {
	        if(parent != NULL)
               copyType(&(parent->token_info.id_type), &(exp->token_info.id_type) );
       }
	}
	else { // NOT Exp
		// int
		if( checkIfInt(exp_type) == false ) {
	       printf("error type 7 at line %d: Type mismatched for operands\n", 
		     		info->line);
		   return false;
	   }
	   else {
	        if(parent != NULL)
               editBasicType(&(parent->token_info.id_type), INTEGER);
       }
	}

	return true;
}

// Exp -> ID
bool checkID(TreeNode* id, TreeNode* parent) {
	TokenInfo* id_token = &(id->token_info);
    TokenInfo* parent_token = &(parent->token_info);
    Type* id_type = &(id_token->id_type);

	// check if the id is defined
	ItemInfo* id_item = checkDefine(id);
	if(id_item == NULL) {
		printf("error type 1 at line %d: Undefined variable '%s'\n",
			   id_token->line, id_token->str);
		return false;
	}
    Type* item_type = &(id_item->id_type);
    
    // id's type
    copyType(id_type, item_type);

	// parent's type
	copyType(&(parent_token->id_type), id_type ); 

	// can assign ?
    parent_token->int_value = ifCanAssign(item_type);
    
    return true;
}	



     

