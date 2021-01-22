
#include "hashMap.h"

#define hash_func meiyan

static inline uint32_t meiyan(const char *key, size_t count) 
{
	typedef uint32_t* P;
	uint32_t h = 0x811c9dc5;
	while (count >= 8) 
	{
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

struct hashMapNode_s *hashMapKeyNodeNew(char*k, hashLength_t l) 
{
	l=l+1 ; // fix : valgrind ->Invalid read of size 1

	struct hashMapNode_s *node = gcMalloc(sizeof(struct hashMapNode_s));
    
	node->len = l;
	node->key = gcMalloc(l);
	memcpy(node->key, k, l);
	node->next 	= 0;
	node->value = NULL;
	
	return node;
}

void hashMapKeyNodeDelete(struct hashMapNode_s *node) 
{
	gcFree(node->key);
	if (node->next) hashMapKeyNodeDelete(node->next);
	gcFree(node);
}

struct hashMap_s* hashMapNew(size_t initial_size) 
{
	struct hashMap_s* hm = gcMalloc(sizeof(struct hashMap_s));
	if (initial_size == 0) initial_size = 1024;
	hm->length = initial_size;
	hm->count = 0;
	hm->table = gcCalloc(sizeof(struct hashMapNode_s*), initial_size);
	hm->growth_treshold = 2.0;
	hm->growth_factor = 10;
	return hm;
}

void hashMapDelete(struct hashMap_s* hm) 
{
	for (size_t i = 0; i < hm->length; i++) 
	{
		if (hm->table[i])
		hashMapKeyNodeDelete(hm->table[i]);
	}
	gcFree(hm->table);
	hm->table = 0;
	gcFree(hm);
}

void hashMapReinsertWhenResizing(struct hashMap_s* hm, struct hashMapNode_s *k2) 
{
	size_t n = hash_func(k2->key, k2->len) % hm->length;
	if (hm->table[n] == 0) 
	{
		hm->table[n] = k2;
		hm->value = &hm->table[n]->value;
		return;
	}
	struct hashMapNode_s *k = hm->table[n];
	k2->next = k;
	hm->table[n] = k2;
	hm->value = &k2->value;
}

void hashMapResize(struct hashMap_s* hm, size_t newsize) 
{
	size_t o = hm->length;
	struct hashMapNode_s **old = hm->table;
	hm->table = gcCalloc(sizeof(struct hashMapNode_s*), newsize);
	hm->length = newsize;
	for (size_t i = 0; i < o; i++) 
	{
		struct hashMapNode_s *k = old[i];
		while (k) 
		{
			struct hashMapNode_s *next = k->next;
			k->next = 0;
			hashMapReinsertWhenResizing(hm, k);
			k = next;
		}
	}
	gcFree(old);
}

size_t hashMapAdd(struct hashMap_s* hm, void *key, size_t keyn) 
{
	size_t n = hash_func((const char*)key, keyn) % hm->length;
	if (hm->table[n] == 0) 
	{
		double f = (double)hm->count / (double)hm->length;
		if (f > hm->growth_treshold) 
		{
			hashMapResize(hm, hm->length * hm->growth_factor);
			return hashMapAdd(hm, key, keyn);
		}
		hm->table[n] = hashMapKeyNodeNew((char*)key, keyn);
		hm->value = &hm->table[n]->value;
		hm->count++;
		return 0;
	}
	struct hashMapNode_s *k = hm->table[n];
	while (k) 
	{
		if (k->len == keyn && memcmp(k->key, key, keyn) == 0) 
		{
			hm->value = &k->value;
			return 1;
		}
		k = k->next;
	}
	hm->count++;
	struct hashMapNode_s *k2 = hashMapKeyNodeNew((char*)key, keyn);
	k2->next = hm->table[n];
	hm->table[n] = k2;
	hm->value = &k2->value;
	return 0;
}

size_t hashMapFind(struct hashMap_s* hm, void *key, size_t keyn) 
{
	size_t n = hash_func((const char*)key, keyn) % hm->length;
	#if defined(__MINGW32__) || defined(__MINGW64__)
	__builtin_prefetch(hm->table[n]);
	#endif

	#if defined(_WIN32) || defined(_WIN64)
	_mm_prefetch((char*)hm->table[n], _MM_HINT_T0);
	#endif
	struct hashMapNode_s *k = hm->table[n];
	if (!k) return 0;
	while (k) 
	{
		if (k->len == keyn && !memcmp(k->key, key, keyn)) 
		{
			hm->value = &k->value;
			return 1;
		}
		k = k->next;
	}
	return 0;
}

void hashMapForEach(struct hashMap_s* hm, enumFunc f, void *user) 
{
	for (size_t i = 0; i < hm->length; i++) 
	{
		if (hm->table[i] != 0) 
		{
			struct hashMapNode_s *k = hm->table[i];
			while (k) 
			{
				if (	!	f(k->key, k->len, &k->value, user)	) return;
				k = k->next;
			}
		}
	}
}

size_t hashMapSet( hashMap_p hm,char * key , void * value )
{
	size_t res=hashMapAdd(hm, key, strlen(key) );
	*hm->value = value;
	return res;
}

void* hashMapGet( hashMap_p hm,char * key  )
{
	hashMapFind(hm, key, strlen(key) ) ;
	return hm->value  ;
}



#undef hash_func



/**/


