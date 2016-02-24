#ifndef __CODE_H__
#define __CODE_H__

#include "common.h"

// 15 Code formats
enum CodeKind_ { DEFINE_LABEL, DEFINE_FUNCTION, MOV2VAR, BINARY_OP, UNARY_OP,
                MOV2MEM, GOTO, IF_GOTO, RET, DEC_SIZE, ARG, CALL, PARAM,
				READ, WRITE
               };
typedef enum CodeKind_ CodeKind;

// kinds of Operands
enum OperandKind_ { VARIABLE, 
	                ADDRESS,
                    CONSTANT 
                  };
typedef enum OperandKind_ OperandKind;

// Oprators
enum OP_ { ADD = '+', SUB = '-', MUL = '*', DIVI = '/',
           GET_MEM_ADDR = '&', GET_MEM_CONTENT = '*' };
typedef enum OP_ OP;

// Operand structure
struct Operand_ {
	OperandKind kind;
	int info; // tempvar_no or value of constant
	int posInFrame; 
};
typedef struct Operand_ Operand;


inline Operand* constructOperand(OperandKind k, int info) {
	Operand *p = (Operand *)malloc(sizeof(Operand));
	p->kind = k;
	p->info = info;
}

// Code structure
struct Code_ {
	CodeKind kind;
	union {
		struct { int num; } def_label;                          // [LABEL labelnum	:]
		struct { char name[NAME_LENGTH]; int frame_size;} def_func;            // [FUNCTION name	:]
		struct { Operand *d, *s; } mov2var;                   // [des = s]
		struct { OP op; Operand *d, *s, *t;} bin_op;           // [des = s [op] t] 
		struct { OP op; Operand *d, *s;} unary_op;             // [des = [op]s]
		struct { Operand *d, *s;} mov2mem;                     // [*des = s]
		struct { int num; } go;                                 // GOTO labelnum
		struct { char relop; Operand *s, *t; int num; } if_goto; // [ IF a [relop] b GOTO labelnum ]
		                                                         // relop's value is from TokenInfo's 'int_value' field
																 // relop's value: < , > . = ! 
		struct { Operand *s; } ret;                              // [RETURN s]
		struct { Operand *s; int size; } dec;                    // [DEC s [size] (number of bytes)
		struct { Operand *s; } arg;                              // [ARG s]
		struct { Operand *d; char func_name[NAME_LENGTH]; } call;// [CALL func_name]
		struct { Operand *s; } param;                            // [PARAM s]
		struct { Operand *d; } read;                             // [READ d]
		struct { Operand *s; } write;                            // [WRITE d]
	};
};
typedef struct Code_ Code;

// store codes
struct Codes_ {
	Code *code;  // if code is NULL, it's the head of codes list,
	             // and it's 'next' is the first code of the program
	struct Codes_ *prev, *next;
};
typedef struct Codes_ Codes;
extern Codes* codes_head; // first node
Codes* linkTwoCodes(Codes* prev_code, Codes* code); // link a code to another code
Codes* newCodes(Code *); // return the new list's first node
Codes* addCodesToList(Codes* des_head, Codes* codes); // add a codes' list to another list
Codes* linkCodes(int n, ...); // link n Codes
void initCodes(); // init codes_head
void printProgramCodes(Codes *head);
bool printACode(Code *c);
void freeProgramCodes(Codes *head);

inline Code* newCode() {
	Code* p = (Code *)malloc(sizeof(Code));
	return p;
}  
inline Codes* buildDefLabelCode(int n) {
    Code* p = newCode();
    p->kind = DEFINE_LABEL;
    (p->def_label).num = n;
    return newCodes(p);
}
inline Codes* buildDefFuncCode(char* name, int fsize) {
	Code* p = newCode();
	p->kind = DEFINE_FUNCTION;
	p->def_func.frame_size = fsize;
	strcpy((p->def_func).name, name);
	return newCodes(p);
}
inline Codes* buildMov2VarCode(Operand *des, Operand *s) {
	Code* p = newCode();
	p->kind = MOV2VAR;
	(p->mov2var).d = des;
	(p->mov2var).s = s;
	return newCodes(p);
}
inline Codes* buildBinOpCode(OP o, Operand* des, Operand* s, Operand* t) {
	Code* p = newCode();
	p->kind = BINARY_OP;
	(p->bin_op).op = o;
	(p->bin_op).d = des;
	(p->bin_op).s = s;
	(p->bin_op).t = t;
	return newCodes(p);
}
inline Codes* buildUnaryOpCode(OP o, Operand* des, Operand* s) {
	Code* p = newCode();
	p->kind = UNARY_OP;
	(p->unary_op).op = o;
	(p->unary_op).d = des;
	(p->unary_op).s = s;
	return newCodes(p);
}
inline Codes* buildMov2MemCode(Operand *des, Operand *s) {
	Code* p = newCode();
	p->kind = MOV2MEM;
	(p->mov2mem).d = des;
	(p->mov2mem).s = s;
	return newCodes(p);
}
inline Codes* buildGoCode(int n) {
	Code* p = newCode();
	p->kind = GOTO;
	(p->go).num = n;
	return newCodes(p);
}
inline Codes* buildIfGotoCode(char r, Operand *s, Operand *t, int n) {
	Code* p = newCode();
	p->kind = IF_GOTO;
	(p->if_goto).relop = r;
	(p->if_goto).s = s;
	(p->if_goto).t = t;
	(p->if_goto).num = n;
	return newCodes(p);
}
inline Codes* buildRetCode(Operand *s) {
	Code* p = newCode();
	p->kind = RET;
	(p->ret).s = s;
	return newCodes(p);
}
inline Codes* buildDecCode(Operand *s, int size) {
	Code* p = newCode();
	p->kind = DEC_SIZE;
	(p->dec).s = s;
	(p->dec).size = size;
	return newCodes(p);
}
inline Codes* buildArgCode(Operand *s) {
	Code* p = newCode();
	p->kind = ARG;
    (p->arg).s = s;
    return newCodes(p);
}
inline Codes* buildCallCode(Operand *d, char *name) {
	Code* p = newCode();
	p->kind = CALL;
	(p->call).d = d;
	strcpy((p->call).func_name, name);
	return newCodes(p);
}
inline Codes* buildParamCode(Operand *s) {
	Code* p = newCode();
	p->kind = PARAM;
	(p->param).s = s;
	return newCodes(p);
}
inline Codes* buildReadCode(Operand *d) {
	Code* p = newCode();
	p->kind = READ;
	(p->read).d = d;	
	return newCodes(p);
}
inline Codes* buildWriteCode(Operand *s) {
	Code* p = newCode();
	p->kind = WRITE;
	(p->write).s = s;
	return newCodes(p);
}

// label pool
struct Label_ {
	int num;
	struct Label_ *next;
};
typedef struct Label_ Label;

// temp variable pool
struct TempVar_ {
	int num;
	struct TempVar_ *next;
};
typedef struct TempVar_ TempVar;

void initLabelPool();
void initTempvPool();
int newTempv();
Operand *newVar();
Operand *newAddr();
void freeTempv(int);
void freeOerand(Operand *);
int newLabel();
void freeLabel(int n);
void freeTempsInFunc(Codes *head);
int pushS();
int popS();
int getSpIndex();
void newFuncFrame();

#endif