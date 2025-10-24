#include <iostream>
#include <vector>
#include <bitset>
#include <sstream>
using namespace std;

// Fonction pour initialiser l'état de l'automate à partir d'une chaîne de bits
vector<int> init_state_from_bits(const vector<int>& bits, int size) {
    vector<int> state(size, 0);
    for (int i = 0; i < size && i < bits.size(); i++) {
        state[i] = bits[i];
    }
    return state;
}

// Fonction d'évolution (comme ton code)
vector<int> evolve(const vector<int>& current, int rule) {
    int n = current.size();
    vector<int> next(n, 0);
    for (int i = 0; i < n; i++) {
        int left = (i == 0) ? 0 : current[i - 1];
        int center = current[i];
        int right = (i == n - 1) ? 0 : current[i + 1];
        int index = (left << 2) | (center << 1) | right;
        next[i] = (rule >> index) & 1;
    }
    return next;
}

// Convertit une chaîne en vecteur de bits
vector<int> string_to_bits(const string& input) {
    vector<int> bits;
    for (char c : input) {
        bitset<8> b(c); // 1 char = 8 bits
        for (int i = 7; i >= 0; i--) {
            bits.push_back(b[i]);
        }
    }
    return bits;
}

// Fonction de hachage basée sur automate cellulaire
string ac_hash(const string& input, uint32_t rule, size_t steps) {
    int hash_size = 256; // 256 bits
    vector<int> bits = string_to_bits(input);
    vector<int> state = init_state_from_bits(bits, hash_size);

    for (size_t i = 0; i < steps; i++) {
        state = evolve(state, rule);
    }

    // Convertir le vecteur de bits final en hexadécimal
    stringstream ss;
    for (int i = 0; i < hash_size; i += 4) {
        int val = (state[i] << 3) | (state[i + 1] << 2) | (state[i + 2] << 1) | state[i + 3];
        ss << hex << val;
    }
    return ss.str();
}

int main() {
    string input1 = "Bonjour";
    string input2 = "Bonsoir"; 

    string hash1 = ac_hash(input1, 30, 100);
    string hash2 = ac_hash(input2, 30, 100);

    cout << "Hash 1: " << hash1 << endl;
    cout << "Hash 2: " << hash2 << endl;

    return 0;
}
