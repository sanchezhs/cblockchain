#include "blockchain.h"
#include <stdio.h>

#define UNUSED(x) (void)(x)

int main(void) {
  Blockchain blockchain = {0};
  create_blockchain(&blockchain);

  char *transaction_data_one[] = {"123.2", "2133.0"};
  Block *two = create_block(&blockchain, transaction_data_one, 2);
  UNUSED(two);

  char *transaction_data_two[] = {"4342", "more_data"};
  Block *three = create_block(&blockchain, transaction_data_two, 2);
  UNUSED(three);

  if (!validate_blockchain(&blockchain)) {
    printf("Blockchain contains errors\n");
  } else {
    printf("Blockchain validated successfully\n");
  }

  print_blockchain(&blockchain);
  destroy_blockchain(&blockchain);
  return 0;
}
