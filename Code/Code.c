#include "Code.h"
#include "SymbolTable.h"

#define __CODES_DEBUG__

static FILE *output_file;
#define PRINTCODE(format, ...)\
   fprintf(output_file, format, ## __VA_ARGS__)

// init codes_head and pools
Codes* codes_head;
extern int argN;
inline void initCodes() {
   codes_head = NULL;
   argN = 0;
   output_file = fopen("codes.ir", "w");
   initLabelPool();
   initTempvPool();
}

// return the new list's first node
Codes* newCodes(Code* c) {
  Codes* p = (Codes*)malloc(sizeof(Codes));
    p->code = c;
    p->next = p;
    p->prev = p;
    return p;
}

// link a code after another code
// ensure that there only one code in argument 'code'
Codes* linkTwoCodes(Codes* prev_code, Codes* code) {
   Codes* next_code = prev_code->next;
   prev_code->next = code;
   next_code->prev = code;
   code->next = next_code;
   code->prev = prev_code;

   return prev_code;
}

static inline Codes* getTail(Codes* head) {
	return head->prev;
} 

// add a codes' list to another list
// des_head and codes can be NULL
Codes* addCodesToList(Codes* des_head, Codes* codes) {
	if(des_head == NULL && codes != NULL) return codes;
  if(des_head != NULL && codes == NULL) return des_head;
  if(des_head == NULL && codes == NULL) return NULL;
	
    Codes* des_tail = getTail(des_head);
    Codes* codes_tail = getTail(codes);

    codes->prev = des_tail;
    codes_tail->next = des_head;
    
    des_head->prev = codes_tail;
    des_tail->next = codes;

    return des_head;
}

// to link n codes
// n >= 2
static inline Codes* vlinkCodes(int n, Codes** arg_bottom) {
  if(n >= 2) {
    int i;
    Codes* codes_head = *arg_bottom;
    Codes** next = arg_bottom + 1;
    for(i = 1; i < n; i ++) { // link n-1 codes to head
      codes_head = addCodesToList(codes_head, *next);
      next = next + 1;
    }
    return codes_head;
  }
  else {
    fprintf(stderr, "%s, %d, link less than 1 codes\n", __FILE__, __LINE__);
  }
}

// prepare arguments for vlinkCodes
Codes* linkCodes(int n, ...) {
  Codes** arg_bottom= (Codes**)(&n) + 1;
  return vlinkCodes(n, arg_bottom);
}

#define POOL_LENGTH 512
static Label LabelPool[POOL_LENGTH];
static Label* label_used_head, *label_free_head;

static TempVar TempvPool[POOL_LENGTH];
static TempVar *tempv_used_head, *tempv_free_head;

void initLabelPool() {
   int i = 0;

   // used_list
   label_used_head = &(LabelPool[i++]);
   label_used_head->next = NULL;
   
   // free_list
   label_free_head = &(LabelPool[i]); // free_list's head
   for(;i < POOL_LENGTH - 1; i ++) {
   	 LabelPool[i].num = i - 1; // 0 1 2 3 4 ...., and 0 for head
   	 LabelPool[i].next = &(LabelPool[i + 1]);
   }  
   // tail
   LabelPool[i].num = i - 1;
   LabelPool[i].next = NULL;
}

void initTempvPool() {
   int i;

   // used_list
   tempv_used_head = &(TempvPool[0]);
   tempv_used_head->next = NULL;
   
   // free_list
   tempv_free_head = &(TempvPool[1]); // free_list's head
   for(i = 1;i < POOL_LENGTH - 1; i ++) {
   	 TempvPool[i].num = i - 1; // 0 1 2 3 4 ...., and 0 for head
   	 TempvPool[i].next = &(TempvPool[i + 1]);
   }  
   // tail
   TempvPool[i].num = i - 1;
   TempvPool[i].next = NULL;
}

// success, return a number bigger than 0
// fail, return 0
int newTempv() {
   if(tempv_free_head->next != NULL) {
   	  // get p from free_list
   	  TempVar* p = tempv_free_head->next;
   	  tempv_free_head->next = p->next;
      
      // put p into used_list
   	  p->next = tempv_used_head->next;
   	  tempv_used_head->next = p;

   	  return p->num;
   }

   return 0;
}

static int spIndex;
int pushS() {
  return ++spIndex;
}
int popS() {
  return --spIndex;
}
int getSpIndex() {
  return spIndex;
}
void newFuncFrame() {
  spIndex = 0;
}
Operand *newVar() {
  int t;
  if(t = newTempv()) {
    Operand *p = constructOperand(VARIABLE, t);
    // allocate stack mem
    p->posInFrame = pushS();
    return p;
  }
  else {
    #ifdef __CODES_DEBUG__
    fprintf(stderr, "%s, %d, failed: create an variable\n", __FILE__, __LINE__);
    #endif
  }
}

Operand *newAddr() {
  int t;
  if(t = newTempv()) {
    Operand *p = constructOperand(ADDRESS, t);
    // allocate stack mem
    pushS(&(p->posInFrame));
    return p;
  }
  else {
    #ifdef __CODES_DEBUG__
    fprintf(stderr, "%s, %d, failed: create an variable\n", __FILE__, __LINE__);
    #endif
  }
}  

void freeTempv(int num) {
   TempVar* t = tempv_used_head;
   
   // searching the used_list
   while((t = t->next) != NULL) {
   	 if(t->num != num) continue;
   	 else {// find it
   	 	// remove from used_list
   	 	tempv_used_head->next = t->next;

        // put into free_list
   	 	t->next = tempv_free_head->next;
   	 	tempv_free_head->next = t;
   	 }
   }
}

void freeTempvInOperand(Operand *s) {
  if(s->kind != CONSTANT) {
    freeTempv(s->info);
  }
}

void freeOperand(Operand** oper) {
  Operand *s = *oper;
  if(s != NULL) {
    free(s);
    *oper = NULL;
  }
}

int newLabel() {
   if(label_free_head->next != NULL) {
   	  // get p from free_list
   	  Label* p = label_free_head->next;
   	  label_free_head->next = p->next;
      
      // put p into used_list
   	  p->next = label_used_head->next;
   	  label_used_head->next = p;

   	  return p->num;
   }

   return 0;
}

void freeLabel(int num) {
   Label* t = label_used_head;
   
   // searching the used_list
   while((t = t->next) != NULL) {
   	 if(t->num != num) continue;
   	 else {// find it
   	 	// remove from used_list
   	 	label_used_head->next = t->next;

        // put into free_list
   	 	t->next = label_free_head->next;
   	 	label_free_head->next = t;
   	 }
   }
}

static inline void printOperand(Operand *p) {
  if(p == NULL) return;
  switch(p->kind) {
    case VARIABLE:
    case ADDRESS:  PRINTCODE("t%d", p->info);
                   break;
    case CONSTANT: PRINTCODE("#%d", p->info);
    default: break;
  }
}

static inline void printRelop(char relop) {
  switch(relop) {
    case '!' : PRINTCODE("!="); break;
    case ',' : PRINTCODE("<="); break;
    case '.' : PRINTCODE(">="); break;
    case '=' : PRINTCODE("=="); break;
    default  : PRINTCODE("%c", relop); break;
  }
}

static inline void printOP(char op) {
  PRINTCODE("%c", op);
}

bool printACode(Code *c) {
  switch(c->kind) {
    case DEFINE_LABEL: PRINTCODE("LABEL label%d :", c->def_label.num); 
                       return true;
    case DEFINE_FUNCTION: PRINTCODE("\nFUNCTION %s :", c->def_func.name);
                          return true;
    case MOV2VAR: if(c->mov2var.d != NULL) { // has destination
                     printOperand(c->mov2var.d);
                     PRINTCODE(" := ");
                     printOperand(c->mov2var.s);
                     return true;
                   }
                   return false;
    case MOV2MEM:  PRINTCODE("*");
                   printOperand(c->mov2mem.d);
                   PRINTCODE(" := ");
                   printOperand(c->mov2mem.s);
                   return true;
    case BINARY_OP: printOperand(c->bin_op.d);
                    PRINTCODE(" := ");
                    printOperand(c->bin_op.s);
                    PRINTCODE(" ");
                    printOP(c->bin_op.op);
                    PRINTCODE(" ");
                    printOperand(c->bin_op.t);
                    return true;
    case UNARY_OP: printOperand(c->unary_op.d);
                   PRINTCODE(" := ");
                   printOP(c->unary_op.op);
                   printOperand(c->unary_op.s);
                   return true;
    case GOTO: PRINTCODE("GOTO label%d", c->go.num);
               return true;
    case IF_GOTO: PRINTCODE("IF ");
                  printOperand(c->if_goto.s);
                  PRINTCODE(" ");
                  printRelop(c->if_goto.relop);
                  PRINTCODE(" ");
                  printOperand(c->if_goto.t);
                  PRINTCODE(" GOTO label%d", c->if_goto.num);
                  return true;
    case RET: PRINTCODE("RETURN ");
                 printOperand(c->ret.s);
                 return true;
    case DEC_SIZE: PRINTCODE("DEC ");
                   printOperand(c->dec.s);
                   PRINTCODE(" %d", c->dec.size);
                   return true;
    case ARG: PRINTCODE("ARG ");
              printOperand(c->arg.s);
              return true;
    case CALL: if(c->call.d != NULL) 
                    printOperand(c->call.d);
               else PRINTCODE("t");
               PRINTCODE(" := CALL %s", c->call.func_name);
               return true;
    case PARAM: PRINTCODE("PARAM ");
                printOperand(c->param.s);
                return true;
    case READ: PRINTCODE("READ ");
               printOperand(c->read.d);
               return true;
    case WRITE: PRINTCODE("WRITE ");
                printOperand(c->write.s);
                return true;
    default: return false;
  }
}
void printProgramCodes(Codes *head) {
  Codes *cur = head;
  while(cur != NULL ) {
    if( printACode(cur->code) ) PRINTCODE("\n"); // print a line (not empty)
    cur = cur->next;
    if(cur == head) break; // tail
  }
} 

static void freeACode(Code *c) {
  // free Operands
  switch(c->kind) {
    case MOV2VAR: free(c->mov2var.d);
                  free(c->mov2var.s);
                  break;

    case IF_GOTO: free(c->if_goto.s);
                  free(c->if_goto.t);
                  break;
                  
    case RET: free(c->ret.s);
                 break;
                 
    case DEC_SIZE: free(c->dec.s);
                   break;
              
    case ARG: free(c->arg.s);
              break;
              
    case CALL: free(c->call.d);
               break;

    case PARAM: free(c->param.s);
                break;
                
    case READ: free(c->read.d);
               break;
               
    case WRITE: free(c->write.s);
                break;
                
    default: break;
  }  
  free(c);
}

static void freeTempsInACode(Code *c) {
  switch(c->kind) {

    case MOV2VAR: if(c->mov2var.d != NULL) { // has destination
                     freeTempvInOperand(c->mov2var.d);
                     freeTempvInOperand(c->mov2var.s);
                   }
                   break;
      
    case MOV2MEM:  freeTempvInOperand(c->mov2mem.d);
                   freeTempvInOperand(c->mov2mem.s);
                   break;

    case BINARY_OP: freeTempvInOperand(c->bin_op.d);
                    freeTempvInOperand(c->bin_op.s);
                    freeTempvInOperand(c->bin_op.t);
                    break;

    case UNARY_OP: freeTempvInOperand(c->unary_op.d);
                   freeTempvInOperand(c->unary_op.s);
                   break;

    case IF_GOTO: freeTempvInOperand(c->if_goto.s);
                  freeTempvInOperand(c->if_goto.t);
                  break;
                  
    case RET: freeTempvInOperand(c->ret.s);
              break;
                 
    case DEC_SIZE: freeTempvInOperand(c->dec.s);
                   break;

    case ARG: freeTempvInOperand(c->arg.s);
              break;
              
    case CALL: if(c->call.d != NULL) { 
                  freeTempvInOperand(c->call.d);
               }
               break;

    case PARAM: freeTempvInOperand(c->param.s);
                break;
                
    case READ: freeTempvInOperand(c->read.d);
               break;
               
    case WRITE: freeTempvInOperand(c->write.s);
                break;
               
    default: break;
  }
}

void freeTempsInFunc(Codes *head) {
  Codes *cur = head;
  while(cur != NULL ) {
    freeTempsInACode(cur->code);
    cur = cur->next;
    if(cur == head) break; // tail
  }
}

void freeProgramCodes(Codes *head) {
  Codes *cur = head, *tail = head->prev;
  
  while(cur != NULL ) {
    Codes *temp = cur;
    cur = cur->next;

    freeACode(temp->code);
    free(temp);
    
    if(cur == tail) break; // tail
  }
  freeACode(tail->code);
  free(tail);
}

