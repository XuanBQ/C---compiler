#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ID NAME LENGTH
#define NAME_LENGTH 33

#define DEBUG_INFO(code)\
     printf("%s, %d\n", __FILE__, __LINE__);\
     code;

// bool
enum bool_ { false, true };
typedef enum bool_ bool;

#endif
