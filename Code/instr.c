#include "Code.h"
#include <stdio.h>

#define PRINT_A_INSTR(dfile, instr)\
	fprintf(dfile, instr)

#define PRINT_POP_PARAMS(dfile,params_num)\
	if(params_num > 0)\ 
		fprintf(dfile, "addi $fp, $fp, %d\n", (params_num)*4)

#define PRINT_RETURN_INSTRS(dfile)\
	PRINT_A_INSTR(dfile, "addi $sp, $fp, 4\nlw $fp, 0($fp)\njr $ra\n")

void printInstrsHead(FILE *dfile) {
	PRINT_A_INSTR(dfile, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
}

void printReadInstrs(FILE *dfile) {
	PRINT_A_INSTR(dfile, "read:\nmove $fp, $sp\nli $v0, 4\nla $a0, _prompt\nsyscall\nli $v0, 5\nsyscall\n");
	PRINT_RETURN_INSTRS(dfile);
}

void printWriteInstrs(FILE *dfile) {
	PRINT_A_INSTR(dfile, "write:\nmove $fp, $sp\nlw $a0, 0($fp)\nli $v0, 1\nsyscall\nli $v0, 4\nla $a0, _ret\nsyscall\nmove $v0, $0\n");
	PRINT_POP_PARAMS(dfile, 1);
	PRINT_RETURN_INSTRS(dfile);
}

void printPrepareFrame(FILE *dfile, int frame_size) {
	fprintf(dfile, "addi $sp, $sp, -%d\n", frame_size); // stack_size < 16 bits
}

#define OFFSET_IN_FRAME(operand) (-4*(operand->posInFrame))

// move the value of s to si(i <= 7)
void printVar2Reg(FILE* dfile, Operand *s, int i) {
	if(i > 7 || i < 0 ) return;

	// s is CONSTANT
	if(s->kind == CONSTANT) {
		fprintf(dfile, "li $s%d, %d\n", i, s->info);
	}
	else {
		fprintf(dfile, "lw $s%d, %d($fp)\n", i, OFFSET_IN_FRAME(s));
	}
}

void printBinOpInstr(FILE *dfile, OP op, int s, int t, int d) {
	switch(op) {
		case ADD: fprintf(dfile, "add $s%d, $s%d, $s%d\n", d, s, t); break; 
		case SUB: fprintf(dfile, "sub $s%d, $s%d, $s%d\n", d, s, t); break; 
		case MUL: fprintf(dfile, "mul $s%d, $s%d, $s%d\n", d, s, t); break; 
		case DIVI: fprintf(dfile, "div $s%d, $s%d, $s%d\nmflo $s%d\n", d, s, t, d); break; 
		default: printf("Unknown op in printBinOpInstr\n");
	}
}

void printBranch(FILE *dfile, char relop, int s, int t, int label_num) {
	switch(relop) {
		case '>': fprintf(dfile, "bgt $s%d, $s%d, label%d\n", s, t, label_num); break;
		case '<': fprintf(dfile, "blt $s%d, $s%d, label%d\n", s, t, label_num); break;
		case ',': fprintf(dfile, "ble $s%d, $s%d, label%d\n", s, t, label_num); break;
		case '.': fprintf(dfile, "bge $s%d, $s%d, label%d\n", s, t, label_num); break;
		case '=': fprintf(dfile, "beq $s%d, $s%d, label%d\n", s, t, label_num); break;
		case '!': fprintf(dfile, "bne $s%d, $s%d, label%d\n", s, t, label_num); break;
		default: printf("Unknown relop in printBranck\n");
	}
}

// prepare fp, save ra before jal
void printPrepareFpRt(FILE *dfile) {
	// save ra
	fprintf(dfile, "addi $sp, $sp, -4\nsw $ra, 0($sp)\n");
	// save fp
	fprintf(dfile, "addi $sp, $sp, -4\nsw $fp, 0($sp)\n");
}

// call a func
// and return from it
void callFunc(FILE *dfile, char *func_name) {
	// call func
	fprintf(dfile, "jal %s\n", func_name);
	// rewrite ra
	fprintf(dfile, "lw $ra, 0($sp)\n");
}

// push a arg
void pushArg(FILE *dfile, Operand *arg) {
	fprintf(dfile, "lw $t1, %d($fp)\naddi $sp, $sp, -4\nsw $t1, 0($sp)\n", OFFSET_IN_FRAME(arg));
}

void getReturnValue(FILE *dfile, Operand *d) {
	fprintf(dfile, "sw $v0, %d($fp)\n", OFFSET_IN_FRAME(d));
}

// c_head is the first "ARG ..."
int printCallFunc(FILE *dfile, Codes *c_head) {
	int offset = 0;
	Code *c = c_head->code;

	// push  ra,fp  first
	printPrepareFpRt(dfile);
	// push args
	while(c->kind == ARG) {
		pushArg(dfile, c->arg.s);
		c_head = c_head->next;
		c = c_head->code;
		offset ++;
	}
	// call func
	if(c->kind == CALL) {
		callFunc(dfile, c->call.func_name);
		getReturnValue(dfile, c->call.d);
		offset ++;
	}
	else {
		printf("c.kind != CALL in printCallFunc\n");
	}

	return offset;
}

// reutrn next code's offset
int printInstrsOfACode(FILE *dfile, Codes *c_head, int params_num) {
	Code *c = c_head->code;
	switch(c->kind) {
		case DEFINE_LABEL: fprintf(dfile, "label%d:\n", c->def_label.num); break;
	
		case MOV2VAR: { int i = 0;
						printVar2Reg(dfile, c->mov2var.s, i);
					  	fprintf(dfile, "sw $s%d, %d($fp)\n", i, OFFSET_IN_FRAME(c->mov2var.d));
					  	break;
					  }
		case MOV2MEM: { int i = 0, j = 1;
			            printVar2Reg(dfile, c->mov2mem.s, i);
			            fprintf(dfile, "lw $s%d, %d($fp)", j, OFFSET_IN_FRAME(c->mov2mem.d));
			            fprintf(dfile, "sw $s%d, 0($s%d)", i, j);
			            break;
					  }
		case BINARY_OP: { int s = 0, t = 1, d = 2;
			              printVar2Reg(dfile, c->bin_op.s, s);
			              printVar2Reg(dfile, c->bin_op.t, t);
			              printBinOpInstr(dfile, c->bin_op.op, s, t, d);
			              fprintf(dfile, "sw $s%d, %d($fp)\n", d, OFFSET_IN_FRAME(c->bin_op.d));
			              break;
						}
        case UNARY_OP: { int s = 0;
        				 if(c->unary_op.op == GET_MEM_ADDR) {
							fprintf(dfile, "lw $s%d, %d($fp)\n", s, OFFSET_IN_FRAME(c->unary_op.s));
						 }
						 fprintf(dfile, "sw $s%d, %d($fp)\n", s, OFFSET_IN_FRAME(c->unary_op.d));
						 break;
        			   }
        case GOTO: fprintf(dfile, "j label%d\n", c->def_label.num); break;
        case IF_GOTO: { int s = 0, t = 1;
        				fprintf(dfile, "lw $s%d, %d($fp)\nlw $s%d, %d($fp)\n", 
        					s, OFFSET_IN_FRAME(c->if_goto.s), t, OFFSET_IN_FRAME(c->if_goto.t));
        				printBranch(dfile, c->if_goto.relop, s, t, c->if_goto.num);
        				break;
        			  }
        case RET: { fprintf(dfile, "lw $v0, %d($fp)\n", OFFSET_IN_FRAME(c->ret.s));
        			PRINT_POP_PARAMS(dfile, params_num);
        			PRINT_RETURN_INSTRS(dfile);
        			break;
        		  }
        case ARG: return printCallFunc(dfile, c_head); // it will deal with 'call'
        //case PARAM: will not appear here
        case READ: { 
        			printPrepareFpRt(dfile);
        			callFunc(dfile, "read");
        			getReturnValue(dfile, c->read.d);
        			break;
        		   }
        case WRITE: {
        			 	printPrepareFpRt(dfile);
        			 	pushArg(dfile, c->write.s);
        			 	callFunc(dfile, "write");
        			 	break;
        			}
        case CALL: { // only the func without params will appear here
        			 printCallFunc(dfile, c_head);
        			 break;
        		    } 
        default: printf("Unknown kind in printInstrsOfAFunc, %d\n", c->kind);
	}
	return 1;
}

int printDefFunc(FILE *dfile, Codes *head, int *params_num) {
	int pnum = 0;

    // func's name and frame
	Code *c = head->code;
	fprintf(dfile, "\n%s:\n", c->def_func.name);
	fprintf(dfile, "move $fp, $sp\n");
  	printPrepareFrame(dfile, c->def_func.frame_size);

  	// params
  	head = head->next;
  	c = head->code;
  	while(c->kind == PARAM) {
  		pnum ++;
  		head = head->next;
  		c = head->code;
  	}
    
    *params_num = pnum;
    return pnum + 1;
}

void printProgramInstrs(Codes *head, char *dest_file) {
	FILE *dfile = fopen(dest_file, "w");
	printInstrsHead(dfile);
	printReadInstrs(dfile);
	fprintf(dfile, "\n");
	printWriteInstrs(dfile);
	fprintf(dfile, "\n");
		
	Codes *cur = head;  	
	int offset, params_num;
  	while(cur != NULL ) {
  		if((cur->code)->kind == DEFINE_FUNCTION) {
  			offset = printDefFunc(dfile, cur, &(params_num));
        }
        else { 
            offset = printInstrsOfACode(dfile, cur, params_num);
    		
  		}

  		// next code
  		while(offset --) {
    		cur = cur->next;
    	}
    	if(cur == head) break; // tail
  	}
}
