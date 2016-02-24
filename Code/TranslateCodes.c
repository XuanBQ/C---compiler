#include "Code.h"
#include "SymbolTable.h"
#include "TokenId.h"

#define __TRANSLATECODES_DEBUG__

// max permitted amount of a function's arguments  
#define ARGS_LIST_LENGTH 20

// operand is of type OperandKind
#define EXP_OP_EXP(operand, des)\
        /* Exp -> Exp1 [OP] Exp2 */\
        Operand *t1 = newVar();\
        Operand *t2 = newVar();\
        /* code1 for Exp1 */\
        Codes* code1 = translateExp(firstC, t1);\
        /* code2 for Exp2 */\
        Codes* code2 = translateExp(secondC->nextSibling, t2);\
        /* code3 for Exp */\
        Codes* code3 = buildBinOpCode(operand, des, t1, t2);\
        /* link */\
        return linkCodes(3, code1, code2, code3)

#define EXP_COND(Exp, Tempv)\
        int label1 = newLabel();\
        int label2 = newLabel();\
        /* code0 */\
        Operand *s = constructOperand(CONSTANT, 0);\
        Codes*code0 = buildMov2VarCode(Tempv, s);\
        /* code1 */\
        Codes *code1 = translateCond(Exp, label1, label2);\
        /* code2 */\
        Codes *code2_1 = buildDefLabelCode(label1);\
        Operand *s1 = constructOperand(CONSTANT, 1);\
        Codes *code2_2 = buildMov2VarCode(Tempv, s1);\
        Codes *code2 = addCodesToList(code2_1, code2_2);\
        /* link */\
        Codes *code3 = buildDefLabelCode(label2);\
        return linkCodes(4, code0, code1, code2, code3)

static Codes* translateCond(TreeNode *exp, int label_true, int label_false);
static Codes* translateArrayAddr(TreeNode *exp, Operand *des);
static Codes* translateArgs(TreeNode *args, Operand* args_list[], int *args_amount, int max_amount);
static void addArg(Operand* args_list[], Operand *arg, int *args_amount, int max_amount);

// des_num is a TempVar's num field
static Codes* translateExp(TreeNode* exp, Operand *des) {
   // first child
	TreeNode *firstC = exp->firstChild;
	TokenInfo *firstCInfo = &(firstC->token_info);
	switch(firstCInfo->id) {
		case ID: { TreeNode *secondC = firstC -> nextSibling;
                   // Exp -> ID
                   if(secondC == NULL) {
                     Operand *s = getIdTempVar(firstC);
			         Type *firstCType = &(firstCInfo->id_type);
                     if(checkIfArray(firstCType) == true) {// array's addr
                        return buildUnaryOpCode(GET_MEM_ADDR, des, s);
                     }
			         else return buildMov2VarCode(des, s);
                   }
                   // Exp -> ID LP RP
                   // or Exp -> ID LP Args RP
                   else {
                      TreeNode *thirdC = secondC -> nextSibling;
                      int thirdCid = thirdC->token_info.id;
                      // Exp -> ID LP RP
                      if(thirdCid == RP) {
                        char *func_name = getIdName(firstC);
                        if(strcmp(func_name, "read") == 0) {    
                            return buildReadCode(des);
                        }
                        else {
                            Codes *code = buildCallCode(des, func_name);
                            return code;
                        }
                      }
                      // Ex -> ID LP Args RP
                      else {
                        char *func_name = getIdName(firstC);
                        Operand* args_list[ARGS_LIST_LENGTH];
                        int args_amount = 0;
                        // code1 about args
                        Codes *code1 = translateArgs(thirdC, args_list, &args_amount, ARGS_LIST_LENGTH);
                        if(strcmp(func_name, "write") == 0) {
                            Codes *code2 = buildWriteCode(args_list[0]);
                            return linkCodes(2, code1, code2);
                        }
                        else {
                            int i;
                            Codes *code2 = NULL;
                            for(i = args_amount - 1; i >= 0; i --) {
                               Codes *arg_code = buildArgCode(args_list[i]);
                               code2 = addCodesToList(code2, arg_code);
                            }
                            // code3
                            Codes *code3 = buildCallCode(des, func_name);
                            return linkCodes(3, code1, code2, code3);
                        }
                      }
                   }
		         }
		case INT: {
                    int value = firstCInfo->int_value;
                    Operand *s = constructOperand(CONSTANT, value);
                    return buildMov2VarCode(des, s);
		          }
		case EXP : { 
			         TreeNode *secondC = firstC -> nextSibling;
			         int secondCid = secondC->token_info.id;
			         // Exp -> Exp1 ASSIGNOP Exp2
                     if(secondCid == ASSIGNOP) {
                     	 TreeNode* exp1fc = firstC -> firstChild;
                     	 TokenInfo* exp1fcInfo = &(exp1fc->token_info);
                         // code1: translate Exp2
                         Operand *exp2_t = newVar();
                         Codes *code1 = translateExp(secondC->nextSibling, exp2_t);
                     	 // Exp1 -> ID
                     	 if(exp1fcInfo->id == ID) {
                     	 	Operand *var = getIdTempVar(exp1fc); // get it from table
                            // code2 for Exp
                            Codes* code2 = buildMov2VarCode(var, exp2_t);
                            return linkCodes(2, code1, code2);
                     	 }
                     	 // Exp1 -> Exp3 LB INT RB
                     	 // will not appear: Exp1 -> Exp3 DOT ID
                         else if(exp1fcInfo->id == EXP){
                            Operand *t1 = newAddr();
                            // code2: get the address
                            Codes *code2 = translateArrayAddr(exp1fc, t1);
                            // code3: for Exp
                            Codes *code3 = buildMov2MemCode(t1, exp2_t);
                            return linkCodes(3, code1, code2, code3);
                         }
                     }
                     // Exp -> Exp1 PLUS Exp2
                     else if(secondCid == PLUS) {
                     	EXP_OP_EXP(ADD, des);
                     }
                     // Exp -> Exp1 MINUS Exp2
                     else if(secondCid == MINUS) {
                    	EXP_OP_EXP(SUB, des);
                     } 
                     // Exp -> Exp1 STAR Exp2
                     else if(secondCid == STAR) {
                    	EXP_OP_EXP(MUL, des);
                     }
                     // Exp -> Exp1 DIV Exp2
                     else if(secondCid == DIV) {
                    	EXP_OP_EXP(DIVI, des);
                     }
                     // Exp -> Exp1 AND Exp2
                     else if(secondCid == AND) {
                    	EXP_COND(exp, des);
                     }
                     // Exp -> Exp1 OR Exp2
                     else if(secondCid == OR) {
                    	EXP_COND(exp, des);
                     }
                     // Exp -> Exp1 RELOP Exp2
                     else if(secondCid == RELOP) {
                    	EXP_COND(exp, des);
                     }
                     // Exp -> Exp1 LB Exp RB
                     else if(secondCid == LB) {
                        Operand *t1 = newAddr();
                        Codes *code1 = translateArrayAddr(firstC, t1);
                        Codes *code2 = buildUnaryOpCode(GET_MEM_CONTENT, des, t1);
                        return addCodesToList(code1, code2);
                     }
                     // will not appear: Exp -> Exp1 DOT ID
                     //else if(secondCid == DOT) { }
                     break; 
                 }
        case MINUS: { // Exp -> MINUS Exp1
                      Operand *t1 = newVar();
                      Codes* code1 = translateExp(firstC->nextSibling, t1);
                      // code2
                      Operand* s = constructOperand(CONSTANT, 0);
                      Codes* code2 = buildBinOpCode(SUB, des, s, t1);
                      // link
                      return linkCodes(2, code1, code2);
                      break;
                    }
        case NOT: { // Exp -> NOT Exp1
                    EXP_COND(exp, des);
                    break;
                  }
        
        case LP: { // Exp -> LP Exp1 RP
        	       // translate Exp1
        	       return translateExp(firstC->nextSibling, des);
                 }
        case FLOAT: { 
                      #ifdef __TRANSLATECODES_DEBUG__
        	          fprintf(stderr, "%s, %d, in translateExp\n", __FILE__, __LINE__);
        	          #endif
        	          break;
        	        }
        default: return NULL;
	}
} 

// translating condition expression
static Codes* translateCond(TreeNode *exp, int label_true, int label_false) {
	// first child
	TreeNode *firstC = exp->firstChild;
	TokenInfo *firstCInfo = &(firstC->token_info);
	switch(firstCInfo->id) {
		case NOT: return translateCond(exp, label_false, label_true);
		case EXP: {
			         TreeNode* secondC = firstC -> nextSibling;
			         int secondCid = secondC->token_info.id;
                     // Exp -> Exp1 RELOP Exp2
			         if(secondCid == RELOP) {
			         	Operand *t1 = newVar();
			         	Operand *t2 = newVar();
			         	Codes *code1 = translateExp(firstC, t1);
			         	Codes *code2 = translateExp(secondC->nextSibling, t2);
			         	// code3
			         	char relop = (char)(secondC->token_info.int_value);
			         	Codes *code3 = buildIfGotoCode(relop, t1, t2, label_true);
                        // code4
                        Codes *code4 = buildGoCode(label_false);
                        return linkCodes(4, code1, code2, code3, code4);
			         }
			         // Exp -> Exp1 AND Exp2
			         else if(secondCid == AND) {
			         	int label1 = newLabel();
			         	Codes *code1 = translateCond(firstC, label1, label_false);
			         	Codes *code3 = translateCond(secondC->nextSibling, label_true, label_false);
			         	Codes *code2 = buildDefLabelCode(label1);
			         	return linkCodes(3, code1, code2, code3);
			         }
			         // Exp -> Exp1 OR Exp2
			         else if(secondCid == OR) {
			         	int label1 = newLabel();
			         	Codes *code1 = translateCond(firstC, label_true, label1);
			         	Codes *code3 = translateCond(secondC->nextSibling, label_true, label_false);
			         	Codes *code2 = buildDefLabelCode(label1);
			         	return linkCodes(3, code1, code2, code3);
			         }
			         // Exo -> Exp1 [other]
			         // no break, excute the statements in 'default'
		          }
		default: {
                   Operand *t1 = newVar();
                   Codes *code1 = translateExp(exp, t1);
                   // code2
                   Operand *t = constructOperand(CONSTANT, 0);
                   Codes *code2 = buildIfGotoCode('!', t1, t, label_true);
                   // code3
                   Codes *code3 = buildGoCode(label_false);
                   return linkCodes(3, code1, code2, code3);
		         }
	}
} 

// Exp0 -> Exp LB Exp1 RB
static Codes* translateArrayAddr(TreeNode *exp, Operand *des) {
    // Exp's first child
    TreeNode *firstC = exp->firstChild;
    TokenInfo *firstCInfo = &(firstC->token_info);
    // code1: translate Exp1
    Operand *exp1_t = newVar();
    Codes *code1 = translateExp(exp->nextSibling->nextSibling, exp1_t);
    // code2: offset
    int id_subsize = getSubSize(&(exp->token_info.id_type));
    Operand *t = constructOperand(CONSTANT, id_subsize);
    Operand *offset_t = newVar();
    Codes *code2 = buildBinOpCode(MUL, offset_t, t, exp1_t);
    // Exp -> ID (firstC is ID)
    if(firstCInfo->id == ID) { 
        // initial address of the array
        Operand *array = getIdTempVar(firstC);
        Operand *t1 = array;
        Codes *code3 = NULL;
        if(array->kind != ADDRESS) {
          t1 = newVar();
          code3 = buildUnaryOpCode(GET_MEM_ADDR, t1, array);
        }
        // initial address + offset
        Codes *code4 = buildBinOpCode(ADD, des, t1, offset_t);
        return linkCodes(4, code1, code2, code3, code4);
    }
    // Exp -> Exp2 LB Exp3 RB (firstC is Exp2)
    else { 
        Operand *t1 = newVar();
        Codes *code3 = translateArrayAddr(firstC, t1);
        Codes *code4 = buildBinOpCode(ADD, des, t1, offset_t);
        return linkCodes(4, code1, code2, code3, code4);
    }
}

static Codes* translateArgs(TreeNode *args, Operand* args_list[], int *args_amount, int max_amount) {
    TreeNode *firstC = args->firstChild;
    TreeNode *secondC = firstC->nextSibling;
    // Args -> Exp
    if(secondC ==  NULL) {
        Operand *t1 = newVar();
        Codes *code1 = translateExp(firstC, t1);
        addArg(args_list, t1, args_amount, max_amount);
        return code1;
    }
    // Args -> Exp COMMA Args1
    else {
        Operand *t1 = newVar();
        Codes *code1 = translateExp(firstC, t1);
        addArg(args_list, t1, args_amount, max_amount);
        Codes *code2 = translateArgs(secondC->nextSibling, args_list, args_amount, max_amount);
        return linkCodes(2, code1, code2);
    }
}

static void addArg(Operand* args_list[], Operand *arg, int *args_amount, int max_amount) {
    int cur_amount = *args_amount;
    if(cur_amount < max_amount) {    
        args_list[cur_amount] = arg;
        *args_amount = cur_amount + 1;
    }
    else {
        #ifdef __TRANSLATECODES_DEBUG__
        fprintf(stderr, "%s, %d, too many args\n", __FILE__, __LINE__);
        #endif
    }
}

// return the temporary variable/address of the ID, and
// generate codes about the id
Operand* codeVarDec(TreeNode *top_var, Codes** pcode) {
    // find the ID
    TreeNode* id = top_var->firstChild;
    while((id->token_info).id != ID) {
        id = id -> firstChild;
    }

    Type *id_type = &(id->token_info.id_type);
    if( checkIfArray(id_type) == false ) {// not an array
        Operand *s = newVar();
        setIdTempVar(id, s);
        return s;
    }
    else { // an array
      Operand *s = newVar();
      setIdTempVar(id, s);
      *pcode = buildDecCode(s, getSize(id_type));
      return s;
    }
}

// Def ->Specifier DecList SEMI
// add codes to top_declist's code(declist_code)
void codeDecList(TreeNode *declist, Codes** pdeclist_code) {   
    if(declist == NULL) return;

    Codes *declist_code = *pdeclist_code;
    TreeNode* node = declist; 
    TokenInfo* info = &(node->token_info);

    // top VarDec(s)
    if(info->id == VARDEC) {
        Codes *code1 = NULL;
        Operand* place = codeVarDec(node, &code1);
        *pdeclist_code = addCodesToList(declist_code, code1);

        // about Dec -> VarDec ASSIGNOP Exp
        TreeNode* assignop = node->nextSibling;
        if( assignop != NULL ) { // has assignop, and the id must not be an array
           TreeNode* exp = assignop->nextSibling;
           Codes *code2 = translateExp(exp, place);
           // top_declist's code
           *pdeclist_code = addCodesToList(declist_code, code2);
        }
        
        return;
    }
    
    // visiting children
    codeDecList(node->firstChild, pdeclist_code);
    codeDecList(node->nextSibling, pdeclist_code);
}

// deflist in : CompSt -> LC DefList StmtList RC
void codeDefList(TreeNode *deflist, Codes **pdeflist_code) {
    if(deflist == NULL) return;
    
    TreeNode *node = deflist;
    TokenInfo *info = &(node->token_info);
    // link all def's codes
    if(info->id == DEF) {
       Codes *deflist_code = *pdeflist_code;
       *pdeflist_code = addCodesToList(deflist_code, info->code);
        // DefList -> Def DefList
        codeDefList(node->nextSibling, pdeflist_code); // Def's nextSibling
    }

    codeDefList(node->firstChild, pdeflist_code); // DefList's firstChild
    
}

// stmt's code in :  Stmt -> Exp SEMI
void codeStmtExp(TreeNode *stmt, TreeNode *exp) {
    stmt->token_info.code = translateExp(exp, NULL);
}
// stmt's code in :  Stmt -> CompSt
void codeStmtCompSt(TreeNode *stmt, TreeNode *compst) {
    stmt->token_info.code = compst->token_info.code;
}
// stmt's code in :  Stmt -> RETURN Exp SEMI
void codeStmtReturn(TreeNode *stmt, TreeNode *exp) {
    Operand *s = newVar();
    Codes *code1 = translateExp(exp, s);
    Codes *code2 = buildRetCode(s);
    stmt->token_info.code = addCodesToList(code1, code2);
}
// stmt's code in :  Stmt -> IF LP Exp RP Stmt1
void codeStmtIf(TreeNode *stmt, TreeNode *exp, TreeNode *stmt1) {
    int label1 = newLabel();
    int label2 = newLabel();
    Codes *code1 = translateCond(exp, label1, label2);
    Codes *code3 = stmt1->token_info.code;
    Codes *code2 = buildDefLabelCode(label1);
    Codes *code4 = buildDefLabelCode(label2);
    stmt->token_info.code = linkCodes(4, code1, code2, code3, code4);
}
// stmt's code in :  Stmt -> IF LP Exp RP Stmt1 ELSE Stmt2
void codeStmtIfElse(TreeNode *stmt, TreeNode *exp, TreeNode *stmt1, TreeNode *stmt2) {
    int label1 = newLabel();
    int label2 = newLabel();
    int label3 = newLabel();
    Codes *code1 = translateCond(exp, label1, label2);
    Codes *code3 = stmt1->token_info.code;
    Codes *code6 = stmt2->token_info.code;
    Codes *code2 = buildDefLabelCode(label1);
    Codes *code4 = buildGoCode(label3);
    Codes *code5 = buildDefLabelCode(label2);
    Codes *code7 = buildDefLabelCode(label3);
    stmt->token_info.code = linkCodes(7, code1, code2, code3, code4, code5, code6, code7);
}

// stmt's code in :  Stmt -> WHILE LP Exp RP Stmt1
void codeStmtWhile(TreeNode *stmt, TreeNode *exp, TreeNode *stmt1) {
    int label1 = newLabel();
    int label2 = newLabel();
    int label3 = newLabel();
    Codes *code2 = translateCond(exp, label2, label3);
    Codes *code4 = stmt1->token_info.code;
    Codes *code1 = buildDefLabelCode(label1);
    Codes *code3 = buildDefLabelCode(label2);
    Codes *code5 = buildGoCode(label1);
    Codes *code6 = buildDefLabelCode(label3);
    stmt->token_info.code = linkCodes(6, code1, code2, code3, code4, code5, code6);
}

// stmtlist in : CompSt -> LC DefList StmtList RC
void codeStmtList(TreeNode *stmtlist, Codes **pstmtlist_code) {
    if(stmtlist == NULL) return;

    Codes* stmtlist_code = *pstmtlist_code;
    TreeNode *node = stmtlist;
    TokenInfo *info = &(node->token_info);
    // add all stmts' codes
    if(info -> id == STMT) {
        *pstmtlist_code = addCodesToList(stmtlist_code, info->code);
        // StmtList -> Stmt StmtList
        codeStmtList(node->nextSibling, pstmtlist_code); //Stmt's nextSibling
    }

    codeStmtList(node->firstChild, pstmtlist_code); // StmtList's firstChild
}

// CompSt -> LC DefList StmtList RC
void codeCompSt(TreeNode *compst, TreeNode *deflist, TreeNode *stmtlist) {
    Codes *deflist_code = deflist->token_info.code;
    Codes *stmtlist_code = stmtlist->token_info.code;
    compst->token_info.code = addCodesToList(deflist_code, stmtlist_code);
}

// generate codes: [param arg], and
// the temporary variable/address of the arg
// the var's position in params(argNo >= 0)
static Codes* codeArg(TreeNode *top_var, int argNo) {
    // find the ID
    TreeNode* id = top_var->firstChild;
    while((id->token_info).id != ID) {
        id = id -> firstChild;
    }

    Type *id_type = &(id->token_info.id_type);
    if( checkIfArray(id_type) == false ) {// not an array
        Operand *s = newVar();
        popS(); // has already pushed it in newVar(), but no need to do that
        s->posInFrame = -argNo;
        setIdTempVar(id, s);
        return buildParamCode(s);
    }
    else { // an array
      Operand *s = newAddr();
      setIdTempVar(id, s);
      popS(); // has already pushed it in newVar(), but no need to do it
      s->posInFrame = -(getSize(id_type));//in fact, it will not appear in lab4
      return buildParamCode(s);
    }
}

int argN;
// FunDec -> ID LP VarList RP,
// add codes about arguments to FunDec
void codeVarList(TreeNode *varlist, Codes** pfundec_code) {
    if(varlist == NULL) return;

    TreeNode *node = varlist;
    TokenInfo *info = &(varlist->token_info);
    if(info->id == VARDEC) {
        Codes *fundec_code = *pfundec_code;
        *pfundec_code = addCodesToList(fundec_code, codeArg(node, argN++));
        return;
    }

    codeVarList(node->firstChild, pfundec_code);
    codeVarList(node->nextSibling, pfundec_code);
}

// ExtDef -> Specifier FunDec CompSt
void codeExtDef(TreeNode *extdef, TreeNode *fundec, TreeNode *compst) {
    Codes *fundec_code = fundec->token_info.code;
    Codes *compst_code = compst->token_info.code;

    // add [FUNCTION func_name] to fundec_code's head
    char *func_name = getIdName(fundec->firstChild);
    Codes *code1 = buildDefFuncCode(func_name, 4*getSpIndex());
    fundec_code = addCodesToList(code1, fundec_code);

    // extdef's code
    extdef->token_info.code = addCodesToList(fundec_code, compst_code);
}

// link ExtDefs' codes to codes_head
void codeExtDefList(TreeNode *extdeflist, Codes** program_head) {
    if(extdeflist == NULL) return;

    TreeNode *node = extdeflist;
    TokenInfo *info = &(node->token_info);
    if(info->id == EXTDEF) {
        Codes *program_head_code = *program_head;
        *program_head = addCodesToList(program_head_code, info->code);
        // other ExtDefs (ExtDef's brothers)
        codeExtDefList(node->nextSibling, program_head);
        return;
    }

    codeExtDefList(node->firstChild, program_head); // ExtDefList has no brothers
} 