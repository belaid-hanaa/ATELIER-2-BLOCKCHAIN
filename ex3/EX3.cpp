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


#include <functional>
using HashFunc = std::function<std::string(const std::string&)>;

struct Transaction {
    int id;
    string sender;
    string receiver;
    double amount;

    string to_string() const {
        stringstream ss;
        ss << id << "|" << sender << "|" << receiver << "|" << amount;
        return ss.str();
    }
};


struct Node {
    string hash;
    Node* left = nullptr;
    Node* right = nullptr;
    Node(const string& h) : hash(h) {}
};

class MerkleTree {
public:
    MerkleTree(const vector<Transaction>& transactions, HashFunc hf) : hash_func(hf){
        vector<string> tx_hashes;
        for (auto &tx : transactions)
            tx_hashes.push_back(hash_func(tx.to_string()));
        build_tree(tx_hashes);
    }

    ~MerkleTree() { free_nodes(root); }
    string get_root_hash() const { return root ? root->hash : ""; }

private:
    HashFunc hash_func;
    Node* root = nullptr;

    void free_nodes(Node* n) {
        if (!n) return;
        free_nodes(n->left);
        free_nodes(n->right);
        delete n;
    }

    void build_tree(vector<string>& leaves_hashes) {
        if (leaves_hashes.empty()) return;

        vector<Node*> leaves;
        for (auto &h : leaves_hashes) leaves.push_back(new Node(h));

        size_t n = leaves.size();
        size_t p = 1;
        while (p < n) p <<= 1;
        while (leaves.size() < p) leaves.push_back(new Node(leaves.back()->hash));

        vector<Node*> current = leaves;
        while (current.size() > 1) {
            vector<Node*> next;
            for (size_t i = 0; i < current.size(); i += 2) {
                Node* L = current[i];
                Node* R = current[i + 1];
                Node* parent = new Node(hash_func(L->hash + R->hash));
                parent->left = L;
                parent->right = R;
                next.push_back(parent);
            }
            current.swap(next);
        }
        root = current.front();
    }
};

// --------------------- Block ---------------------
struct Block {
    int index;
    long long timestamp;
    string prev_hash;
    string merkle_root;
    uint64_t nonce = 0;
    string hash;
    HashFunc hash_func;

    Block(int idx, const string& prev, const vector<Transaction>& txs, HashFunc hf)
        : index(idx), prev_hash(prev), hash_func(hf) 
    {
        timestamp = duration_cast<milliseconds>(
                        system_clock::now().time_since_epoch()).count();
        MerkleTree mt(txs, hash_func);
        merkle_root = mt.get_root_hash();
        hash = compute_hash();
    }

    string compute_hash() const {
        stringstream ss;
        ss << index << "|" << timestamp << "|" << prev_hash << "|" << merkle_root << "|" << nonce;
        return hash_func(ss.str());
    }

    void mine_block(int difficulty) {
        string target(difficulty, '0');
        auto start = steady_clock::now();
        while (hash.substr(0, difficulty) != target) {
            ++nonce;
            hash = compute_hash();
            if ((nonce & 0xFFFF) == 0) this_thread::yield();
        }
        auto end = steady_clock::now();
        auto ms = duration_cast<milliseconds>(end - start).count();
        cout << "Block mined! Index: " << index << ", Nonce: " << nonce 
             << ", Time: " << ms << " ms, Hash: " << hash.substr(0, 10) << "...\n";
    }
};

struct Validator {
    string name;
    double stake;
};

string select_validator(const vector<Validator>& validators) {
    double total_stake = 0.0;
    for (auto &v : validators) total_stake += v.stake;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, total_stake);

    double r = dis(gen), cumulative = 0.0;
    for (auto &v : validators) {
        cumulative += v.stake;
        if (r <= cumulative) return v.name;
    }
    return validators.back().name;
}

// --------------------- Blockchain ---------------------
class Blockchain {
public:
    vector<Block> chain;

    Blockchain() {
        vector<Transaction> genesis_tx = { {0, "genesis", "genesis", 0.0} };
        chain.emplace_back(0, "0", genesis_tx, [](const string& s){ return sha256(s); });

    }

    const Block& latest() const { return chain.back(); }

    void add_block(Block new_block) {
        new_block.prev_hash = latest().hash;
        chain.push_back(new_block);
    }

    bool validate_chain() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            if (chain[i].prev_hash != chain[i - 1].hash) return false;
            if (chain[i].compute_hash() != chain[i].hash) return false;
        }
        return true;
    }
};

// --------------------- Main ---------------------
int main() {
    Blockchain blockchain;

    vector<Validator> validators = {
        {"Alice", 50.0},
        {"Bob", 30.0},
        {"Charlie", 20.0}
    };

    vector<Transaction> txs1 = {
        {1, "Alice", "Bob", 100.0},
        {2, "Bob", "Charlie", 50.0},
        {3, "Charlie", "Alice", 25.0}
    };

    vector<Transaction> txs2 = {
        {4, "Alice", "Charlie", 75.0},
        {5, "Bob", "Alice", 20.0}
    };

    cout << "Choose hash function:\n1. SHA256\n2. AC_HASH\nYour choice: ";
    int choice;
    cin >> choice;

    HashFunc hash_func;
    if (choice == 1)
        hash_func = [](const string& s){ return sha256(s); };
    else
        hash_func = [](const string& s){ return ac_hash(s, 30, 100); };

    cout << "\n========== Mining with PoW ==========\n";
    Block pow_block1(1, blockchain.latest().hash, txs1, hash_func);
    pow_block1.mine_block(2);
    blockchain.add_block(pow_block1);

    Block pow_block2(2, blockchain.latest().hash, txs2, hash_func);
    pow_block2.mine_block(2);
    blockchain.add_block(pow_block2);

    cout << "\nChain valid: " << (blockchain.validate_chain() ? "YES" : "NO") << "\n";

    cout << "\n========== Adding PoS Block ==========\n";
    vector<Transaction> txs_pos = {
        {6, "Charlie", "Bob", 10.0}
    };
    Block pos_block(3, blockchain.latest().hash, txs_pos, hash_func);
    string chosen_validator = select_validator(validators);
    cout << "Validator chosen: " << chosen_validator << "\n";
    pos_block.hash = pos_block.compute_hash();
    blockchain.add_block(pos_block);

    cout << "\nChain valid: " << (blockchain.validate_chain() ? "YES" : "NO") << "\n";

    return 0;
}
