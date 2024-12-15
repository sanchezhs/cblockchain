#include "blockchain.h"
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------
// Blockchain Implementation
// -----------------------------------------------------------

void print_compute_hash(unsigned char *digest_value,
                        unsigned int digest_length) {
  printf("Printing digest:\n");
  for (unsigned int j = 0; j < digest_length; j++) {
    fprintf(stdout, "%02x", digest_value[j]);
  }
  fprintf(stdout, "\n");
}

Block *create_block(Blockchain *blockchain, char **transaction_data,
                    size_t num_transactions) {
  Block *new_block = (Block *)malloc(sizeof(Block));
  if (new_block == NULL) {
    fprintf(stderr, "ERROR: Failed to allocate memory for new block\n");
    return NULL;
  }
  unsigned char hash[HASH_SIZE];
  unsigned int hash_size;
  Block *last = blockchain->tail;

  calculate_block_hash(last, hash, &hash_size);
  memcpy(new_block->prev_block_hash, hash, HASH_SIZE);

  new_block->timestamp = time(NULL);

  unsigned char **transaction_hashes =
      (unsigned char **)malloc(sizeof(unsigned char *) * num_transactions);
  if (transaction_hashes == NULL) {
    fprintf(stderr,
            "ERROR: Failed to allocate memory for transaction hashes\n");
    free(new_block);
    return NULL;
  }
  for (size_t i = 0; i < num_transactions; i++) {
    transaction_hashes[i] = (unsigned char *)malloc(HASH_SIZE);
    if (transaction_hashes[i] == NULL) {
      fprintf(stderr,
              "ERROR: Failed to allocate memory for transaction hash\n");
      for (size_t j = 0; j < i; j++) {
        free(transaction_hashes[j]);
      }
      free(transaction_hashes);
      free(new_block);
      return NULL;
    }
    compute_hash((unsigned char *)transaction_data[i],
                 strlen(transaction_data[i]), transaction_hashes[i],
                 &hash_size);
  }

  new_block->merkletree = create_tree(transaction_hashes, num_transactions);

  for (size_t i = 0; i < num_transactions; i++) {
    free(transaction_hashes[i]);
  }
  free(transaction_hashes);
  new_block->next_block = NULL;

  if (blockchain->tail != NULL) {
    blockchain->tail->next_block = new_block;
  } else {
    blockchain->head = new_block;
  }
  blockchain->tail = new_block;
  blockchain->count++;

  return new_block;
}

void destroy_blockchain(Blockchain *blockchain) {
  Block *curr = blockchain->head;
  Block *next = NULL;

  while (curr != NULL) {
    next = curr->next_block;
    free_tree(curr->merkletree);
    free(curr);
    curr = next;
  }

  blockchain->head = NULL;
  blockchain->tail = NULL;
  blockchain->count = 0;
}

Block *get_last_block(Blockchain *blockchain) { return blockchain->tail; }

char *block_to_string(Block *block) {
  size_t block_size = HASH_SIZE * 2 + sizeof(block->timestamp) + 100;
  char *str_block = (char *)malloc(block_size);
  if (str_block == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed for block string\n");
    return NULL;
  }

  str_block[0] = '\0';

  snprintf(str_block, block_size, "Previous Block Hash: ");
  for (size_t i = 0; i < HASH_SIZE; i++) {
    snprintf(str_block + strlen(str_block), block_size - strlen(str_block),
             "%02x", block->prev_block_hash[i]);
  }

  snprintf(str_block + strlen(str_block), block_size - strlen(str_block),
           "\nTimestamp: %ld\n", block->timestamp);

  if (block->merkletree != NULL && block->merkletree->root != NULL) {
    unsigned char root_hash[HASH_SIZE];
    unsigned int root_hash_size;

    compute_hash(block->merkletree->root->hash, HASH_SIZE, root_hash,
                 &root_hash_size);
    snprintf(str_block + strlen(str_block), block_size - strlen(str_block),
             "Merkle Tree Root Hash: ");
    for (size_t i = 0; i < root_hash_size; i++) {
      snprintf(str_block + strlen(str_block), block_size - strlen(str_block),
               "%02x", root_hash[i]);
    }
  } else {
    snprintf(str_block + strlen(str_block), block_size - strlen(str_block),
             "Merkle Tree: NULL\n");
  }

  return str_block;
}

void calculate_block_hash(Block *block, unsigned char *digest_value,
                          unsigned int *digest_length) {
  unsigned char merkle_root[HASH_SIZE];
  compute_hash(block->merkletree->root->hash, HASH_SIZE, merkle_root,
               digest_length);

  size_t block_size = HASH_SIZE + HASH_SIZE;

  unsigned char *block_data = (unsigned char *)malloc(block_size);
  if (block_data == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed for block data\n");
    return;
  }

  memcpy(block_data, block->prev_block_hash, HASH_SIZE);
  memcpy(block_data + HASH_SIZE, merkle_root, HASH_SIZE);

  if (!compute_hash(block_data, block_size, digest_value, digest_length)) {
    fprintf(stderr, "ERROR: Failed to calculate block hash\n");
  }

  free(block_data);
}

bool validate_block(Block *block, Block *prev_block) {
  if (block == NULL || prev_block == NULL) {
    printf("WARNING: validate_block found a NULL block, returning false\n");
    return false;
  }

  unsigned char prev_block_digest[HASH_SIZE];
  unsigned int prev_block_digest_length;

  calculate_block_hash(prev_block, prev_block_digest,
                       &prev_block_digest_length);

  return memcmp(block->prev_block_hash, prev_block_digest, HASH_SIZE) == 0;
}

bool validate_blockchain(Blockchain *blockchain) {
  if (blockchain == NULL || blockchain->head == NULL)
    return false;

  bool valid = true;
  Block *curr = blockchain->head;

  while (curr != NULL && curr->next_block != NULL) {
    if (!validate_block(curr->next_block, curr)) {
      valid = false;
      break;
    }
    curr = curr->next_block;
  }
  return valid;
}

void create_blockchain(Blockchain *blockchain) {
  Block *genesis = (Block *)malloc(sizeof(Block));
  if (genesis == NULL) {
    fprintf(stderr, "ERROR: Failed to allocate memory for genesis block\n");
    return;
  }

  const char genesis_data[] = "Genesis";
  unsigned char hash[HASH_SIZE];
  unsigned int hash_size;

  compute_hash((unsigned char *)genesis_data, strlen(genesis_data), hash,
               &hash_size);

  unsigned char *transaction_hashes[] = {hash};
  genesis->merkletree = create_tree(transaction_hashes, 1);
  genesis->timestamp = time(0);
  memset(genesis->prev_block_hash, 0, HASH_SIZE);
  genesis->next_block = NULL;

  blockchain->head = genesis;
  blockchain->tail = genesis;
  blockchain->count = 1;
}

void print_blockchain(Blockchain *blockchain) {
  if (blockchain == NULL || blockchain->head == NULL) {
    printf("Blockchain is empty or NULL.\n");
    return;
  }

  Block *current_block = blockchain->head;
  printf("\n===================================================\n");
  printf("Printing Blockchain with %d blocks:\n", blockchain->count);

  size_t i = 0;
  while (current_block != NULL) {
    printf("Block %zu\n", i);
    printf("Previous Block Hash: ");

    for (size_t j = 0; j < HASH_SIZE; j++) {
      printf("%02x", current_block->prev_block_hash[j]);
    }
    printf("\n");

    printf("Timestamp: %ld\n", current_block->timestamp);

    unsigned char root_hash[HASH_SIZE];
    unsigned int root_hash_size;
    if (current_block->merkletree != NULL) {
      compute_hash(current_block->merkletree->root->hash, HASH_SIZE, root_hash,
                   &root_hash_size);
      printf("Merkle Tree Root Hash: ");
      for (size_t k = 0; k < root_hash_size; k++) {
        printf("%02x", root_hash[k]);
      }
      printf("\n");
    } else {
      printf("Merkle Tree: NULL\n");
    }

    current_block = current_block->next_block;
    printf("\n");
    i++;
  }
  printf("===================================================\n");
}

bool add_transaction(Block *block, const char *transaction) {
  if (block == NULL || transaction == NULL) {
    fprintf(stderr, "Transaction data is invalid\n");
    return false;
  }

  return true;
}
