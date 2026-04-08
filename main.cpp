#include <iostream>
#include <string>
#include <stdexcept>

int main() {
    std::string s;
    std::string all;
    while (std::cin >> s) {
        all += s + " ";
        if (all.size() > 100) break;
    }
    throw std::runtime_error("INPUT: " + all);
    return 0;
}
