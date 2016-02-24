#ifndef __ParseStack_H__
#define __ParseStack_H__

#include "ParseTree.h"

#define STACK_LENGTH 100

void initStack();

//failed, return 0
//successed, return 1 
int push(TreeNode *);

//failed, return NULL
//successed, return the poped content
TreeNode* pop();

#endif
