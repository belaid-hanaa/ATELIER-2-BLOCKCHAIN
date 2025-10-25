#ifndef AC_HASH_H
#define AC_HASH_H

#include <string>
#include <vector>
#include <cstdint>

// Convert text to bits
std::string text_to_bits(const std::string& input);

// Initialize CA state from bits
std::vector<int> init_state(const std::string& bits);

// Evolve CA state with a given rule
std::vector<int> evolve(const std::vector<int>& current, int rule_number);

// Convert bit vector to hex string
std::string bits_to_hex(const std::vector<int>& bits);

// Cellular Automaton hash function
std::string ac_hash(const std::string& input, uint32_t rule, size_t steps, size_t CHUNK_SIZE = 256);

#endif // AC_HASH_H