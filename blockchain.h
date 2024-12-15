#ifndef BLOCK_CHAIN_H
#define BLOCK_CHAIN_H

#include "merkletree.h"
#include <stdbool.h>
#include <time.h>

typedef struct Block Block;

struct Block {
  Block *next_block;
  unsigned char prev_block_hash[HASH_SIZE];
  time_t timestamp;
  MerkleTree *merkletree;
};

typedef struct {
  Block *head;
  Block *tail;
  int count;
} Blockchain;

// -----------------------------------------------------------
// Blockchain management
// -----------------------------------------------------------
void create_blockchain(Blockchain *blockchain);
Block *create_block(Blockchain *blockchain, char **transaction_data,
                    size_t num_transactions);
Block *get_last_block(Blockchain *blockchain);
char *block_to_string(Block *block);
void destroy_blockchain(Blockchain *blockchain);
void calculate_block_hash(Block *block, unsigned char *digest_value,
                          unsigned int *digest_length);

// -----------------------------------------------------------
// Blockchain validation
// -----------------------------------------------------------
bool validate_block(Block *block, Block *prev_block);
bool validate_blockchain(Blockchain *blockchain);

// -----------------------------------------------------------
// Blockchain printing
// -----------------------------------------------------------
void print_block(Block *block);
void print_blockchain(Blockchain *blockchain);

// -----------------------------------------------------------
// Blockchain interaction
// -----------------------------------------------------------
bool add_transaction(Block *block, const char *transaction);

#endif // BLOCK_CHAIN_H
