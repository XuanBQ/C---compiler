#ifndef __DEFINEID_H__
#define __DEFINEID_H__

#include "TypeInfo.h"
#include "SymbolTable.h"

bool struct_defining;

void buildFieldList(TreeNode* def_list, SymbolList* head); // for DefList

bool fillOptTag(TreeNode* opt_tag);

// find the defined struct in the Table
// if tag's id doesn't exists in the Table, 
// report error and return NULL
SymbolList* getFieldList(TreeNode* tag); // for Tag

// define the IDs that's ExtDecList's children
void fillExtDecList(TreeNode* ext_dec_list);

// define the ID that's VarDec's child
// return the ID's info
TreeNode* fillVarDec(Type* type, TreeNode* top_var);

// define the ID that's FunDec's child
void fillFunDec(TreeNode* id, TreeNode* var_list);

void checkReturn(TreeNode* compst, Type* t);

// define the IDs that's DecList's children
void fillDecList(TreeNode* dec_list, Type* type_info);

void fillDefList(TreeNode* def_list);

#endif
