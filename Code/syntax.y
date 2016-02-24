%{
   #include <stdio.h>
   #include "lex.yy.c"
   #include "TypeInfo.h"
   #include "ParseTree.h"
   #include "ParseStack.h"
   #include "TokenId.h"
   #include "DefineId.h"
   #include "CheckExpErrors.h"
   #include "Code.h"
   #include "TranslateCodes.h"

   //#define __DEBUG__ 
   
   bool struct_defining;
   int B_error;
   TreeNode *root;
%}

/* declare types */
%union {
  int type_int;
  float type_float;
  char* type_str;
}
/* declare tokens */
%right ASSIGNOP
%left OR
%left AND
%left <type_int> RELOP
%left PLUS MINUS
%left DIV STAR
%right NOT
%left DOT RP LP RB LB
%token <type_int> INT
%token <type_float> FLOAT
%token <type_str> ID
%token <type_int> TYPE
%token SEMI COMMA
%left LC RC
%nonassoc LOWER_THAN_ELSE
%nonassoc STRUCT RETURN IF ELSE WHILE

/* every 'reduce' generate a parent node p
   get the child nodes of p by the reducing rule, 
   and link p with the childs
   var childi is the leftest i child of parent p 
*/
/* use stack to help */
%%
Program : ExtDefList { TreeNode *p = buildNode(PROGRAM, @$.first_line);
		               TreeNode *child = pop();

                       // the whole program's codes
                       codeExtDefList(child, &codes_head);

                       linkChild(p, child);
                       push(p);
                       root = p;
                     }
		;

ExtDefList : { TreeNode *p = buildNode(NOTHING, @$.first_line);

               push(p);
             } 
		   | ExtDef ExtDefList { TreeNode *p = buildNode(EXTDEFLIST, @$.first_line);
                                 TreeNode *child0 = pop();
                                 TreeNode *child1 = pop();

                                 linkChild(p, child1);
                                 linkChild(p, child0);
                                 push(p);
                               }
           ;

ExtDef : Specifier ExtDecList SEMI { TreeNode *p = buildNode(EXTDEF, @$.first_line);
	                                 TreeNode *child0 = buildNode(SEMI, @3.first_line);
                                     TreeNode *child1 = pop();
                                     TreeNode *child2 = pop();

                                     // ExtDecList's type
                                     copyType(&((child1->token_info).id_type), &((child2->token_info).id_type));
                                                                  

                                     // ExtDecList's child IDs
                                     fillExtDecList(child1);

                                     linkChild(p, child2);
                                     linkChild(p, child1);
                                     linkChild(p, child0);
                                     push(p);
                                   }
	   | Specifier SEMI { TreeNode *p = buildNode(EXTDEF, @$.first_line);
                          TreeNode *child0 = buildNode(SEMI, @2.first_line);
                          TreeNode *child1 = pop();

                          linkChild(p, child1);
                          linkChild(p, child0);
                          push(p);
                        }
       | Specifier FunDec CompSt { TreeNode *p = buildNode(EXTDEF, @$.first_line);
                                   TreeNode *child0 = pop();
                                   TreeNode *child1 = pop();
                                   TreeNode *child2 = pop();
                                   
                                   // check if return right
                                   checkReturn(child0, &(child2->token_info.id_type));

                                   // function's code
                                   codeExtDef(p, child1, child0);
                                   
                                   // for args field
                                   leaveField();
                                   
                                   linkChild(p, child2);
                                   linkChild(p, child1);
                                   linkChild(p, child0);
                                   push(p);
                                   
                                   
                                 }
      | Specifier FunDec SEMI { TreeNode *p = buildNode(EXTDEF, @$.first_line);
                                TreeNode *child0 = buildNode(SEMI, @3.first_line);
                                TreeNode *child1 = pop();
                                TreeNode *child2 = pop();
                                
                                // for args field
                                   leaveField();
                                   
                                // FunDec's return type
                                   copyType(&((child1->token_info).id_type), &((child2->token_info).id_type)); 

                                // FunDec's args
                                   fillFunDec(child1, NULL);
                                linkChild(p, child2);
                                linkChild(p, child1);
                                linkChild(p, child0);
                                push(p);
                              }
             | error STRUCT
             | error TYPE
             | error SEMI
       ;

ExtDecList : VarDec { TreeNode *p = buildNode(EXTDECLIST, @$.first_line);
		              TreeNode *child = pop();

                      linkChild(p, child);
                      push(p);
                    }
		   | VarDec COMMA ExtDecList { TreeNode *p = buildNode(EXTDECLIST, @$.first_line);
                                       TreeNode *child0 = pop();
                                       TreeNode *child1 = buildNode(COMMA, @2.first_line);
                                       TreeNode *child2 = pop();

                                       linkChild(p, child2);
                                       linkChild(p, child1);
                                       linkChild(p, child0);
                                       push(p);
                                     }
           | error SEMI
           ;

Specifier : TYPE { TreeNode *p = buildNode(SPECIFIER, @$.first_line);
		           TreeNode *child0 = createNode();
                   editTokenInfo(&(child0->token_info), TYPE, @1.first_line, (void *)(&$1));

                   // Specifier's type
                   editBasicType(&((p->token_info).id_type), (BasicType)$1);

                   linkChild(p, child0);
                   push(p);
                 }
		  | StructSpecifier { TreeNode *p = buildNode(SPECIFIER, @$.first_line);
                              TreeNode *child = pop();
                              
                              // lab3
                              printf("Cannot translate: Code contains variables or paramters of structure type.\n");
                              
                              // Specifier's type
                              copyType(&((p->token_info).id_type), &((child->token_info).id_type));

                              linkChild(p, child);
                              push(p);
                            }
          | error ID
          | error SEMI
          ;

StructSpecifier : STRUCT OptTag LC DefList RC { TreeNode *p = buildNode(STRUCTSPECIFIER, @$.first_line);
				                                TreeNode *child0 = buildNode(RC, @5.first_line);
                                                TreeNode *child1 = pop();
                                                TreeNode *child2 = buildNode(LC, @3.first_line);
                                                TreeNode *child3 = pop();
                                                TreeNode *child4 = buildNode(STRUCT, @1.first_line);
                                                
                                                // struct field list
                                                SymbolList* head = createListNode();
                                                buildFieldList(child1, head);
                                                // p's type, different from OptTag's type
                                                editStructType(&((p->token_info).id_type), head);

                                                leaveField();

                                                // OptTag, fill in the Table
                                                editTagType(&((child3->token_info).id_type), head);
                                                if( fillOptTag(child3) == false ) freeList(head);

                                                // end define struct
                                                struct_defining = false;

                                                linkChild(p, child4);
                                                linkChild(p, child3);
                                                linkChild(p, child2);
                                                linkChild(p, child1);
                                                linkChild(p, child0);
                                                push(p);
                                              }
				| STRUCT Tag { TreeNode *p = buildNode(STRUCTSPECIFIER, @$.first_line);
                               TreeNode *child0 = pop();
                               TreeNode *child1 = buildNode(STRUCT, @1.first_line);
                               
                               // isn't define struct
                               struct_defining = false;
                               
                               // get struct field list,
                               // and will edit Tag's type
                               SymbolList* head = getFieldList(child0);
                               
                               // if exists
                               TokenInfo* tag = &(child0->token_info);
                               if(head != NULL) {
                                 if( !checkIfStructTag( &(tag->id_type) ) ) {
                                     printf("error type 17 at Line %d: Undefined struct '%s'\n", 
                                        tag->line, tag->str ); 
                                 }
                                 // p's type
                                 editStructType(&((p->token_info).id_type), head);
                               }
                               else { 
                                 printf("error type 17 at Line %d: Undefined struct '%s'\n", 
                                        tag->line, tag->str ); 
                               }                               

                               linkChild(p, child1);
                               linkChild(p, child0);
                               push(p);
                             }
                | error ID
                | error RC
                ;

OptTag : { TreeNode *p = buildNode(OPTTAG, @$.first_line);
           TreeNode *child = buildNode(NOTHING, @$.first_line);

           linkChild(p, child);
            push(p);
         } 
	   | ID { TreeNode *p = buildNode(OPTTAG, @$.first_line);
              TreeNode *child = createNode();
              editTokenInfo(&(child->token_info), ID, @1.first_line, (void *)$1);

              linkChild(p, child);
              push(p);
            }
       | error LC
       ;

Tag : ID { TreeNode *p = buildNode(TAG, @$.first_line);
	       TreeNode *child = createNode();
           editTokenInfo(&(child->token_info), ID, @1.first_line, (void *)$1);

           // give Tag the ID's name
           strcpy( (p->token_info).str, $1);

           linkChild(p, child);
           push(p);
         }
	;

VarDec : ID { TreeNode *p  = buildNode(VARDEC, @$.first_line);
	          // about array dimension
	          (p->token_info).int_value = 0;  

	          TreeNode *child = createNode();
              editTokenInfo(&(child->token_info), ID, @1.first_line, (void *)$1);

              linkChild(p, child);
               
              push(p);
            }
	   | VarDec LB INT RB { TreeNode *p = buildNode(VARDEC, @$.first_line);

                            TreeNode *child0 = buildNode(RB, @4.first_line);
                            TreeNode *child1 = createNode(); 
                            editTokenInfo(&(child1->token_info), INT, @3.first_line, (void *)(&$3));
                            TreeNode *child2 = buildNode(LB, @2.first_line);
                            TreeNode *child3 = pop();
                       
                            TokenInfo* pinfo = &(p->token_info);
                            TokenInfo* c3info = &(child3->token_info);
                            // about array dimension
                            pinfo->int_value = c3info->int_value + 1; 
                            // about p'd type
                            editArrayType(&(pinfo->id_type), &(c3info->id_type), pinfo->int_value, $3);  

                            linkChild(p, child3);
                            linkChild(p, child2);
                            linkChild(p, child1);
                            linkChild(p, child0);
                            push(p);
                          }
       | error RB
       | error COMMA
	   ;

FunDec : ID LP {enterField();} 
               VarList RP { TreeNode *p = buildNode(FUNDEC, @$.first_line);
	                          TreeNode *child0 = buildNode(RP, @4.first_line);
                            TreeNode *child1 = pop();
                            TreeNode *child2 = buildNode(LP, @2.first_line);
                            TreeNode *child3 = createNode();
                            editTokenInfo(&(child3->token_info), ID, @1.first_line, (void *)$1);

                            // FunDec's return type (get it from stack)
                            TreeNode *specifier = pop();
                            copyType(&((child3->token_info).id_type), &(specifier->token_info.id_type));
                            push(specifier); 

                            // FunDec's args
                            fillFunDec(child3, child1);

                            // codes about arguments
                            codeVarList(child1, &(p->token_info.code));

                            linkChild(p, child3);
                            linkChild(p, child2);
                            linkChild(p, child1);
                            linkChild(p, child0);
                            push(p);

                            // new frame in the stack
                            newFuncFrame();
                          }
	   | ID LP { enterField(); }
             RP  { TreeNode *p = buildNode(FUNDEC, @$.first_line);
                    TreeNode *child0 = buildNode(RP, @3.first_line);
                    TreeNode *child1 = buildNode(LP, @2.first_line);
                    TreeNode *child2 = createNode();
                    editTokenInfo(&(child2->token_info), ID, @1.first_line, (void *)$1);
                    
                    // FunDec's return type
                    TreeNode *specifier = pop();
                    copyType(&((child2->token_info).id_type), &(specifier->token_info.id_type));
                    push(specifier); 

                    // FunDec's args
                    fillFunDec(child2, NULL); // null means no args
                    
                    linkChild(p, child2);
                    linkChild(p, child1);
                    linkChild(p, child0);
                    push(p);

                    // new frame in the stack
                    newFuncFrame();
                  }
       | error RP
       | error LC
       ;

VarList : ParamDec COMMA VarList { TreeNode *p = buildNode(VARLIST, @$.first_line);
		                           TreeNode *child0 = pop();
                                   TreeNode *child1 = buildNode(COMMA, @2.first_line);
                                   TreeNode *child2 = pop();

                                   linkChild(p, child2);
                                   linkChild(p, child1);
                                   linkChild(p, child0);
                                   push(p);
                                  }
		| ParamDec { TreeNode *p = buildNode(VARLIST, @$.first_line);
                     TreeNode *child = pop();

                     linkChild(p, child);
                     push(p);
                   }
        | error RP
        ;

ParamDec : Specifier VarDec { TreeNode *p = buildNode(PARAMDEC, @$.first_line);
		                      TreeNode *child0 = pop();
                              TreeNode *child1 = pop();

                              // define VarDec's child id
                              fillVarDec(&((child1->token_info).id_type), child0);

                              linkChild(p, child1);
                              linkChild(p, child0);
                              push(p);
                            }
          | error COMMA
		 ;

CompSt : LC DefList StmtList RC { TreeNode *p = buildNode(COMPST, @$.first_line);
	                              TreeNode *child0 = buildNode(RC, @4.first_line);
                                  TreeNode *child1 = pop();
                                  TreeNode *child2 = pop();
                                  TreeNode *child3 = buildNode(LC, @1.first_line);
                                  
                                  // code
                                  codeStmtList(child1, &(child1->token_info.code));
                                  codeDefList(child2, &(child2->token_info.code));
                                  codeCompSt(p, child2, child1);

                                  // leave current field
                                  leaveField();

                                  linkChild(p, child3);
                                  linkChild(p, child2);
                                  linkChild(p, child1);
                                  linkChild(p, child0);
                                  push(p);
                                  
                                }
       | error SEMI
	   | error RC
	   ;

StmtList : { TreeNode *p = buildNode(STMTLIST, @$.first_line);
             TreeNode *child = buildNode(NOTHING, @$.first_line);

             linkChild(p, child);
             push(p);
           }
		 | Stmt StmtList { TreeNode *p = buildNode(STMTLIST, @$.first_line);
                           TreeNode *child0 = pop();
                           TreeNode *child1 = pop();

                           linkChild(p, child1);
                           linkChild(p, child0);
                           push(p);
                         }
         | error RC
         ;

Stmt : Exp SEMI { TreeNode *p = buildNode(STMT, @$.first_line);
	              TreeNode *child0 = buildNode(SEMI, @2.first_line);
                  TreeNode *child1 = pop();

                  // code
                  codeStmtExp(p, child1);

                  linkChild(p, child1);
                  linkChild(p, child0);
                  push(p);
                }
	 | CompSt { TreeNode *p = buildNode(STMT, @$.first_line);
                TreeNode *child = pop();

                // code
                codeStmtCompSt(p, child);

                linkChild(p, child);
                push(p);
              }
     | RETURN Exp SEMI { TreeNode *p = buildNode(STMT, @$.first_line);
                         TreeNode *child0 = buildNode(SEMI, @3.first_line);
                         TreeNode *child1 = pop();
                         TreeNode *child2 = buildNode(RETURN, @1.first_line);
                         
                         // code
                         codeStmtReturn(p, child1);

                         linkChild(p, child2);
                         linkChild(p, child1);
                         linkChild(p, child0);
                         push(p);
                       }
     | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { TreeNode *p = buildNode(STMT, @$.first_line);
                                                 TreeNode *child0 = pop();
                                                 TreeNode *child1 = buildNode(RP, @4.first_line);
                                                 TreeNode *child2 = pop();
                                                 TreeNode *child3 = buildNode(LP, @2.first_line);
                                                 TreeNode *child4 = buildNode(IF, @1.first_line);

                                                 // exp should be int
                                                 if( !checkIfInt( &(child2->token_info.id_type)) ) {
                                                    printf("error type 7 at line %d: variable in 'if( )' should be int\n",
                                                           child2->token_info.line);
                                                 }

                                                 // code
                                                 codeStmtIf(p, child2, child0);
                                                                          

                                                 linkChild(p, child4);
                                                 linkChild(p, child3);
                                                 linkChild(p, child2);
                                                 linkChild(p, child1);
                                                 linkChild(p, child0);
                                                 push(p);
                                               }
     | IF LP Exp RP Stmt ELSE Stmt { TreeNode *p = buildNode(STMT, @$.first_line);
                                     TreeNode *child0 = pop();
                                     TreeNode *child1 = buildNode(ELSE, @6.first_line);
                                     TreeNode *child2 = pop();
                                     TreeNode *child3 = buildNode(RP, @4.first_line);
                                     TreeNode *child4 = pop();
                                     TreeNode *child5 = buildNode(LP, @2.first_line);
                                     TreeNode *child6 = buildNode(IF, @1.first_line);

                                    if( !checkIfInt( &(child4->token_info.id_type)) ) {
                                                    printf("error type 7 at line %d: variable in 'if( )' should be int\n",
                                                           child4->token_info.line);
                                    }
                                    
                                     //code
                                     codeStmtIfElse(p, child4, child2, child0);

                                     linkChild(p, child6);
                                     linkChild(p, child5);
                                     linkChild(p, child4);
                                     linkChild(p, child3);
                                     linkChild(p, child2);
                                     linkChild(p, child1);
                                     linkChild(p, child0);
                                     push(p);
                                   }
     | WHILE LP Exp RP Stmt { TreeNode *p = buildNode(STMT, @$.first_line);
                              TreeNode *child0 = pop();
                              TreeNode *child1 = buildNode(RP, @4.first_line);
                              TreeNode *child2 = pop();
                              TreeNode *child3 = buildNode(LP, @2.first_line);
                              TreeNode *child4 = buildNode(WHILE, @1.first_line);

                              if( !checkIfInt( &(child2->token_info.id_type)) ) {
                                                    printf("error type 7 at line %d: variable in 'while( )' should be int\n",
                                                           child2->token_info.line);
                              }

                              // code
                              codeStmtWhile(p, child2, child0);

                              linkChild(p, child4);
                              linkChild(p, child3);
                              linkChild(p, child2);
                              linkChild(p, child1);
                              linkChild(p, child0);
                              push(p);
                           }
     | error SEMI
     ;

DefList : { TreeNode *p = buildNode(DEFLIST, @$.first_line);
             TreeNode *child = buildNode(NOTHING, @$.first_line);

             linkChild(p, child);
             push(p);
          }
		| Def DefList { TreeNode *p = buildNode(DEFLIST, @$.first_line);
                        TreeNode *child0 = pop();
                        TreeNode *child1 = pop();

                        linkChild(p, child1);
                        linkChild(p, child0);
                        push(p);
                      }
        | error IF
        | error WHILE
        | error RETURN
        | error RC
        ;

Def : Specifier DecList SEMI { TreeNode *p = buildNode(DEF, @$.first_line);
	                           TreeNode *child0 = buildNode(SEMI, @3.first_line);
                               TreeNode *child1 = pop();
                               TreeNode *child2 = pop();

                               // DecList's type
                               copyType(&((child1->token_info).id_type), &((child2->token_info).id_type));

                               // define DecList's chidren ids
                               // local variables
                               // and generate codes
                               if(struct_defining != true) {
                                  fillDecList(child1, &((child2->token_info).id_type));
                                  codeDecList(child1, &(child1->token_info.code));
                                }

                                // Def's code
                                p->token_info.code = child1->token_info.code;

                               linkChild(p, child2);
                               linkChild(p, child1);
                               linkChild(p, child0);
                               push(p);
                             }
	| error SEMI
    | error TYPE
    | error STRUCT
	;

DecList : Dec { TreeNode *p = buildNode(DECLIST, @$.first_line);
		        TreeNode *child = pop();

		        linkChild(p, child);
                push(p);
              }
		| Dec COMMA DecList { TreeNode *p = buildNode(DECLIST, @$.first_line);
                              TreeNode *child0 = pop();
                              TreeNode *child1 = buildNode(COMMA, @2.first_line);
                              TreeNode *child2 = pop();

                              linkChild(p, child2);
                              linkChild(p, child1);
                              linkChild(p, child0);
                              push(p);
                            }
        ;

Dec : VarDec { TreeNode *p = buildNode(DEC, @$.first_line);

	           TreeNode *child = pop();
               // about array dimensions
               (p->token_info).int_value= (child->token_info).int_value;

	           linkChild(p, child);
               push(p);
             }
	| VarDec ASSIGNOP Exp { TreeNode *p = buildNode(DEC, @$.first_line);
                            TreeNode *child0 = pop();
                            TreeNode *child1 = buildNode(ASSIGNOP, @2.first_line);
                            TreeNode *child2 = pop();

                            // about array dimension
                            (p->token_info).int_value = (child2->token_info).int_value;

                            linkChild(p, child2);
                            linkChild(p, child1);
                            linkChild(p, child0);
                            push(p);
                          }
    | error COMMA
    ;

Exp : Exp ASSIGNOP Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                          TreeNode *child0 = pop();
                          TreeNode *child1 = buildNode(ASSIGNOP, @2.first_line);
                          TreeNode *child2 = pop();

                          // types checking
                          checkExpOpExp(child0, child2, p, ASSIGNOP);

                          linkChild(p, child2);
                          linkChild(p, child1);
                          linkChild(p, child0);
                          push(p);
                        }
	| Exp AND Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                    TreeNode *child0 = pop();
                    TreeNode *child1 = buildNode(AND, @2.first_line);
                    TreeNode *child2 = pop();
 
                    // types checking
                    checkExpOpExp(child0, child2, p, AND);

                    linkChild(p, child2);
                    linkChild(p, child1);
                    linkChild(p, child0);
                    push(p);
                  }       
    | Exp OR Exp{ TreeNode *p = buildNode(EXP, @$.first_line);
                  TreeNode *child0 = pop();
                  TreeNode *child1 = buildNode(OR, @2.first_line);
                  TreeNode *child2 = pop();

                  // types checking
                  checkExpOpExp(child0, child2, p, OR);

                  linkChild(p, child2);
                  linkChild(p, child1);
                  linkChild(p, child0);
                  push(p);
                }      
    | Exp RELOP Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                      TreeNode *child0 = pop();
                      TreeNode *child1 = createNode();
                      editTokenInfo(&(child1->token_info), RELOP, @2.first_line, (void *)(&$2));
                      TreeNode *child2 = pop();

                      // types checking
                      checkExpOpExp(child0, child2, p, RELOP);

                      linkChild(p, child2);
                      linkChild(p, child1);
                      linkChild(p, child0);
                      push(p);
                    }      
    | Exp PLUS Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                     TreeNode *child0 = pop();
                     TreeNode *child1 = buildNode(PLUS, @2.first_line);
                     TreeNode *child2 = pop();

                      // types checking
                      checkExpOpExp(child0, child2, p, PLUS);

                     linkChild(p, child2);
                     linkChild(p, child1);
                     linkChild(p, child0);
                     push(p);
                   }           
    | Exp MINUS Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                      TreeNode *child0 = pop();
                      TreeNode *child1 = buildNode(MINUS, @2.first_line);
                      TreeNode *child2 = pop();

                       // types checking
                      checkExpOpExp(child0, child2, p, MINUS);

                      linkChild(p, child2);
                      linkChild(p, child1);
                      linkChild(p, child0);
                      push(p);
                    }           
    | Exp STAR Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                     TreeNode *child0 = pop();
                     TreeNode *child1 = buildNode(STAR, @2.first_line);
                     TreeNode *child2 = pop();

                       // types checking
                     checkExpOpExp(child0, child2, p, STAR);

                     linkChild(p, child2);
                     linkChild(p, child1);
                     linkChild(p, child0);
                     push(p);
                    }        
    | Exp DIV Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                      TreeNode *child0 = pop();
                      TreeNode *child1 = buildNode(DIV, @2.first_line);
                      TreeNode *child2 = pop();

                      // types checking
                      checkExpOpExp(child0, child2, p, DIV);

                      linkChild(p, child2);
                      linkChild(p, child1);
                      linkChild(p, child0);
                      push(p);
                    }        
    | LP Exp RP { TreeNode *p = buildNode(EXP, @$.first_line);
                  TreeNode *child0 = buildNode(RP, @3.first_line);
                  TreeNode *child1 = pop();
                  TreeNode *child2 = buildNode(LP, @1.first_line);

                  // type
                  copyType(&(p->token_info.id_type), &(child1->token_info.id_type));

                  linkChild(p, child2);
                  linkChild(p, child1);
                  linkChild(p, child0);
                  push(p);
                }
    | MINUS Exp %prec NOT { TreeNode *p = buildNode(EXP, @$.first_line); 
                            TreeNode *child0 = pop();
                            TreeNode *child1 = buildNode(MINUS, @1.first_line);

                            // type
                            checkOpExp(child0, p, MINUS);
                            //copyType(&(p->token_info.id_type), &(child0->token_info.id_type));

                            linkChild(p, child1);
                            linkChild(p, child0);
                            push(p);
                          }
    | NOT Exp { TreeNode *p = buildNode(EXP, @$.first_line);
                TreeNode *child0 = pop();
                TreeNode *child1 = buildNode(NOT, @1.first_line);

                // type
                checkOpExp(child0, p, MINUS);

                linkChild(p, child1);
                linkChild(p, child0);
                push(p);
              }
    | ID LP Args RP { TreeNode *p = buildNode(EXP, @$.first_line);
                      TreeNode *child0 = buildNode(RP, @4.first_line);
                      TreeNode *child1 = pop();
                      TreeNode *child2 = buildNode(LP, @2.first_line);
                      TreeNode *child3 = createNode();
                      editTokenInfo(&(child3->token_info), ID, @1.first_line, (void *)$1);

                      // check and type p return_type
                      checkFunc(child3, child1, p);

                      linkChild(p, child3);
                      linkChild(p, child2);
                      linkChild(p, child1);
                      linkChild(p, child0);
                      push(p);
                    }
    | ID LP RP { TreeNode *p = buildNode(EXP, @$.first_line);
                 TreeNode *child0 = buildNode(RP, @3.first_line);
                 TreeNode *child1 = buildNode(LP, @2.first_line);
                 TreeNode *child2 = createNode();
                 editTokenInfo(&(child2->token_info), ID, @1.first_line, (void *)$1);

                 // check and type p return_type
                 checkFunc(child2, NULL, p);

                 linkChild(p, child2);
                 linkChild(p, child1);
                 linkChild(p, child0);
                 push(p);
               }
    | Exp LB Exp RB { TreeNode *p = buildNode(EXP, @$.first_line);
                      TreeNode *child0 = buildNode(RB, @4.first_line);
                      TreeNode *child1 = pop();
                      TreeNode *child2 = buildNode(LB, @2.first_line);
                      TreeNode *child3 = pop();

                      // check and type p
                      checkLbRb(child3, child1, p);

                      linkChild(p, child3);
                      linkChild(p, child2);
                      linkChild(p, child1);
                      linkChild(p, child0);
                      push(p);
                    }
    | Exp DOT ID { TreeNode *p = buildNode(EXP, @$.first_line);
                   TreeNode *child0 = createNode();
                   editTokenInfo(&(child0->token_info), ID, @3.first_line, (void *)$3);
                   TreeNode *child1 = buildNode(DOT, @2.first_line);
                   TreeNode *child2 = pop();

                   // check
                   // and type
                   checkDot(child2, child0, p);

                   linkChild(p, child2);
                   linkChild(p, child1);
                   linkChild(p, child0);
                   push(p);
                 }
    | ID { TreeNode *p = buildNode(EXP, @$.first_line);
           TreeNode *child = createNode();
           editTokenInfo(&(child->token_info), ID, @1.first_line, (void *)$1);

           // exp's type
           checkID(child, p);

           linkChild(p, child);
           push(p);
         }
    | INT { TreeNode *p = buildNode(EXP, @$.first_line);
            TreeNode *child = createNode();
            editTokenInfo(&(child->token_info), INT, @1.first_line, (void *)(&$1));

            // type
            editBasicType(&(child->token_info.id_type), INTEGER); 
            copyType(&(p->token_info.id_type), &(child->token_info.id_type));

            linkChild(p, child);
            push(p);
          }
    | FLOAT { TreeNode *p = buildNode(EXP, @$.first_line);
              TreeNode *child = createNode();
              editTokenInfo(&(child->token_info), FLOAT, @1.first_line, (void *)(&$1));

              // type
              editBasicType(&(child->token_info.id_type), DECIMAL);
              copyType(&(p->token_info.id_type), &(child->token_info.id_type));

              linkChild(p, child);
              push(p);
            }
    | error RP
    | error RB
    | error ID
    | error INT
    | error FLOAT
    | error DOT
    | error STAR
    | error DIV
    | error MINUS
    | error PLUS
    | error COMMA
    ;

Args : Exp COMMA Args { TreeNode *p = buildNode(ARGS, @$.first_line);
	                    TreeNode *child0 = pop();
                        TreeNode *child1 = buildNode(COMMA, @2.first_line);
                        TreeNode *child2 = pop();

                        linkChild(p, child2);
                        linkChild(p, child1);
                        linkChild(p, child0);
                        push(p);
                      }
	 | Exp { TreeNode *p = buildNode(ARGS, @$.first_line);
             TreeNode *child = pop();

             linkChild(p, child);
             push(p);
           }
     | error RP
     ;

%%
void yyerror(char *s) {
   B_error = 1;
   fprintf(stderr, "Error type B near Line %d: near %s\n", yylloc.first_line, yytext);

}
