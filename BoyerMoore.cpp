#include <iostream>
#include <vector>
#include <string>
#include <map>

using namespace std;

// Function to create the Bad Character Shift Table for the given pattern.
// This table is used to handle the case where a character mismatch is found.
map<char, int> preprocessBadCharacter(const string& pattern) {
    map<char, int> badCharShift;
    int m = pattern.size();  // Length of the pattern
    // Fill the table with the last occurrence of each character in the pattern
    for (int i = 0; i < m; i++) {
        badCharShift[pattern[i]] = i;
    }
    return badCharShift;
}

// Function to create the Good Suffix Shift Tables L and l for the given pattern.
// These tables are used when a suffix of the pattern matches a suffix of the text segment being compared.
void preprocessGoodSuffix(const string& pattern, vector<int>& L, vector<int>& l) {
    int m = pattern.size();  // Length of the pattern
    vector<int> N(m + 1);    // Temporary array for preprocessing
    L.assign(m, 0);          // Initialize L array
    l.assign(m + 1, 0);      // Initialize l array

    N[m] = m;
    int g = m, f = m;
    // Calculate the N table which stores the length of the longest suffix of the substring pattern[i...m-1] which is also a suffix of the pattern
    for (int i = m - 1; i >= 0; --i) {
        if (i > g && N[i + m - 1 - f] < i - g) {
            N[i] = N[i + m - 1 - f];
        } else {
            g = i;
            f = i;
            while (g >= 0 && pattern[g] == pattern[g + m - 1 - f]) {
                --g;
            }
            N[i] = f - g;
        }
    }

    // Use the N table to calculate the l table
    for (int i = 0; i < m; ++i) {
        l[N[i]] = i;
    }
    int j = 0;
    // Compute the L table from values in l and N
    for (int i = m - 1; i >= 0; --i) {
        if (N[i] == i + 1) {
            for (; j < m - 1 - i; ++j) {
                if (L[j] == 0) {
                    L[j] = m - 1 - i;
                }
            }
        }
    }
    for (int i = 0; i <= m - 1; ++i) {
        L[i] = m - 1 - l[i];
    }
}

// Boyer-Moore Algorithm for string matching
vector<int> boyerMoore(const string& text, const string& pattern) {
    map<char, int> badCharShift = preprocessBadCharacter(pattern);
    vector<int> L, l;
    preprocessGoodSuffix(pattern, L, l);

    vector<int> positions;  // To store the positions where pattern is found
    int n = text.size();    // Length of the text
    int m = pattern.size(); // Length of the pattern
    int s = 0;              // s is the shift of the pattern with respect to text

    // Main loop to slide the pattern over text one by one
    while(s <= n - m) {
        int j = m - 1;

        // Compare the pattern from right to left
        while (j >= 0 && pattern[j] == text[s + j])
            j--;

        // If the whole pattern has matched text segment
        if (j < 0) {
            positions.push_back(s);
            // Shift the pattern so that the next character in text aligns with the last occurrence in pattern
            s += (s + m < n) ? m - l[0] : 1;
        } else {
            // Calculate the shifts
            int badCharSkip = j - (badCharShift.count(text[s + j]) ? badCharShift[text[s + j]] : -1);
            int goodSuffixSkip = 0;
            if (j < m - 1)
                goodSuffixSkip = L[j + 1];
            // Shift pattern to the maximum of bad character and good suffix rules
            s += max(1, max(badCharSkip, goodSuffixSkip));
        }
    }
    return positions;
}

int main() {
    string text, pattern;

    // Get input from the user
    cout << "Enter the text: ";
    getline(cin, text);
    cout << "Enter the pattern to find: ";
    getline(cin, pattern);

    // Searching for the pattern in the text
    vector<int> positions = boyerMoore(text, pattern);

    if (positions.empty()) {
        cout << "Pattern not found in the text." << endl;
    } else {
        cout << "Pattern found at positions: ";
        for (int pos : positions) {
            cout << pos << " ";
        }
        cout << endl;
    }

    return 0;
}
