#include "TypeInfo.h"
Type* createType() {
	Type* t = (Type*)malloc(sizeof(Type));
	return t;
}

void copyType(Type* dest, Type* source) {
	if(source != NULL)
	  memcpy(dest, source, sizeof(Type));
}

bool compareType(Type* s, Type* t) {
	if(s->kind != t->kind) return false;

	switch(s->kind) {
		case BASIC: if(s->type == t->type) return true; 
		            break;
		case ARRAY: if( (s->array).dim == (t->array).dim 
			            && compareType(s->array.elem, t->array.elem) ) return true;
			        break; 
		case STRUCTTAG:
		case STRUCTURE: if( s->list == t->list ) return true;
		                break;
		// case FUNCTION: 
		default: return false;
	}

	return false;
}

void editBasicType(Type* dest, BasicType id_type) {
	dest->kind = BASIC;
	dest->type = id_type;
}

void editArrayType(Type* dest, Type* elem_type, int dim, int length) {
	dest->kind = ARRAY;
	dest->array.dim = dim;
	dest->array.elem = elem_type;
	// exchange length
	if(elem_type->kind == ARRAY) {
		dest->array.length = elem_type->array.length;
		elem_type->array.length = length;
	}
	else {
		dest->array.length = length;
	}
}

void editStructType(Type* dest, SymbolList* structure) {
	dest->kind = STRUCTURE;
	dest->list = structure;
}

void editFuncType(Type* dest, SymbolList* func_args) {
	dest->kind = FUNCTION;
	dest->list = func_args;
}

void editTagType(Type* dest, SymbolList* structure) {
	dest->kind = STRUCTTAG;
	dest->list = structure;
}

void freeType(Type* dest) {
	switch(dest->kind) {
		case BASIC: break;
		case ARRAY: break; //return freeType((dest->array).elem);
		case STRUCTURE: break; 
		case STRUCTTAG: return freeList(dest->list);
		case FUNCTION: break;
		default: return;
	}
}

SymbolList* createListNode() {
	SymbolList* p = (SymbolList*)malloc(sizeof(SymbolList));
	p -> next = NULL;
	return p;
}

// add to tail
void addListNode(SymbolList* dest, char* name, Type* t) {
	SymbolList* tail;
	while(dest != NULL) {
		tail = dest;
		dest = dest->next;
	}

	// new list_node
	SymbolList* p = createListNode();

	// edit list_node
	strcpy(p->name, name);
	copyType(&(p->type), t);

	// link
	tail->next = p;
	p->next = NULL;
}

void freeList(SymbolList* first) {
	while(first != NULL) {
		SymbolList* temp = first;
		first = first->next;
		
		free(temp);
	}
}

void print_list(SymbolList* head) {
    SymbolList* cur = head->next;
	while(cur != NULL) {
		printf("name: %s; type: ", cur->name);  
		print_type(&(cur->type));
		cur = cur->next;
	}

}

void print_type(Type* t) { 
	// kind
	if(t->kind == BASIC) {
		if(t->type == INTEGER) 
			printf("BASIC: INTEGER\n");
		else if(t->type == DECIMAL) 
			printf("BASIC: DECIMAL\n");
	}
	else if(t->kind == ARRAY) {
		printf("ARRAY: %d, length: %d, type: ", t->array.dim, t->array.length);
		print_type( t->array.elem );
	}
	else if(t->kind == STRUCTURE) {
		printf("STRUCTURE: ");
		print_list(t->list);
	}
	else if(t->kind == STRUCTTAG) {
		printf("STRUCTTAG: ");
		print_list(t->list);
	}
	else if(t->kind == FUNCTION) {
		printf("FUNCTION: args: ");
		print_list(t->list);
	}
	else {
		printf("NO THIS TYPE\n");
	}
}

bool checkIfInt(Type* t) {
	if(t->kind == BASIC && t->type == INTEGER)
		return true;
	else 
		return false;
}

bool checkIfFloat(Type* t) {
	if(t->kind == BASIC && t->type == DECIMAL) 
		return true;
	else
		return false;
}

bool checkIfFunc(Type* t) {
	if(t->kind == FUNCTION)
		return true;
	else
		return false;
}

bool checkIfStruct(Type* t) {
	if(t->kind == STRUCTURE) 
		return true;
	else
		return false;
}

bool checkIfStructTag(Type* t) {
	if(t->kind == STRUCTTAG)
		return true;
	else
		return false;
}

int getDim(Type* t) {
	if(t->kind == ARRAY) {
		return t->array.dim;
	}
	else return -1;
}

bool checkIfArray(Type *t) {
    return t->kind == ARRAY;
}

static int getArrayLength(Type *t) {
    if(checkIfArray(t) == true) {
		return t->array.length;
	}
	else return 1;	// not an array
}

// return how many bytes are under this type 
int getSubSize(Type *t) {
	if(checkIfArray(t) == true) {
		Type* e = t->array.elem;
		return getSize(e);
	}
	else return 4;	
}

// return the space's length(bytes) this type need
int getSize(Type *t) {
	return getSubSize(t) * getArrayLength(t);
}

SymbolList* getListNode(SymbolList* head, char* name) {
	SymbolList* node = head->next;
	while(node != NULL) {
		if( strcmp(node->name, name) == 0 ) {
			return node;
		}
		node = node->next;
	}

	return NULL;
}




