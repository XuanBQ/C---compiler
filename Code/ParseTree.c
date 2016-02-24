#include "ParseTree.h"
#include "ParseStack.h"
#include "TokenId.h"
#include "TypeInfo.h"
#include <string.h>

TreeNode *createNode() {
	//allocating space
	TreeNode *p;
	p = (TreeNode *)malloc(sizeof(TreeNode));

	//allocate space failed
	if(p == NULL) {
		printf("Creating a new node failed!\n");
		exit(-1);
	}

	//initialize the new node
	p -> firstChild = p -> nextSibling = NULL;

	return p;
}

void editTokenInfo(TokenInfo *node, int id, int line, void *value_addr) {
	node -> id = id;
	node -> line = line;

	// init int_val
	node -> int_value = -1;  // no int_value

	// more information
	if(value_addr != NULL) {
	  switch(id) {
	    	//int constant, record its value in node->int_value
	    	case INT: { int int_val = *((int *)value_addr); 
				        node -> int_value = int_val;
				        break;
				      }

	        //float constant, record its value in node->float_value
		    case FLOAT: { float float_val = *((float *)value_addr); 
		     			  node -> float_value = float_val;
					      break;
					    }

	        //string, copy it to node->str
		    case ID: { char *str0 = (char *)value_addr;
		     		   sscanf(str0, "%[0-9a-zA-Z_]", node -> str ); //filer some other characters following
				       break;
				     }

	       //TYPE, "int" or "float"
		   case TYPE: { int int_val = *((int *)value_addr);
				        node -> int_value = int_val;
				        break;
				      }
					  
		  // RELOP, int_val
		   case RELOP: { int int_val = *((int *)value_addr);
						 node -> int_value = int_val;
						 break;
					   }

		default: break; //syntax error, do nothing here

	  }
	}

    node -> code = NULL;
}

TreeNode *buildNode(int id, int line) {
	TreeNode *p = createNode();
	editTokenInfo(&(p->token_info), id, line, NULL);
}

void linkChild(TreeNode *parent, TreeNode *child) {
	//check if parent and child are both legal
	if(parent == NULL) {
		printf("pointer 'parent' cannot be NULL!\n");
		exit(-1);
	}
	if(child == NULL) {
		printf("pointer 'child' cannot be NULL !\n");
		exit(-1);
	}

	//check if the parent has first child
	if(parent -> firstChild == NULL) {
		//let 'child' becomes the first child of the parent
		parent -> firstChild = child;
	}
    else {
		//find the position to link 'child'
		TreeNode *p = parent -> firstChild;
		TreeNode *temp_p; 
		while(p != NULL) {
			temp_p = p;  //temp_p save the value of p before p is changed
			p = p -> nextSibling;
		}

		//let 'child' becomes the last child of the parent
		temp_p -> nextSibling = child;
	}
}

void visitNode(TreeNode *q) {
	if(!q) {
		printf(" Node cannnot be NULL\n ");
		exit(-1);
	}

	TokenInfo *p = &(q -> token_info);
    int id = p -> id;

	//print NAME
	printf("%s", TOKEN_NAME(p -> id)); 

	//print other info
	if(id < PROGRAM) { //terminal symbols
	  switch(id) {
		  case TYPE: if(p -> int_value == INTEGER) printf(": int");
		             else if(p -> int_value == DECIMAL) printf(": float");
		             break;

		  case ID: printf(": %s", p -> str);
				   break;

		  case INT: printf(": %d", p -> int_value);
				    break;

		  case FLOAT: printf(": %f", p -> float_value);
					  break;

		   /* RELOP, int_value
		    * ascii code, '<' for <
		    * '>' for >
		    * '=' for ==
		    * ',' for <=
		    * '.' for >
			* '!' for !=
			*/
		  case RELOP: { char val = (char)(p -> int_value);
						if(val == '<') printf(": <");
						else if(val == '>') printf(": >");
						else if(val == ',') printf(": <=");
						else if(val == '.') printf(": >=");
						else if(val == '=') printf(": ==");
						else if(val == '!') printf(": !=");
					  }
					  break;

		  default: /* no other nfo to print */ break;
	  }
	}
	else { //nonterminal tokens, print line number
		printf(" (%d)", p -> line);
	}
}

void preTraversal(TreeNode *root, int ispaces) {
	if(!root) return;

    //print information
    //if root is a empty string, do noth
    if((root->token_info).id != NOTHING) {
	   printSpace(ispaces);
	   visitNode(root);
	   printf("\n");
	}

	preTraversal(root -> firstChild, ispaces + 2);
	preTraversal(root -> nextSibling, ispaces);
}

void freeTree(TreeNode *root) {
	if(!root) return;
	
	freeTree(root -> firstChild);
	freeTree(root -> nextSibling);
	
	free(root);
}

void printSpace(int n) {
	int i;
	for(i = 0; i < n; i ++) {
		printf(" ");
	}
}

		





