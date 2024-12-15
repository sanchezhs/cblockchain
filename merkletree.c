#include "merkletree.h"
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------
// MerkleTree Implementation
// -----------------------------------------------------------

Node *create_node(const unsigned char *hash) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  if (new_node == NULL) {
    fprintf(stderr, "ERROR: Failed to allocate memory for new node\n");
    return NULL;
  }

  memcpy(new_node->hash, hash, HASH_SIZE);

  new_node->left = NULL;
  new_node->right = NULL;

  return new_node;
}

void combine_hashes(unsigned char *hash1, unsigned char *hash2,
                    unsigned char *combined_hash) {
  unsigned char concatenated[2 * HASH_SIZE];
  unsigned int hash_size;

  memcpy(concatenated, hash1, HASH_SIZE);
  memcpy(concatenated + HASH_SIZE, hash2, HASH_SIZE);

  compute_hash(concatenated, sizeof(concatenated), combined_hash, &hash_size);
}

MerkleTree *create_tree(unsigned char **transaction_hashes,
                        size_t num_transactions) {
  MerkleTree *tree = (MerkleTree *)malloc(sizeof(MerkleTree));
  if (tree == NULL) {
    fprintf(stderr, "ERROR: Failed to allocate memory for MerkleTree\n");
    return NULL;
  }

  Node **nodes = (Node **)malloc(sizeof(Node) * num_transactions);
  for (size_t i = 0; i < num_transactions; i++) {
    nodes[i] = create_node(transaction_hashes[i]);
  }

  while (num_transactions > 1) {
    size_t new_size = (num_transactions + 1) / 2;

    Node **new_nodes = (Node **)malloc(sizeof(Node *) * new_size);
    size_t j = 0;

    for (size_t i = 0; i < new_size; i++) {
      if (i + 1 < num_transactions) {
        unsigned char combined_hash[HASH_SIZE];
        combine_hashes(nodes[i]->hash, nodes[i + 1]->hash, combined_hash);
        new_nodes[j++] = create_node(combined_hash);
      } else {
        new_nodes[j++] = nodes[i];
      }
    }
    num_transactions = new_size;
  }

  tree->root = nodes[0];
  free(nodes);

  return tree;
}

void free_node(Node *node) {
  if (node != NULL) {
    free_node(node->left);
    free_node(node->right);
    free(node);
  }
}

void free_tree(MerkleTree *tree) {
  if (tree != NULL) {
    free_node(tree->root);
    free(tree);
  }
}

int compute_hash(const unsigned char *data, size_t data_len,
                 unsigned char *digest_value, unsigned int *digest_length) {
  OSSL_LIB_CTX *library_context;
  int ret = 0;
  const char *option_properties = NULL;
  EVP_MD *message_digest = NULL;
  EVP_MD_CTX *digest_context = NULL;

  library_context = OSSL_LIB_CTX_new();
  if (library_context == NULL) {
    fprintf(stderr, "OSSL_LIB_CTX_new() returned NULL\n");
    goto cleanup;
  }

  /* Fetch a message digest by name */
  message_digest = EVP_MD_fetch(library_context, "SHA3-512", option_properties);
  if (message_digest == NULL) {
    fprintf(stderr, "EVP_MD_fetch could not find SHA3-512.\n");
    goto cleanup;
  }

  /* Determine the length of the fetched digest type */
  *digest_length = EVP_MD_get_size(message_digest);
  if (*digest_length <= 0) {
    fprintf(stderr, "EVP_MD_get_size returned invalid size.\n");
    goto cleanup;
  }

  /* Ensure the provided buffer is large enough */
  if (*digest_length > HASH_SIZE) {
    fprintf(stderr, "Provided digest buffer is too small.\n");
    goto cleanup;
  }

  /* Create a digest context */
  digest_context = EVP_MD_CTX_new();
  if (digest_context == NULL) {
    fprintf(stderr, "EVP_MD_CTX_new failed.\n");
    goto cleanup;
  }

  /* Initialize the digest context */
  if (EVP_DigestInit(digest_context, message_digest) != 1) {
    fprintf(stderr, "EVP_DigestInit failed.\n");
    goto cleanup;
  }

  /* Digest the input data */
  if (EVP_DigestUpdate(digest_context, data, data_len) != 1) {
    fprintf(stderr, "EVP_DigestUpdate failed.\n");
    goto cleanup;
  }

  if (EVP_DigestFinal(digest_context, digest_value, digest_length) != 1) {
    fprintf(stderr, "EVP_DigestFinal failed.\n");
    goto cleanup;
  }

  ret = 1;

cleanup:
  if (ret != 1)
    ERR_print_errors_fp(stderr);
  EVP_MD_CTX_free(digest_context);
  EVP_MD_free(message_digest);
  OSSL_LIB_CTX_free(library_context);
  return ret;
}
