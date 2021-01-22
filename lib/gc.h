
#ifndef gcGarbageCollector
#define gcGarbageCollector

#include "cxx.h"


typedef void(*gcHashDictValue_t)(void*);

typedef int gcKeyLength_t ;

struct gcKeyNode_s 
{
    struct gcKeyNode_s *    next;
    char *                  key;
    gcKeyLength_t           len;
    gcHashDictValue_t       dtor;
};

struct gc_s 
{
    struct gcKeyNode_s **    table;
    int                      length, count;
    double                   growth_treshold;
    double                   growth_factor;
    gcHashDictValue_t *      dtor;
};

void            cb_fclose    (void*ptr) ;
struct gc_s*    gc_new       (int initial_size);
void            gc_del       (struct gc_s* gc);
int             gcAdd        (struct gc_s* gc,void* key,gcHashDictValue_t dtor);
int             gc_find      (struct gc_s* gc, void *key, int keyn) ;
void*           gcMalloc_    (struct gc_s* gc , size_t size) ;
void*           gcCalloc_    (struct gc_s* gc , size_t SIZEOF , size_t size) ;
void*           gcFree_      (struct gc_s* gc , void* ptr) ;
void*           gcRealloc_   (struct gc_s* gc , void* ptr, size_t size) ;
void            gcPrint_     (struct gc_s* gc );
void*           gcFileOpen_  (struct gc_s* gc ,char* fileName, char* action);
FILE*           gcFileTemp   (void) ;

#define         gcStart(...)            GC=gc_new(0)/*__VA_ARGS__*/
#define         gcStop(...)             gc_del(GC)/*__VA_ARGS__*/
#define         gcRealloc(ptr,size)     gcRealloc_(GC,ptr,size)
#define         gcFree(ptr)             gcFree_(GC,ptr)
#define         gcMalloc(size)          gcMalloc_(GC,size)
#define         gcCalloc(SIZEOF,size)   gcCalloc_(GC,SIZEOF,size)
#define         gcPrint(...)            gcPrint_(GC)/*__VA_ARGS__*/
#define         gcFileOpen(pf,mode)     gcFileOpen_(GC,pf,mode)
#define         gcFileClose(ptr)        gcFree_(GC,ptr)

extern struct gc_s* GC ;

// ......................................... [] dup functioon

char*       gcStrDup    (char *s ) ;

wchar_t*    gcWcsDup    (wchar_t *s ) ;

int*        gcIntDup    (int val) ;

double*     gcDoubleDup (double val )  ;

// ......................................... [] compare functioon

int    gcCompareInt        ( const void * a, const void * b ) ;

int    gcCompareFloat      ( const void * a, const void * b ) ;

int    gcCompareDouble     ( const void * a, const void * b ) ;

int    gcCompareFloatAsInt ( const void * a, const void * b ) ;

int gcCompareDoubleAsInt   ( const void * a, const void * b ) ;

int gcCompareStrC          ( const void * a, const void * b ) ;

int gcCompareWStrC         ( const void * a, const void * b ) ;

int gcComparepStrC         ( const void * a, const void * b ) ;

int gcComparepWStrC        ( const void * a, const void * b ) ;


#endif



/**/


