#pragma once

// detecting memory leaks (remove to disable)
#define MEM_LEAKS_DETECTION 1

#if defined(MEM_LEAKS_DETECTION)
#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#else
#define DBG_NEW new
#endif

