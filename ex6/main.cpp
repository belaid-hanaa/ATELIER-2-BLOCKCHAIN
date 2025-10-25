#include <iostream>
#include <vector>
#include <chrono>
#include <random>
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

int main() {
    int RULE =30;
    int STEPS = 256;
    int SIZE = 256;
    const size_t N_BITS = 100000;

    string input1;
    input1.reserve(N_BITS);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 1);

    for (size_t i = 0; i < N_BITS; ++i)
        input1 += (dist(gen) ? '1' : '0');
    
    vector<int> hash1 = ac_hash_bit_to_bit(input1, RULE, STEPS,SIZE);
    double ones_perc =  ones_percentage(hash1);
    cout << "Percentage of Ones in the hash : " << ones_perc*100 <<"%";

    return 0;
}