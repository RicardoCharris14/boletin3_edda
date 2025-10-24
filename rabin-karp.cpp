#include <iostream>
#include <vector>
#include <string>
using namespace std;

class RabinKarpHash {
private:
    const int mod = 1e9 + 7;
    const int base = 31;
    vector<int> hash;
    vector<int> power;

    // modular addition
    int add(int a, int b) {
        a += b;
        if (a >= mod) a -= mod;
        return a;
    }

    // modular subtraction
    int sub(int a, int b) {
        a -= b;
        if (a < 0) a += mod;
        return a;
    }

    // modular multiplication
    int mul(int a, int b) {
        return (int)((1LL * a * b) % mod);
    }

    // convert character to int 
    int charToInt(char c) {
        return c;
    }

public:
    // constructor: precomputes prefix hashes and powers
    RabinKarpHash(string &s) {
        int n = s.size();
        hash.resize(n);
        power.resize(n);

        hash[0] = charToInt(s[0]);
        power[0] = 1;

        for (int i = 1; i < n; ++i) {
            hash[i] = add(mul(hash[i - 1], base), charToInt(s[i]));
            power[i] = mul(power[i - 1], base);
        }
    }

    // get hash of substring s[l...r] in O(1)
    int getSubHash(int l, int r) {
        int h = hash[r];
        if (l > 0) {
            h = sub(h, mul(hash[l - 1], power[r - l + 1]));
        }
        return h;
    }
};

// Rabin-Karp search using hash class
vector<int> searchPattern(string &text, string &pattern) {
    int n = text.size(), m = pattern.size();

    RabinKarpHash textHash(text);
    RabinKarpHash patHash(pattern);

    int patternHash = patHash.getSubHash(0, m - 1);
    vector<int> result;

    for (int i = 0; i <= n - m; i++) {
        int subHash = textHash.getSubHash(i, i + m - 1);
        if (subHash == patternHash) {
            std::string_view text_match_view(text.data() + i, m);
            if (text_match_view == pattern){
                result.push_back(i);
            }
        }
    }

    return result;
}