#ifndef HASHMAP
#define HASHMAP

#include "cxx.h"


typedef size_t (*enumFunc)(void *key, size_t count,void** value, void *user);

typedef void*  			hashValue_t 	;
typedef hashValue_t 	hashMap_d 		;
typedef size_t 			hashLength_t	;

struct hashMapNode_s 
{
	struct hashMapNode_s *	next;
	char *					key;
	hashLength_t 			len;
	hashValue_t 			value;
};

struct hashMap_s 
{
	struct hashMapNode_s **	table;
	size_t 					length; 
	size_t					count;
	double 					growth_treshold;
	double 					growth_factor;
	hashValue_t *			value;
};

struct hashMap_s* 	hashMapNew		(size_t initial_size);
void 				hashMapDelete	(struct hashMap_s* hm);
size_t 				hashMapAdd		(struct hashMap_s* hm, void *key, size_t keyn);
size_t 				hashMapFind		(struct hashMap_s* hm, void *key, size_t keyn);
void 				hashMapForEach	(struct hashMap_s* hm, enumFunc f, void *user);

typedef struct hashMapNode_s ** hashMapIt_t ;

#define hashMapCapacity(HM)		(HM->length)
#define hashMapSize(HM)			(HM->count)	
#define hashMapBegin(HM)		(&HM->table[0])
#define hashMapEnd(HM)			(&HM->table[HM->length])

typedef struct hashMap_s 	hashMap_t  ;
typedef struct hashMap_s* 	hashMap_p  ;


size_t 	hashMapSet	( hashMap_p hm,char * key , void * value ) ;
void* 	hashMapGet	( hashMap_p hm,char * key  ) ;

// hashMap_d  ; 
// hashMap_s  ; 
// hashMap_t  ;
// hashMap_p  ;

#endif
 
 
 
/**/


