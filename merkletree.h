#ifndef MERKLE_TREE_H
#define MERKLE_TREE_H

#define HASH_SIZE 64

#include <stdio.h>

typedef struct Node Node;

struct Node {
  unsigned char hash[64];
  Node *left;
  Node *right;
};

typedef struct {
  Node *root;
} MerkleTree;

// -----------------------------------------------------------
// Merkle Tree
// -----------------------------------------------------------
MerkleTree *create_tree(unsigned char **transaction_hashes,
                        size_t num_transactions);
void free_tree(MerkleTree *tree);

// -----------------------------------------------------------
// Nodes
// -----------------------------------------------------------
Node *create_node(const unsigned char *hash);
void free_node(Node *node);

// -----------------------------------------------------------
// Hash
// -----------------------------------------------------------
int compute_hash(const unsigned char *data, size_t data_len,
                 unsigned char *hash, unsigned int *hash_length);
void combine_hashes(unsigned char *hash1, unsigned char *hash2,
                    unsigned char *combined_hash);

#endif // MERKLE_TREE_H
