#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <random>
#include <thread>
#include "sha256.h"
#include "../ex2/ac_hash.h"


using namespace std;
using namespace chrono;

// --------------------- Transaction ---------------------
struct Transaction {
    int id;
    string sender;
    string receiver;
    double amount;

    string to_string() const;
};

// --------------------- Node ---------------------
struct Node {
    string hash;
    Node* left = nullptr;
    Node* right = nullptr;
    Node(const string& h);
};

// --------------------- Merkle Tree ---------------------
class MerkleTree {
public:
    MerkleTree(const vector<Transaction>& transactions, string hash_mode = "AC");
    ~MerkleTree();
    string get_root_hash() const;

private:
    Node* root = nullptr;
    string hash_mode;

    void free_nodes(Node* n);
    void build_tree(vector<string>& leaves_hashes);
};

// --------------------- Block ---------------------
struct Block {
    int index;
    long long timestamp;
    string prev_hash;
    string merkle_root;
    uint64_t nonce = 0;
    string hash;
    string hash_mode;

    Block(int idx, const string& prev, const vector<Transaction>& txs, string hash_mode = "AC");
    string compute_hash() const;
    void mine_block(int difficulty);
};

// --------------------- Validator (PoS) ---------------------
struct Validator {
    string name;
    double stake;
};

string select_validator(const vector<Validator>& validators);

// --------------------- Blockchain ---------------------
class Blockchain {
private:
    string hash_mode;
    int pow_difficulty;

public:
    vector<Block> chain;

    Blockchain(string hash_mode = "AC", int pow_difficulty = 3);

    const Block& latest() const;
    void add_block_pow(vector<Transaction> txs);
    void add_block_pos(vector<Transaction> txs, vector<Validator> validators);
    bool validate_chain() const;
};

#endif // BLOCKCHAIN_H