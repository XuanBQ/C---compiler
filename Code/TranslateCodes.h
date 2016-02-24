#ifndef __TRANSLATECODES_H__
#define __TRANSLATECODES_H__

void codeExtDefList(TreeNode *extdeflist, Codes** program_head);

void codeExtDef(TreeNode *extdef, TreeNode *fundec, TreeNode *compst);

void codeVarList(TreeNode *varlist, Codes** pfundec_code);

void codeCompSt(TreeNode *compst, TreeNode *deflist, TreeNode *stmtlist);

void codeStmtList(TreeNode *stmtlist, Codes **pstmtlist_code);

void codeStmtWhile(TreeNode *stmt, TreeNode *exp, TreeNode *stmt1);

void codeStmtIfElse(TreeNode *stmt, TreeNode *exp, TreeNode *stmt1, TreeNode *stmt2);

void codeStmtIf(TreeNode *stmt, TreeNode *exp, TreeNode *stmt1);

void codeStmtReturn(TreeNode *stmt, TreeNode *exp) ;

void codeStmtCompSt(TreeNode *stmt, TreeNode *compst);

void codeStmtExp(TreeNode *stmt, TreeNode *exp);

void codeDefList(TreeNode *deflist, Codes **pdeflist_code);

void codeDecList(TreeNode *declist, Codes** pdeclist_code);

void addArg(Operand* args_list[], Operand *arg, int *args_amount, int max_amount);

#endif

