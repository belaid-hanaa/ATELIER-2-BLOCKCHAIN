#include <iostream>
#include <vector>
#include <bitset>
#include <sstream>
#include <cstdint>
#include "ac_hash.h"

using namespace std;

int main() {
    string input1 = "Hello TESt long text test hello pass 256 qzdikhikqnzkdj,n";
    string input2 = "bonjour";
    string input3 = "bonsoir";


    uint32_t rule = 30;
    size_t steps = 256;

    string hash1 = ac_hash(input1, rule, steps,256);
    string hash2 = ac_hash(input2, rule, steps,256);
    string hash3 = ac_hash(input3, rule, steps,256);

    cout << "Rule: " << rule << ", Steps: " << steps << endl;
    cout << "Input1: " << input1 << " Hash: " << hash1 << endl;
    cout << "Input2: " << input2 << " Hash: " << hash2 << endl;
    cout << "Input3: " << input3 << " Hash: " << hash3 << endl;

    return 0;
}