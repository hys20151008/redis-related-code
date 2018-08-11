#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define dictSetVal(d, entry, _val_) entry->val = (_val_)
#define dictSetKey(d, entry, _key_) entry->key = (_key_)
#define dictCompareKeys(d, key1, key2) ((key1) == (key2))
#define dictIsRehashing(ht) ((ht)->rehashidx != -1)

typedef struct dictEntry {
    void *key;
    void *val;
    struct dictEntry *next;
} dictEntry;

typedef struct dictht {
    dictEntry **table;
    unsigned long size;
    unsigned long sizemask;
    unsigned long used;
} dictht;

typedef struct dict {
    dictht ht[2];
    int rehashidx;
} dict;

unsigned int dictGenHashFunction(const void *key, int len) {

    unsigned int seed = 5381;
    const unsigned int m = 0x5bd1e995;
    const int r = 24;


    unsigned int h = seed ^ len;


    const unsigned char *data = (const unsigned char *)key;

    while(len >= 4) {
        unsigned int  k = *(unsigned int*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }


    switch(len) {
        case 3: h ^= data[2] << 16;
        case 2: h ^= data[1] << 8;
        case 1: h ^= data[0]; h *= m;
    };


    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

static void _dictReset(dictht *ht) {
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

int _dictInit(dict *d) {
    _dictReset(&d->ht[0]);
    _dictReset(&d->ht[1]);
    d->rehashidx = -1;
    return 0;
}


dict *dictCreate() {
    dict *d = malloc(sizeof(*d));
    _dictInit(d);
    return d;
}

static int _dictKeyIndex(dict *d, const void *key) {
    unsigned int h, idx, table, slen;
    dictEntry *he;
    slen = strlen(key);
    h = dictGenHashFunction(key, slen);
	
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
		d->ht[table].table = malloc(sizeof(dictEntry));
        he = d->ht[table].table[idx];
        while (he) {
            if (dictCompareKeys(d, key, he->key))
                return -1;
            he = he->next;
        }
    }
    return idx;
}


static unsigned long _dictNextPower(unsigned long size) {
	unsigned long i = 4;
	if (size >= 512)
		return 512;
	while(1) {
		if (i >= size)
			return i;
		i *= 2;
	}
}

int dictExpand(dict *d, unsigned long size) {
	dictht n;
	unsigned long realsize = _dictNextPower(size);
	if (dictIsRehashing(d) || d->ht[0].used > size)
		return  1;
	n.size = realsize;
	n.sizemask = realsize-1;
	n.table = calloc(realsize, sizeof(dictEntry));
	n.used = 0;

	if (d->ht[0].table == NULL) {
		d->ht[0] = n;
		return 0;
	}
	d->ht[1] = n;
	d->rehashidx = 0;
	return 0;
}

static int _dictExpandIfNeeded(dict *d) {
	if (dictIsRehashing(d)) {
		return 0;
	}
	if (d->ht[0].size == 0)
		return dictExpand(d, 4);
	if (d->ht[0].used >= d->ht[0].size && (d->ht[0].used/d->ht[0].size > 2/3)) {
		return dictExpand(d, d->ht[0].used*2);
	}
	return 0;
}




dictEntry *dictAddRaw(dict *d, void *key) {
    int index;
    dictEntry *entry;
    dictht *ht;

    if ((index = _dictKeyIndex(d, key)) == -1)
        return NULL;

    entry = malloc(sizeof(*entry));

    ht = &d->ht[0];
//    ht->table = malloc(sizeof(*entry));
    entry->next = ht->table[index];
    ht->table[index] = entry;
    ht->used++;
    dictSetKey(d, entry, key);
    return entry;
}


int dictAdd(dict *d, void *key, void *val) {
    dictEntry *entry = dictAddRaw(d, key);
    if (!entry)
        return 1;
    dictSetVal(d, entry, val);
    return 0;
}

dictEntry *dictFind(dict *d, const void *key) {
	dictEntry *he;
	unsigned int h, idx, table, slen;
	slen = strlen(key);

	if (d->ht[0].size == 0) 
		return NULL;

	h = dictGenHashFunction(key, slen);
	for (table = 0; table <= 1;  table++) {
		idx = h & d->ht[table].sizemask;
		he = d->ht[table].table[idx];
		printf("idx=%u\n",idx);
		while(he) {
			printf("key=%s\t", (char*)key);
			printf("he->key=%s\t", (char*)he->key);
			if (dictCompareKeys(d, key, he->key))
				return he;
			he = he->next;
		}
		//if (!dictIsRehashing(d))
		//	return NULL;
	}
	return NULL;
}


int main() {
    char *key = "hello";
    char *val = "world";
    dict *d = malloc(sizeof(*d));
	//d->ht->table = malloc(sizeof(dictEntry));
    d = dictCreate();
    _dictExpandIfNeeded(d);
    dictAdd(d, key, val);
    printf("%s\n", (char*)d->ht[0].table[0]->key);
    printf("%s\n",(char*)d->ht[0].table[0]->val);
	printf("%lu\n", d->ht[0].size);
	printf("%lu\n", d->ht[0].sizemask);


	dictEntry *he;
	//he = malloc(sizeof(*he));
	he = dictFind(d,key);
	//printf("%s\n", (char*)he->key);
	if (he == NULL)
		printf("gg");

    return 0;
}
