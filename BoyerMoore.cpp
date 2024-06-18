#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// Function to create the Bad Character Shift Table for the given pattern.
// This table maps each character in the pattern to its last occurrence within the pattern.
// It is used to determine how far the pattern can be shifted right when a mismatch occurs,
// based on the last occurrence of the mismatched character in the pattern.
map<char, int> preprocessBadCharacter(const string &pattern)
{
    map<char, int> badCharShift;
    int m = pattern.size(); // Length of the pattern
    for (int i = 0; i < m; i++)
    {
        badCharShift[pattern[i]] = i; // Record the last occurrence of each character
    }
    return badCharShift;
}

// Function to create the Good Suffix Shift Tables 'L' and 'l' for the given pattern.
// These tables are used when a suffix of the pattern matches a suffix of the text segment being compared.
// 'L' contains shift values to align another occurrence of the matched suffix within the pattern,
// while 'l' is used to handle cases where no internal match exists but a prefix of the pattern matches the suffix.
void preprocessGoodSuffix(const string &pattern, vector<int> &L, vector<int> &l)
{
    int m = pattern.size(); // Length of the pattern
    vector<int> N(m + 1);   // Temporary array for preprocessing
    L.assign(m, 0);         // Initialize L array
    l.assign(m + 1, 0);     // Initialize l array

    N[m] = m;
    int g = m, f = m;
    // Calculate the N table which stores the length of the longest suffix of the
    // substring pattern[i...m-1] that is also a suffix of the pattern
    for (int i = m - 1; i >= 0; --i)
    {
        if (i > g && N[i + m - 1 - f] < i - g)
        {
            N[i] = N[i + m - 1 - f];
        }
        else
        {
            g = i;
            f = i;
            while (g >= 0 && pattern[g] == pattern[g + m - 1 - f])
            {
                --g;
            }
            N[i] = f - g;
        }
    }
    // Use the N table to calculate the l table
    for (int i = 0; i < m; ++i)
    {
        l[N[i]] = i;
    }
    int j = 0;
    // Compute the L table from values in l and N
    for (int i = m - 1; i >= 0; --i)
    {
        if (N[i] == i + 1)
        {
            for (; j < m - 1 - i; ++j)
            {
                if (L[j] == 0)
                {
                    L[j] = m - 1 - i;
                }
            }
        }
    }
    for (int i = 0; i <= m - 1; ++i)
    {
        L[i] = m - 1 - l[i];
    }
}

// Boyer-Moore Algorithm for string matching with detailed visualization
vector<int> boyerMoore(const string &text, const string &pattern)
{
    map<char, int> badCharShift = preprocessBadCharacter(pattern);
    vector<int> L, l;
    preprocessGoodSuffix(pattern, L, l);

    vector<int> positions;  // To store the positions where the pattern is found
    int n = text.size();    // Length of the text
    int m = pattern.size(); // Length of the pattern
    int s = 0;              // s is the shift of the pattern with respect to text

    // Main loop to slide the pattern over text one by one
    while (s <= n - m)
    {
        int j = m - 1;

        // Compare the pattern from right to left
        while (j >= 0 && pattern[j] == text[s + j])
        {
            j--;
        }
        // If the pattern matches the text segment fully
        if (j < 0)
        {
            positions.push_back(s); // Record the current position as a match
            // Shift the pattern so that the next character in text aligns with the
            // last occurrence of the first character in the pattern.
            // If there is no more text to compare (i.e., s + m >= n), shift by 1 position.
            // Otherwise, shift by m - l[1], which is the shift suggested by the good suffix rule.
            cout << "\nText " << std::string(3, ' ') << ": " << text << "\n"
                 << "Pattern : " << string(s, ' ') << pattern << "\n"
                 << " -- Match found. Shifting by " << ((s + m < n) ? m - l[1] : 1)
                 << " positions using Good Suffix rule.\n";
            s += (s + m < n) ? m - l[1] : 1;
        }
        else
        {
            // Calculate the shifts when there is a mismatch
            // Bad character rule: shift the pattern so that the mismatched character in text
            // aligns with its last occurrence in the pattern.
            int badCharSkip = j - (badCharShift.count(text[s + j]) ? badCharShift[text[s + j]] : -1);

            // Good suffix rule: shift the pattern based on the matched suffix.
            // If the mismatch happens at position j, the goodSuffixSkip is determined by L[j + 1].
            int goodSuffixSkip = (j < m - 1) ? L[j + 1] : 0;

            // The actual shift to be applied is the maximum of the shifts suggested by the
            // bad character rule and the good suffix rule, but at least 1 to ensure progress.
            int shift = max(1, max(badCharSkip, goodSuffixSkip));
            cout << "\nText " << std::string(3, ' ') << ": " << text << "\n"
                 << "Pattern : " << string(s, ' ') << pattern << "\n"
                 << " -- Mismatch found. Shifting by " << shift
                 << " positions using " << (badCharSkip >= goodSuffixSkip ? "Bad Character" : "Good Suffix")
                 << " rule.\n";
            s += shift;
        }
    }
    return positions;
}

int main()
{
    string text, pattern;

    cout << "Enter the text: ";
    getline(cin, text);
    cout << "Enter the pattern to find: ";
    getline(cin, pattern);

    vector<int> positions = boyerMoore(text, pattern);

    if (positions.empty())
    {
        cout << "Pattern not found in the text." << endl;
    }
    else
    {
        cout << "Pattern found at positions: ";
        for (int pos : positions)
        {
            cout << pos << " ";
        }
        cout << endl;
    }

    return 0;
}
