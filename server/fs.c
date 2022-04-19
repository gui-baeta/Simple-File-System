# include "fs.h"
# include "data.h"
# include "lib/bst.h"
# include "lib/hash.h"
# include <stdlib.h>
# include <stdio.h>
# include <string.h>

int obtainNewInumber(tecnicofs* fs) {
	int newInumber = ++(fs->nextINumber);
	return newInumber;
}

tecnicofs* newTecnicofs(char* argv[4]) {
	tecnicofs* fs = malloc(sizeof(tecnicofs));

	fs->bstRoot = malloc(sizeof(node*) * numberBuckets);
	if (!fs) {
		perror("failed to allocate tecnicofs");
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < numberBuckets; i++) { // initializes tree for each bucket
		fs->bstRoot[i] = NULL;
	}

	fs->nextINumber = 0;
	return fs;
}

void freeTecnicofs(tecnicofs* fs) {
	for (size_t i = 0; i < numberBuckets; i++) { // frees tree in each bucket
		free_tree(fs->bstRoot[i]);
	}
	free(fs->bstRoot);
	free(fs);
}

void create(tecnicofs* fs, size_t hash_key, char *name, int inumber) {
	fs->bstRoot[hash_key] = insert(fs->bstRoot[hash_key], name, inumber);
}

void delete(tecnicofs* fs, size_t hash_key, char *name) {
	fs->bstRoot[hash_key] = remove_item(fs->bstRoot[hash_key], name);
}

int lookup(tecnicofs* fs, size_t hash_key, char *name) {
	node* searchNode = search(fs->bstRoot[hash_key], name);
	if ( searchNode ) return searchNode->inumber;
	return -1;
}

void printTecnicofsTree(FILE * fp, tecnicofs *fs) {
	for (size_t i = 0; i < numberBuckets; i++) {
		print_tree(fp, fs->bstRoot[i]);
	}
}
