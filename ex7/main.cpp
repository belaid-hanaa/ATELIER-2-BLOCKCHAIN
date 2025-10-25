#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
#include "../ex2/ac_hash.h"

using namespace std;
using namespace chrono;

vector<int> ac_hash_bit_to_bit(string bits, uint32_t rule, size_t steps,size_t CHUNK_SIZE) {
    if (bits.size() % CHUNK_SIZE != 0)
        bits.append(CHUNK_SIZE - bits.size() % CHUNK_SIZE, '0');

    vector<int> digest(CHUNK_SIZE, 0);

    for (size_t offset = 0; offset < bits.size(); offset += CHUNK_SIZE) {
        string chunk_bits = bits.substr(offset, CHUNK_SIZE);
        vector<int> state = init_state(chunk_bits);

        for (size_t t = 0; t < steps; ++t)
            state = evolve(state, rule);

        for (size_t i = 0; i < CHUNK_SIZE; ++i)
            digest[i] ^= state[i];

        digest = evolve(digest, rule);
    }

    return digest;
}

double ones_percentage(vector<int>& input1){
    double ones_sum = 0.0;
    for (size_t i = 0; i < input1.size(); i++){
        if (input1[i] == 1) ones_sum++;
    }
    return ones_sum/input1.size();
}

double avg_difference(vector<int>& input1,vector<int>& input2){
    double diff_sum = 0.0;
    for (size_t i = 0; i < input1.size(); i++){
        if (input1[i] != input2[i]) diff_sum++;
    }
    return diff_sum/input1.size();
}


int main() {
    const int STEPS = 256;
    const int SIZE = 256;
    const size_t N_BITS = 100000;

    // --- Generate random 100k-bit string ---
    string input1;
    input1.reserve(N_BITS);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 1);

    for (size_t i = 0; i < N_BITS; ++i)
        input1 += (dist(gen) ? '1' : '0');

    string input2 = input1;
    size_t diff_pos = input1.size()/2;
    if (diff_pos < N_BITS)
        input2[diff_pos] = (input2[diff_pos] == '0') ? '1' : '0';

    // --- Rules to test ---
    vector<int> rules = {30, 90, 110};

    // --- Table header ---
    cout << left << setw(8) << "Rule" 
         << setw(12) << "Time(s)" 
         << setw(18) << "Ones Percentage" 
         << setw(18) << "Difference" << endl;
    cout << string(62, '-') << endl;

    for (int rule : rules) {
        auto start = chrono::high_resolution_clock::now();
        vector<int> hash = ac_hash_bit_to_bit(input1, rule, STEPS, SIZE);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;

        vector<int> hash2 = ac_hash_bit_to_bit(input2, rule, STEPS, SIZE);

        double ones_perc = ones_percentage(hash);
        double avg_diff = avg_difference(hash, hash2);

        cout << left << setw(8) << rule 
             << setw(12) << duration.count()
             << setw(18) << to_string(ones_perc * 100) + "%"
             << setw(18) << to_string(avg_diff*100) + "%"<< endl;
    }

    return 0;
}
