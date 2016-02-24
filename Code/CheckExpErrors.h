#ifndef __CHECKEXPERRORS_H__
#define __CHECKEXPERRORS_H__

enum ASSIGN { CAN_ASSIGN = 123, CANT_ASSIGN = 124};

bool checkDot(TreeNode* exp, TreeNode* id, TreeNode* parent);
bool checkLbRb(TreeNode* arr_exp, TreeNode* int_exp, TreeNode* parent);
void buildExpArgs(TreeNode* args, SymbolList* head);
bool checkArgs(SymbolList* head, TreeNode* args);
bool checkFunc(TreeNode* id, TreeNode* args, TreeNode* parent);
bool checkExpOpExp(TreeNode* right_exp, TreeNode* left_exp, TreeNode* parent, int op);
bool checkID(TreeNode* id, TreeNode* p);
bool checkOpExp(TreeNode* exp, TreeNode* parent, int op);
#endif
