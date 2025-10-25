#include <iostream>
#include <vector>
#include <chrono>
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

double avg_difference(vector<int>& input1,vector<int>& input2){
    double diff_sum = 0.0;
    for (size_t i = 0; i < input1.size(); i++){
        if (input1[i] != input2[i]) diff_sum++;
    }
    return diff_sum/input1.size();
}

int main() {
    int RULE =30;
    int STEPS = 256;
    int SIZE = 256;
    // only difference is in the 15th bits
    string input1 = "1011110100101010101011110101011";
    string input2 = "1011110100101000101011110101011";

    vector<int> hash1 = ac_hash_bit_to_bit(input1, RULE, STEPS,SIZE);
    vector<int> hash2 = ac_hash_bit_to_bit(input2, RULE, STEPS,SIZE);

    cout << "input bits : " << input1 << " Hash : ";
    for ( auto bit : hash1){
        cout << bit;
    }
    cout << endl;
    cout << "input bits : " << input2 << " Hash : ";
    for ( auto bit : hash2){
        cout << bit;
    }
    cout << endl;

    cout << "Avg bit difference : " << avg_difference(hash1,hash2)*100 <<"%";

    return 0;
}