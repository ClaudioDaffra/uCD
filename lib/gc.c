
#include "gc.h"

#define gcHashFunc gcMeyan

// global

struct gc_s* GC=NULL ;

// n.b.
//
// 1) I confronti vengono fatti con memcmp .
// 2) Le chiavi vengono duplicate all'iterno della tavola 
//

// ........................................... gc hash function

static 
inline uint32_t gcMeyan(const char *key, int count) 
{
    typedef uint32_t* P;
    uint32_t h = 0x811c9dc5;
    while (count >= 8) {
    h = (h ^ ((((*(P)key) << 5) | ((*(P)key) >> 27)) ^ *(P)(key + 4))) * 0xad3e7;
    count -= 8;
    key += 8;
    }
    #define tmp h = (h ^ *(uint16_t*)key) * 0xad3e7; key += 2;
    if (count & 4) { tmp tmp }
    if (count & 2) { tmp }
    if (count & 1) { h = (h ^ *key) * 0xad3e7; }
    #undef tmp
    return h ^ (h >> 16);
}

// ........................................... gc new node

struct gcKeyNode_s *gc_node_new(char*k, int l) 
{
    if(GC==NULL) { printf("!! GC==NULL\n"); exit(-1);} ;
    
    struct gcKeyNode_s *node = (struct gcKeyNode_s *) malloc(sizeof(struct gcKeyNode_s));
    node->len = l;
    node->key = (char*) malloc(l);
    memcpy(node->key, k, l);
    node->next = 0;
    node->dtor = NULL;
    return node;
}

// ........................................... gc delete node

static
void gc_node_del(struct gcKeyNode_s *node) 
{

    if ( node->dtor!=NULL )
    { 
        union {
            char  ptrc[8];
            void* ptr;
        } pkey ;
        
        #if defined(_MSC_VER)
        #pragma warning(disable:4996)
        #endif
        strncpy( pkey.ptrc , node->key , 8 ) ;

        (node->dtor)(pkey.ptr); // CALL DESTRUCTOR

    }
    if ( node->key!=NULL ) 
    { 
        free(node->key); 
        node->key=NULL ;
    }
    
    if (node->next) gc_node_del(node->next);
    
    free(node);
}

// ........................................... gc new

struct gc_s* gc_new(int initial_size) 
{
    struct gc_s* gc = (struct gc_s *)malloc(sizeof(struct gc_s));
    if (initial_size == 0) initial_size = 1024;
    gc->length = initial_size;
    gc->count = 0;
    gc->table =  calloc(sizeof(struct gcKeyNode_s*), initial_size);
    gc->growth_treshold = 2.0;
    gc->growth_factor = 10;
    return gc;
}

// ........................................... gc delete

void gc_del(struct gc_s* gc) 
{
    for (int i = 0; i < gc->length; i++) 
    {
        if (gc->table[i])
        gc_node_del(gc->table[i]);
    }
    free(gc->table);
    gc->table = 0;
    free(gc);
    
    //.......................... close stream
    
    #if defined(__MINGW32__) || defined(__MINGW64__)
    fcloseall();
    flushall();
    #endif
    
    #if defined(__clang__) || defined(__APPLE__)
    //fcloseall();
    fflush(NULL);
    #endif    
    
    #if defined(_WIN32) || defined(_WIN64)
    _fcloseall();
    _flushall();
    #endif     
    
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);    
    
}

// ........................................... gc reinsert when resizing

static
void gc_reinsert_when_resizing(struct gc_s* gc, struct gcKeyNode_s *k2) 
{
    int n = gcHashFunc(k2->key, k2->len) % gc->length;
    if (gc->table[n] == 0) 
    {
        gc->table[n] = k2;
        gc->dtor = &gc->table[n]->dtor;
        return;
    }
    struct gcKeyNode_s *k = gc->table[n];
    k2->next = k;
    gc->table[n] = k2;
    gc->dtor = &k2->dtor;
}

// ........................................... gc  resize

static
void gc_resize(struct gc_s* gc, int newsize) 
{
    int o = gc->length;
    struct gcKeyNode_s **old = gc->table;
    gc->table = calloc(sizeof(struct gcKeyNode_s*), newsize);
    gc->length = newsize;
    for (int i = 0; i < o; i++) 
    {
        struct gcKeyNode_s *k = old[i];
        while (k) 
        {
            struct gcKeyNode_s *next = k->next;
            k->next = 0;
            gc_reinsert_when_resizing(gc, k);
            k = next;
        }
    }
    free(old);
}

// ........................................... gc add

static
int gc_add(struct gc_s* gc, void *key, int keyn)
{
    assert(gc!=NULL);
    
    int n = gcHashFunc((const char*)key, keyn) % gc->length;
    if (gc->table[n] == 0)
    {
        double f = (double)gc->count / (double)gc->length;
        if (f > gc->growth_treshold)
        {
            #if defined(_MSC_VER)
            #pragma warning(disable:4244)
            #endif
            gc_resize(gc, gc->length * gc->growth_factor);
            return gc_add(gc, key, keyn);
        }
        gc->table[n] = gc_node_new((char*)key, keyn);
        gc->dtor = &gc->table[n]->dtor;
        gc->count++;
        return 0;
    }
    struct gcKeyNode_s *k = gc->table[n];
    while (k) 
    {
        if (k->len == keyn && memcmp(k->key, key, keyn) == 0) 
        {
            gc->dtor = &k->dtor;
            return 1;
        }
        k = k->next;
    }
    gc->count++;
    struct gcKeyNode_s *k2 = gc_node_new((char*)key, keyn);
    k2->next = gc->table[n];
    gc->table[n] = k2;
    gc->dtor = &k2->dtor;
    return 0;
}

// ........................................... gc find

int gc_find(struct gc_s* gc, void *key, int keyn) 
{
    int n = gcHashFunc((const char*)key, keyn) % gc->length;
    #if defined(__MINGW32__) || defined(__MINGW64__)
    __builtin_prefetch(gc->table[n]);
    #endif
    
    #if defined(_WIN32) || defined(_WIN64)
    _mm_prefetch((char*)gc->table[n], _MM_HINT_T0);
    #endif
    
    struct gcKeyNode_s *k = gc->table[n];
    if (!k) return 0;
    while (k) 
    {
        if (k->len == keyn && !memcmp(k->key, key, keyn)) 
        {
            gc->dtor = &k->dtor;
            return 1;
        }
        k = k->next;
    }
    return 0;
}

// ........................................... gc print

void gcPrint_(struct gc_s* gc) 
{
    for (int i = 0; i < gc->length; i++) 
    {
        if (gc->table[i] != 0) 
        {
            struct gcKeyNode_s *k = gc->table[i];
            while (k) 
            {
                union // legal way to printf function pointer
                {
                    // typed def void(*gcHashDictValue_t)(void*);
                    gcHashDictValue_t funcptr ;
                    void *objptr;
                } u;
                u.funcptr = k->dtor;
  
                printf ( "# node(%p)::(%p)\n"
                    ,(void*)k
                    ,(void*)u.objptr
                ) ;
                k = k->next;
            }
        }
    }
}
#undef gcHashFunc

// ........................................... gc add key , dtor

int gcAdd(struct gc_s* gc,void* key,gcHashDictValue_t dtor)
{
    union {
        char  ptrc[8];
        void* ptr;
    } pkey ;
    pkey.ptr=(void*)key;
    
    int ret=gc_add(gc, pkey.ptrc, 8);
    *gc->dtor = dtor;
    return ret ;
}


// ........................................... gc find

static
int gcFind(struct gc_s* gc,void* key)
{
    union {
        char  ptrc[8];
        void* ptr;
    } pkey ;
    pkey.ptr=(void*)key;
    
    return gc_find(gc, pkey.ptrc, 8);
}

// ........................................... gc malloc

void* gcMalloc_( struct gc_s* gc , size_t size )
{
    void* ptr = malloc( size ) ;
    gcAdd(gc,ptr,free);
    return ptr ;
}

// ........................................... gc calloc

void* gcCalloc_( struct gc_s* gc , size_t SIZEOF , size_t size )
{
    void* ptr = calloc( SIZEOF , size ) ;
    gcAdd(gc,ptr,free);
    return ptr ;
}

// ........................................... gc free

void* gcFree_( struct gc_s* gc , void* ptr )
{
    if ( gcFind(gc,ptr) ) 
    {
        if (*gc->dtor!=NULL)
        {
            (*gc->dtor)(ptr);
            
            *gc->dtor=NULL ;

            ptr=NULL;
        }
    }
    return ptr;
}

// ........................................... gc realloc
/*
void* gcRealloc_( struct gc_s* gc , void* ptr, size_t size )
{
    if ( ptr==NULL )                 // FIX WITH NULL POINTER
    {
        return gcMalloc(size);
    }
    
    void* old = ptr ;

    ptr = realloc( ptr,size ) ;
    gcAdd(gc,ptr,free); 

    gcFind(gc,old);
    *gc->dtor=NULL ;    

    return ptr ;
}
*/

void* gcFree_NoDtor( struct gc_s* gc , void* ptr )
{
    if ( gcFind(gc,ptr) ) 
    {
        if (*gc->dtor!=NULL)
        {
            //(*gc->dtor)(ptr);
            
            *gc->dtor=NULL ;

            ptr=NULL;
        }
    }
    return ptr;
}


// ........................................... gc realloc

void* gcRealloc_( struct gc_s* gc , void* ptr, size_t size )
{
    if ( ptr==NULL )                 // FIX WITH NULL POINTER
    {
        return gcMalloc(size);
    }
    
    void* old = ptr ;

    ptr = realloc( ptr,size ) ;

    if (old!=ptr)
    {
        gcAdd(gc,ptr,free);
        // remove no free
        gcFree_NoDtor(gc,old);
    }
    else
    {
        gcFree_(gc,old);
    }

    return ptr ;
}

// ........................................... gc file Open

void* gcFileOpen_( struct gc_s* gc ,char* fileName, char* mode)
{
    #if defined(_MSC_VER)
    #pragma warning(disable:4996)
    #endif
    FILE* ptr = fopen ( fileName,mode );
    
    gcAdd(gc,ptr,cb_fclose);
    
    return ptr ;
}

// ........................................... gc file temp

FILE* gcFileTemp( void )
{
    #if defined(_MSC_VER)
    #pragma warning(disable:4996)
    #endif
    FILE* ptr = tmpfile();
    
    gcAdd(GC,ptr,cb_fclose);
    
    return ptr ;
}

// ........................................... call back fclose
//
// hack to avoid  warning: passing argument 3 of ‘gcAdd’ 
// from incompatible pointer type [-Wincompatible-pointer-types]

void cb_fclose(void*ptr)
{
    if (ptr!=NULL) fclose(ptr);
}

// ................................................................... wrapper strdup

char* gcStrDup( char *s )
{
    if ( s == NULL ) return (char*)NULL ;
    #undef strdup
    gcAdd( GC,strdup(s),free ) ;
    return s ;    
    #define strdup gcStrDup
}

// ................................................................... wrapper wcsdup

wchar_t* gcWcsDup( wchar_t *s)
{
    if ( s == NULL ) return (wchar_t*)NULL ;
    #undef wcsdup
    gcAdd( GC,wcsdup(s),free ) ;
    return s ;    
    #define wcsdup gcWcsDup
}

// ................................................................... intDup

int* gcIntDup(int val)
{
    int* p=(int*)  gcMalloc ( sizeof(int) );
    *p=val;
    return p ;
}

// ................................................................... doubleDup

double* gcDoubleDup ( double val )  
{
    double* p = (double*) gcMalloc ( sizeof(double) );
    *p=val;
    return p ;
}

// ......................................... [] compare function

int gcCompareInt(const void* a, const void* b)
{
  int va = *(const int*) a;
  int vb = *(const int*) b;
  return (va > vb) - (va < vb);
}

int gcCompareFloat (const void * a, const void * b)
{
  float fa = *(const float*) a;
  float fb = *(const float*) b;
  return (fa > fb) - (fa < fb);
}

int gcCompareDouble (const void * a, const void * b)
{
  double fa = *(const double*) a;
  double fb = *(const double*) b;
  return (fa > fb) - (fa < fb);
}

int gcCompareFloatAsInt (const void * a, const void * b)
{
  float _fa = *(const float*) a;
  float _fb = *(const float*) b;
  int fa = (int)_fa;
  int fb = (int)_fb;
  return (fa > fb) - (fa < fb);
}

int gcCompareDoubleAsInt (const void * a, const void * b)
{
  double _fa = *(const double*) a;
  double _fb = *(const double*) b;
  int fa = (int)_fa;
  int fb = (int)_fb;  
  return (fa > fb) - (fa < fb);
}

int gcCompareStrC ( const void * a, const void * b ) 
{
    const char **pa = (const char **)a;
    const char **pb = (const char **)b;
    return strcmp(*pa, *pb);   
    
}

int gcCompareWStrC ( const void * a, const void * b ) 
{
    const wchar_t **pa = (const wchar_t **)a;
    const wchar_t **pb = (const wchar_t **)b;
    return wcscmp(*pa, *pb);  
}

int gcComparepStrC ( const void * a, const void * b ) 
{
    const char *pa = (const char *)a;
    const char *pb = (const char *)b;
    return strcmp(pa, pb);   
    
}

int gcComparepWStrC ( const void * a, const void * b ) 
{
    const wchar_t *pa = (const wchar_t *)a;
    const wchar_t *pb = (const wchar_t *)b;
    return wcscmp(pa, pb);  
}



/**/


