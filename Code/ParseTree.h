#ifndef __ParseTree_H__
#define __ParseTree_H__

#include "common.h"
#include "TypeInfo.h"
#include "Code.h"

struct token_info {
	int id;   //the token's name

	int int_value; // record the value of token 'INT' or 'RELOP'
	               // INTEGER - TYPE int, DECIMAL - TYPE float
	               
	float float_value; // record the value of token 'FLOAT'

	char str[NAME_LENGTH]; // record the name of token 'ID'
	Type id_type; // Type of id

    Codes *code; // carrying the code
	int line;  // the token apperas in which line 
};
typedef struct token_info TokenInfo;

struct tree_node {
	TokenInfo token_info;
	struct tree_node* firstChild;
	struct tree_node* nextSibling;
};
typedef struct tree_node TreeNode;

//creating a new node and return its pointer
TreeNode *createNode();

//edit a 'TokenInfo'
//arg 'value' can point to int_value or float_value or str
void editTokenInfo(TokenInfo *node, int id, int line, void *value);

//creating a construt node
TreeNode *buildNode(int id, int line);

//parent node linking its child
void linkChild(TreeNode *parent, TreeNode *child);

//PreOrder Traversal
void preTraversal(TreeNode *root, int ispaces);

//do something when visiting a node
void visitNode(TreeNode *p);

//free nodes
void freeTree(TreeNode *root);

//print n ' 's
void printSpace(int n); 

#endif
	

	


