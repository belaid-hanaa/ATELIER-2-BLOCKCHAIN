#include "../ex3/blockchain.h"

int main() {
    string HASH_MODE = "AC";
    int POW_DIFFICULTY = 2;
    Blockchain blockchain(HASH_MODE, POW_DIFFICULTY);
    cout << "Using " << HASH_MODE << "\n";

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

    cout << "\n========== Mining with PoW ==========\n";
    blockchain.add_block_pow(txs1);
    blockchain.add_block_pow(txs2);

    cout << "\nChain valid: " << (blockchain.validate_chain() ? "YES" : "NO") << "\n";

    cout << "\n========== Adding PoS Block ==========\n";
    vector<Transaction> txs_pos = {
        {6, "Charlie", "Bob", 10.0}
    };

    blockchain.add_block_pos(txs_pos, validators);

    cout << "\nChain valid: " << (blockchain.validate_chain() ? "YES" : "NO") << "\n";

    return 0;
}