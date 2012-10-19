#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

/* Initializes the table
   Sets the state of all buckets to EMPTY */
void init_table(Table *t){
  int i;
  for (i = 0; i < NUM_BUCKETS; i++){
    t->buckets[i].state = EMPTY;
  }
}

/* Resets the table - does nothing if the table is NULL
   Otherwise makes key-ct zero, sets all buckets to EMPTY,
   and sets the keys and vals to null bytes */
void reset_table(Table *t){
  if (t == NULL) {
    return;
  } 
  else {
    int i, j;
    t->key_ct = 0;
    for(i = 0; i < NUM_BUCKETS; i++) {
      for(j = 0; j < MAX_STR_SIZE + 1; j++){
	t->buckets[i].data.value[j] = '\0';
	t->buckets[i].data.key[j] = '\0';
      }
      t->buckets[i].state = EMPTY;
    }
  }
  return;
}

/* Inserts the key and val into the table t at the hashed position.
   May do quadratic probing if original position is full.
   Returns 0 if successful, -1 otherwise */
int insert(Table *t, const char *key, const char *val){
  unsigned long hash_val;
  int buck, i;  
  if (key == NULL || val == NULL || strlen(key) > MAX_STR_SIZE ||
      strlen(val) > MAX_STR_SIZE || t == NULL || *key == '\0' || *val == '\0') {
    return -1;
  }
  hash_val = hash_code(key);
  buck = hash_val%NUM_BUCKETS;
  /* If any buckets have the same key as the insert, the value is replaced */
  for (i = 0; i < NUM_BUCKETS; i++){
    if (strcmp(t->buckets[i].data.key, key) == 0){
      strcpy(t->buckets[i].data.value, val);
      return 0;
    }
  }
  /* insert into hashed bucket if not full */
  if (t->buckets[buck].state != FULL){
    t->buckets[buck].state = FULL;
    strcpy(t->buckets[buck].data.key, key);
    strcpy(t->buckets[buck].data.value, val);
    t->key_ct++;
    return 0;
  }
  /* quadratic probing if original bucket is full */
  else if (t->buckets[buck].state == FULL){
    int quad;
    for (quad = 1; quad*quad < NUM_BUCKETS; quad++){
      if (t->buckets[(hash_val + quad*quad)%NUM_BUCKETS].state != FULL){
	strcpy(t->buckets[(hash_val + quad*quad)%NUM_BUCKETS].data.key, key);
	strcpy(t->buckets[(hash_val + quad*quad)%NUM_BUCKETS].data.value, val);
	t->buckets[(hash_val + quad*quad)%NUM_BUCKETS].state = FULL;
	t->key_ct++;
	return 0;
      }
    }
  }
  return -1;
}

/* Search for a key in the table t
   Returns 0 if successful, -1 otherwise */
int search(Table *t, const char*key, char *val){
  int i;
  if (t == NULL || key == NULL){
    return -1;
  }
  if (val == NULL){
    return 0;
  }
  /* if the keys are equal, the val parameter gets the value at that key */
  for(i = 0; i < NUM_BUCKETS; i++){
    if (strcmp(t->buckets[i].data.key, key) == 0){
      if (val != NULL){
	strcpy(val, t->buckets[i].data.value);
	return 0;
      }
    }
  }
  return -1;
}

/* Deletes the key/val pair from the table t
   Returns 0 if successful, -1 otherwise */
int delete(Table *t, const char *key){
  int i;
  int hash_val;
  if (t == NULL || key == NULL){
    return -1;
  }
  hash_val = hash_code(key)%NUM_BUCKETS;
  /* if the bucket is EMPTY, nothing to delete, so fails */
  if (t->buckets[hash_val].state == EMPTY){
    return -1;
  }
  /* if bucket is full, and the keys match, then deletes the key/val pair
     and decrements key_ct as well as set the bucket state to DIRTY */
  else if (t->buckets[hash_val].state == FULL){
    if (strcmp(t->buckets[hash_val].data.key, key) == 0){
      *(t->buckets[hash_val].data.key) = '\0';
      *(t->buckets[hash_val].data.value) = '\0';
      t->buckets[hash_val].state = DIRTY;
      t->key_ct--;
      return 0;
    }
  }
  /* the bucket is dirty, so search the table for any keys that match
     if match found, delete key/val pair, otherwise deletion fails */
  else {
    for (i = 0; i < NUM_BUCKETS; i++){
      if (strcmp(t->buckets[i].data.key, key) == 0){
	*(t->buckets[i].data.key) = '\0';
	*(t->buckets[i].data.value) = '\0';
	t->buckets[i].state = DIRTY;
	t->key_ct--;
	return 0;
      }
    }
  }
  return -1;
}

/* Returns the number of keys in the table, or -1 if table is NULL */
int key_count(Table *t){
  if (t != NULL) {
    return t->key_ct;
  }
  else {
    return -1;
  }
}

/* Returns the numberr of buckets in the table, or -1 if table is NULL */
int bucket_count(Table *t){
  if (t != NULL){
    return NUM_BUCKETS;
  }
  else {
    return -1;
  }
}

/* Calculates the hash code of a key, to find out what bucket to
   place things in */
unsigned long hash_code(const char *str){
  if (str == NULL){
    return 0;
  }
  else if (*str == '\0'){
    return 0;
  }
  else {
    char new_str[MAX_STR_SIZE];
    int new_char;
    size_t string_size;
    int last_char;
    unsigned long answer;
    /* makes a copy of str to avoid changing str */
    strcpy(new_str, str);
    /* string_size gets the length of new_str (str) */
    string_size = strlen(new_str);
    /* last_char is the position of the last character of new_str before
       the termination character */
    last_char = string_size - 1;
    /* new_char is the last character of new_str */
    new_char = new_str[last_char];
    /* make the last character of new_str null, thus shortening
       the string by one */
    new_str[last_char] = '\0';
    /* recursive call made here
       call hash_code with new shortened string
       multiply call by 65599
       then add on the last character of the old string (saved in new_char) */
    answer = hash_code(new_str) * 65599 + new_char;
    return answer;
  }
}