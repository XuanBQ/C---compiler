#ifndef __TYPEINFO_H__
#define __TYPEINFO_H__

#include "common.h"

typedef struct Type_ Type;
typedef struct SymbolList_ SymbolList;

enum BasicType_ {
	INTEGER = 400,
	DECIMAL = 401
};
typedef enum BasicType_ BasicType;

enum IDKind_ {
	BASIC = 402,
	ARRAY = 403,
	STRUCTURE = 404, // struct variable
	FUNCTION = 405,
	STRUCTTAG = 406  //struct type's name
};
typedef enum IDKind_ IDKind;

struct Type_ {
	IDKind kind;
	union {
		// basic type
	    BasicType type;	

		// array: element's type, amount of its dimensions, 
		// and its length(index range) 
		struct { Type* elem; int dim; int length;} array;

		// struct field info
		// and function args info
		SymbolList* list;
	};
};

Type* createType();
void copyType(Type* dest, Type* source);

// if two types are equal, return true
bool compareType(Type* s, Type* t);

void editBasicType(Type* dest, BasicType basic_type); 
void editArrayType(Type* dest, Type* elem_type, int dim, int size);
void editStructType(Type* dest, SymbolList* structure);
void editFuncType(Type* dest, SymbolList* func_args);
void editTagType(Type* dest, SymbolList* structure);

bool checkIfInt(Type*);
bool checkIfFloat(Type*);
bool checkIfStructTag(Type*);
bool checkIfStruct(Type*);
bool checkIfFunc(Type*);
bool checkIfArray(Type *t);
int getDim(Type*);
int getSubSize(Type *t);
int getSize(Type*);


void freeType(Type* dest);

struct SymbolList_ {
	char name[NAME_LENGTH];
	Type type;
	SymbolList* next;
};

SymbolList* createListNode();
SymbolList* getListNode(SymbolList* head, char* name);
 
// add to tail
void addListNode(SymbolList* dest, char* name, Type* t);

void freeList(SymbolList* first);

void print_type(Type* type);

void print_list(SymbolList* head);

#endif
