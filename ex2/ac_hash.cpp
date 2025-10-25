#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>


using namespace std;

string text_to_bits(const string& input) {
    string bits;
    for (unsigned char c : input) {
        for (int i = 7; i >= 0; --i)
            bits += ((c >> i) & 1) ? '1' : '0';
    }
    return bits;
}

vector<int> init_state(const string& bits) {
    vector<int> state(bits.size());
    for (size_t i = 0; i < bits.size(); ++i)
        state[i] = (bits[i] == '1') ? 1 : 0;
    return state;
}

vector<int> evolve(const vector<int>& current, int rule_number) {
    int n = current.size();
    vector<int> next(n, 0);
    vector<int> rule(8);
    for (int i = 0; i < 8; ++i)
        rule[i] = (rule_number >> i) & 1;

    for (int i = 0; i < n; ++i) {
        int left  = current[(i - 1 + n) % n];
        int self  = current[i];
        int right = current[(i + 1) % n];
        int pattern = (left << 2) | (self << 1) | right;
        next[i] = rule[pattern];
    }
    return next;
}


string bits_to_hex(const vector<int>& bits) {
    stringstream ss;
    ss << hex << setfill('0');
    for (size_t i = 0; i < bits.size(); i += 4) {
        int nibble = 0;
        for (int j = 0; j < 4 && (i + j) < bits.size(); ++j)
            nibble = (nibble << 1) | bits[i + j];
        ss << setw(1) << nibble;
    }
    return ss.str();
}

string ac_hash(const string& input, uint32_t rule, size_t steps,size_t CHUNK_SIZE) {
    // const size_t CHUNK_SIZE = 64;
    string bits = text_to_bits(input);

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

    return bits_to_hex(digest);
}