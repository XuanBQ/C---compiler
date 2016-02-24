#include "ParseStack.h"
#include "TokenId.h"

static TreeNode* ParseStack[STACK_LENGTH];

static int stack_top;

void initStack() {
	stack_top = -1;
}

int push(TreeNode *node) {
	//check if the stack is full
	if(stack_top >= STACK_LENGTH - 1) {
		printf("Parse failed! The stack is full! %d\n", stack_top);
		
		//failed, return 0
		return 0;
	}

	//check if node is NULL
	if(node == NULL) {
		printf("cannot push NULL node! %d\n", stack_top);
		exit(-1);
	}

	//push
	ParseStack[++stack_top] = node;
	//printf("push %d %s\n", stack_top, TOKEN_NAME((node->token_info).id));

	//successed, return 1
	return 1;
}

TreeNode *pop() {
	//check if the stack is empty
	if(stack_top < 0) {
		printf("Parse failed! Parser stack is empty!\n");

		//failed
		exit(-1);
	}

	//get the Node to pop
	TreeNode *p = ParseStack[stack_top];

	//pop
	//printf("pop %d %s\n", stack_top, TOKEN_NAME((p->token_info).id));
	--stack_top;
	
	//check if p is NULL
	if(p == NULL) {
		printf("pop NULL! stack_top = %d\n", stack_top + 1);
		exit(-1);
	}

	//successed, return the node poped
	return p;
}

