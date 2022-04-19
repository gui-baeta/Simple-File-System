# ifndef FS_H
# define FS_H
# include "lib/bst.h"
# include "lib/inodes.h"

typedef struct tecnicofs {
    node* *bstRoot;
    int nextINumber;
} tecnicofs;

int obtainNewInumber(tecnicofs* fs);
tecnicofs* newTecnicofs();
void freeTecnicofs(tecnicofs* fs);
void create(tecnicofs* fs, size_t hash_key, char *name, int inumber);
void delete(tecnicofs* fs, size_t hash_key, char *name);
int lookup(tecnicofs* fs, size_t hash_key, char *name);
void printTecnicofsTree(FILE * fp, tecnicofs *fs);

# endif /* FS_H */
