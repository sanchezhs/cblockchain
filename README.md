# cblockchain

A simple blockchain implementation written in C. This project demonstrates the core concepts of a blockchain, including block creation, hash calculation, and the use of a Merkle tree for transactions.

## Features

- **Blockchain**: A basic blockchain structure with blocks that link to each other.
- **Merkle Tree**: A tree structure to efficiently manage and verify transaction data in each block.
- **Hashing**: Secure hash generation using SHA3-512 for block and transaction integrity.

## Requirements

- GCC or any other C compiler
- OpenSSL (for cryptographic functions)

## Installation

1. Clone this repository to your local machine:
   ```bash
   git clone https://github.com/your-username/cblockchain.git

2. Change into the project directory:
```bash
cd cblockchain
```
3. Build the project:
```bash
make
```

## Usage
Run the executable:
```bash
./main
```

## Example
Here's a blockchain with three blocks and some example data:
```bash
Blockchain validated successfully

===================================================
Printing Blockchain with 3 blocks:
Block 0
Previous Block Hash: 00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
Timestamp: 1734267085
Merkle Tree Root Hash: 61d8ce2eee53bbb2b63b1908a61ecb7c33547465a2e133712e034e0038b118e1e7b68043b7aaca52f598410ff51c702e95becf0d8c998441f29ab231d52192eb

Block 1
Previous Block Hash: 15d7a006e75a076797e8e2a224785e135ecca9601ffa8a6282fde9cea2e649c87e4d840e6c2a48a464084b9544340629b14f391255b3c34d0d4eab34fb3e3f22
Timestamp: 1734267085
Merkle Tree Root Hash: a1bd100a8bbd77c117447646cb5b40dc75d4b30aacfaf0a7ef3153be92af289e2a71b4c810f73dc06ca352e485f020c88b5d6ff382d85bdab417027e7c628667

Block 2
Previous Block Hash: ab46718ba36a7709b1234615a998752f50a1bec5f1ee5b5e3c68cce2ea4b259badf1a772f49906e824c7ec9ea2bd8316c93e36b427923f7df9a795736b251824
Timestamp: 1734267085
Merkle Tree Root Hash: 686cf6982a843282f1de22bc44009a23d7266a571f588676ef8dfd00a9935370e3c34efb3f0dad24a9c0ffc46364fe7aad306f672052fa4dd2fa81abdce2d28c
```


