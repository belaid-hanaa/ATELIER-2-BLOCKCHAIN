#include "blockchain.h"

// --------------------- Transaction ---------------------
string Transaction::to_string() const {
    stringstream ss;
    ss << id << "|" << sender << "|" << receiver << "|" << amount;
    return ss.str();
}

// --------------------- Node ---------------------
Node::Node(const string& h) : hash(h) {}

// --------------------- Merkle Tree ---------------------
MerkleTree::MerkleTree(const vector<Transaction>& transactions, string hash_mode)
    : hash_mode(hash_mode) {
    vector<string> tx_hashes;
    for (auto &tx : transactions) {
        string hash = hash_mode == "AC"
            ? ac_hash(tx.to_string(), 30, 256)
            : sha256(tx.to_string());
        tx_hashes.push_back(hash);
    }
    build_tree(tx_hashes);
}

MerkleTree::~MerkleTree() {
    free_nodes(root);
}

string MerkleTree::get_root_hash() const {
    return root ? root->hash : "";
}

void MerkleTree::free_nodes(Node* n) {
    if (!n) return;
    free_nodes(n->left);
    free_nodes(n->right);
    delete n;
}

void MerkleTree::build_tree(vector<string>& leaves_hashes) {
    if (leaves_hashes.empty()) return;

    vector<Node*> leaves;
    for (auto &h : leaves_hashes)
        leaves.push_back(new Node(h));

    size_t n = leaves.size();
    size_t p = 1;
    while (p < n) p <<= 1;
    while (leaves.size() < p)
        leaves.push_back(new Node(leaves.back()->hash));

    vector<Node*> current = leaves;
    while (current.size() > 1) {
        vector<Node*> next;
        for (size_t i = 0; i < current.size(); i += 2) {
            Node* L = current[i];
            Node* R = current[i + 1];
            Node* parent = hash_mode == "AC"
                ? new Node(ac_hash(L->hash + R->hash, 30, 256))
                : new Node(sha256(L->hash + R->hash));
            parent->left = L;
            parent->right = R;
            next.push_back(parent);
        }
        current.swap(next);
    }
    root = current.front();
}

// --------------------- Block ---------------------
Block::Block(int idx, const string& prev, const vector<Transaction>& txs, string hash_mode)
    : index(idx), prev_hash(prev), hash_mode(hash_mode) {
    timestamp = duration_cast<milliseconds>(
                    system_clock::now().time_since_epoch()).count();
    MerkleTree mt(txs, hash_mode);
    merkle_root = mt.get_root_hash();
    hash = compute_hash();
}

string Block::compute_hash() const {
    stringstream ss;
    ss << index << "|" << timestamp << "|" << prev_hash << "|" << merkle_root << "|" << nonce;
    if (hash_mode == "AC")
        return ac_hash(ss.str(), 30, 256);
    else
        return sha256(ss.str());
}

void Block::mine_block(int difficulty) {
    string target(difficulty, '0');
    auto start = steady_clock::now();
    while (hash.substr(0, difficulty) != target) {
        ++nonce;
        hash = compute_hash();
        if ((nonce & 0xFFFF) == 0)
            this_thread::yield();
    }
    auto end = steady_clock::now();
    auto ms = duration_cast<milliseconds>(end - start).count();
    cout << "Block mined! Index: " << index << ", Nonce: " << nonce
         << ", Time: " << ms << " ms, Hash: " << hash.substr(0, 10) << "...\n";
}

// --------------------- Validator (PoS) ---------------------
string select_validator(const vector<Validator>& validators) {
    double total_stake = 0.0;
    for (auto &v : validators)
        total_stake += v.stake;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, total_stake);

    double r = dis(gen), cumulative = 0.0;
    for (auto &v : validators) {
        cumulative += v.stake;
        if (r <= cumulative)
            return v.name;
    }
    return validators.back().name;
}

// --------------------- Blockchain ---------------------
Blockchain::Blockchain(string hash_mode, int pow_difficulty)
    : hash_mode(hash_mode), pow_difficulty(pow_difficulty) {
    vector<Transaction> genesis_tx = { {0, "genesis", "genesis", 0.0} };
    chain.emplace_back(0, "0", genesis_tx, hash_mode);
}

const Block& Blockchain::latest() const {
    return chain.back();
}

void Blockchain::add_block_pow(vector<Transaction> txs) {
    Block pow_block(chain.size(), latest().hash, txs, hash_mode);
    pow_block.mine_block(pow_difficulty);
    chain.push_back(pow_block);
}

void Blockchain::add_block_pos(vector<Transaction> txs, vector<Validator> validators) {
    Block pos_block(chain.size(), latest().hash, txs, hash_mode);
    string chosen_validator = select_validator(validators);
    cout << "Validator chosen: " << chosen_validator << endl;
    chain.push_back(pos_block);
}

bool Blockchain::validate_chain() const {
    for (size_t i = 1; i < chain.size(); ++i) {
        if (chain[i].prev_hash != chain[i - 1].hash) return false;
        if (chain[i].compute_hash() != chain[i].hash) return false;
    }
    return true;
}
