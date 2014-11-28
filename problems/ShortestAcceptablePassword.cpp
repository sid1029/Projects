/*
Below is an example of a password policy requirement for a user account in a domain:

The password should be at least six characters in length and should contain characters from all the following categories:
1. English uppercase letters (A through Z)
2. English lowercase letters (a through z)
3. Base 10 digits (0 through 9)

Given a string of alphanumeric (lower case, upper case, or digit) characters, your task is to find the length of its shortest contiguous substring which satisfies the above password policy requirement stated.

Input:
Each test case has a string of at most 200 alphanumeric characters.

Output:
Output one integer indicating the minimum length of a contiguous substring which satisfies the password policy. If there is no such substring, output 0.

Examples:

AliKam123test : 6
AbCdEfG : 0
88syadneerG : 10
Windows7released21october2009 : 8
*/
#include <iostream>
#include <limits>
#include <vector>
#include <string>

using namespace std;


int applyConstraints(char c, int cons[], int inc)
{
    if (c >= '0' && c <= '9')
    {
        cons[0] += inc;
        return 1;
    }
    else if (c >= 'a' && c <= 'z')
    {
        cons[1] += inc;
        return 2;
    }
    else if (c >= 'A' && c <= 'Z')
    {
        cons[2] += inc;
        return 3;
    }

    return 0;
}

bool updateLeast(const int start, const int end, int& least, const int cons[], const std::string& str)
{
    bool allPass = false;
    if (cons[0] > 0 && cons[1] > 0 && cons[2] > 0)
        allPass = true;
    bool updated = false;
    int curr = end - start + 1;
    if (curr < least && curr >= 6 && allPass)
    {
        least = curr;
        updated = true;
        for (int i = start; i <= end; ++i)
        {
            cout << str[i];
        }
        cout << endl;
    }
    return updated;
}

int ShortestPassSequence(std::string& str)
{
    int start = 0, end = 0, leastFound = numeric_limits<int>::max();
    int cons[3] = { 0, 0, 0 };

    while (start <= end && end < (int)str.length())
    {
        int curr = end - start + 1;
        applyConstraints(str[end], cons, 1);
        updateLeast(start, end, leastFound, cons, str);
        if (curr > 6)
        {
            // See how many chars can be removed from start while still keeping count >= 6.
            while (curr > 6)
            {
                int conType = applyConstraints(str[start], cons, 0);
                if (conType && cons[conType - 1] > 1)
                {
                    applyConstraints(str[start], cons, -1);
                    start++;
                    curr = end - start + 1;
                    updateLeast(start, end, leastFound, cons, str);
                }
                else
                    break;
            }
        }
        end++;
    }

    return ((leastFound == numeric_limits<int>::max() || leastFound < 6) ? 0 : leastFound);
}

int main()
{
    int len = 0;
	vector<string> inputs = {
		"AasdjkannA8dfkjjjlk",
		"AliKam123test",
		"AbCdEfG",
		"88syadneerG",
		"sdfsdjf833ojekfnjdfSKSDJFSDFn",
		"SKDJFHKSDJF38934r934090383409dsfkjlsdfksdhfj99sldfj",
		"SSDFLKJ0309';/.,/.,/,.sdfosdpkfk)#))#",
		"Windows7released21october2009"
	};

	for (size_t i = 0; i < inputs.size(); ++i)
	{
		cout << "Shortest valid pass in : " << inputs[i] << endl;
		len = ShortestPassSequence(inputs[i]);
		cout << len << endl << endl;
	}

    return 0;
}