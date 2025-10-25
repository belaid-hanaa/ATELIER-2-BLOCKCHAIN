#include "../ex3/blockchain.h"
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

int main() {
    int POW_DIFFICULTY = 2;
    vector<Transaction> txs = {
        {1, "Alice", "Bob", 100.0},
        {2, "Bob", "Charlie", 50.0},
        {3, "Charlie", "Alice", 25.0}
    };
    
    // ---------- AC Hash ----------
    cout << "========== Using AC ==========" << endl;
    Blockchain blockchain_ac("AC", POW_DIFFICULTY);
    uint64_t nonce_total = 0;

    auto start_ac = high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        blockchain_ac.add_block_pow(txs);
    }
    auto end_ac = high_resolution_clock::now();
    duration<double> duration_ac = end_ac - start_ac;

    for (auto &block : blockchain_ac.chain) {
        nonce_total += block.nonce;
    }

    cout << "Average iterations: " << nonce_total / 10 << endl;
    cout << "Execution time: " << duration_ac.count() << " seconds" << endl;
    cout << "Average time per block: " << (duration_ac.count() / 10.0) << " seconds" << endl;

    // ---------- SHA256 ----------
    cout << "\n========== Using SHA256 ==========" << endl;
    Blockchain blockchain_sha256("SHA256", POW_DIFFICULTY);
    nonce_total = 0;

    auto start_sha = high_resolution_clock::now();
    for (int i = 0; i < 10; i++) {
        blockchain_sha256.add_block_pow(txs);
    }
    auto end_sha = high_resolution_clock::now();
    duration<double> duration_sha = end_sha - start_sha;

    for (auto &block : blockchain_sha256.chain) {
        nonce_total += block.nonce;
    }

    cout << "Average iterations: " << nonce_total / 10 << endl;
    cout << "Execution time: " << duration_sha.count() << " seconds" << endl;
    cout << "Average time per block: " << (duration_sha.count() / 10.0) << " seconds" << endl;

    // ---------- Summary ----------
    cout << "\n========== Comparison Summary ==========" << endl;
    cout << left << setw(15) << "Method" 
         << setw(20) << "Avg Iterations" 
         << setw(20) << "Total Time (s)" 
         << setw(20) << "Avg Time/Block (s)" << endl;
    cout << string(75, '-') << endl;
    cout << left << setw(15) << "AC"
         << setw(20) << (blockchain_ac.chain.empty() ? 0 : nonce_total / 10)
         << setw(20) << duration_ac.count()
         << setw(20) << (duration_ac.count() / 10.0) << endl;
    cout << left << setw(15) << "SHA256"
         << setw(20) << (blockchain_sha256.chain.empty() ? 0 : nonce_total / 10)
         << setw(20) << duration_sha.count()
         << setw(20) << (duration_sha.count() / 10.0) << endl;

    return 0;
}