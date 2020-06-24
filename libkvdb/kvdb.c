#include <stdio.h>
#include <assert.h>

struct kvdb {
  char* filename;
  char* key;
  char value;
  // your definition here
};

struct kvdb *kvdb_open(const char *filename) {
  assert(db != NULL);
  return NULL;
}

int kvdb_close(struct kvdb *db) {
  assert(db != NULL);
  return -1;
}

int kvdb_put(struct kvdb *db, const char *key, const char *value) {
  assert(db != NULL);
  assert(key != NULL);
  assert(value != NULL);
  return -1;
}

char *kvdb_get(struct kvdb *db, const char *key) {
  assert(db != NULL);
  assert(key != NULL);
  return NULL;
}
