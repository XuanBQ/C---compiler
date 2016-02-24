#ifndef __TokenId_H__
#define __TokenId_H__

//convert id to the index in the name array
#define TOKEN_NUM(id) ((id) - 258)

//get the string by id
#define TOKEN_NAME(id) (TokenName[TOKEN_NUM(id)])

/* TOKENNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const TokenName[] =
{
  "ASSIGNOP", "OR", "AND", "RELOP",
  "MINUS", "PLUS", "STAR", "DIV", "NOT", "LB", "RB", "LP", "RP", "DOT",
  "INT", "FLOAT", "ID", "TYPE", "SEMI", "COMMA", "RC", "LC",
  "LOWER_THAN_ELSE", "WHILE", "ELSE", "IF", "RETURN", "STRUCT", "$accept",
  "Program", "ExtDefList", "ExtDef", "ExtDecList", "Specifier",
  "StructSpecifier", "OptTag", "Tag", "VarDec", "FunDec", "VarList",
  "ParamDec", "CompSt", "StmtList", "Stmt", "DefList", "Def", "DecList",
  "Dec", "Exp", "Args", "NOTHING", 0
};

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ASSIGNOP = 258,
     OR = 259,
     AND = 260,
     RELOP = 261,
     MINUS = 262,
     PLUS = 263,
     STAR = 264,
     DIV = 265,
     NOT = 266,
     LB = 267,
     RB = 268,
     LP = 269,
     RP = 270,
     DOT = 271,
     INT = 272,
     FLOAT = 273,
     ID = 274,
     TYPE = 275,
     SEMI = 276,
     COMMA = 277,
     RC = 278,
     LC = 279,
     LOWER_THAN_ELSE = 280,
     WHILE = 281,
     ELSE = 282,
     IF = 283,
     RETURN = 284,
     STRUCT = 285
   };
#endif

//nonterminal token id
enum NTokenId {
	PROGRAM = 287,
	EXTDEFLIST = 288,
	EXTDEF = 289,
	EXTDECLIST = 290,
	SPECIFIER = 291,
	STRUCTSPECIFIER = 292,
	OPTTAG = 293,
	TAG = 294,
	VARDEC = 295,
	FUNDEC = 296,
	VARLIST = 297,
	PARAMDEC = 298,
	COMPST = 299,
	STMTLIST = 300,
	STMT = 301,
	DEFLIST = 302,
	DEF = 303,
	DECLIST = 304,
	DEC = 305,
	EXP = 306,
	ARGS = 307,
  NOTHING = 308  //empty string
};

#endif
