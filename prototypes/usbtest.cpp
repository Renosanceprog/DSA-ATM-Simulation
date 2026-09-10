#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main() {
    string targetPath = "";
    
    while (true)
    {
        for (int i = 68; i <= 90; i++) {
            string s = string(1, (char)i) + ":\\example.txt";
            
            ifstream file(s);
            
            if (file.is_open()) {
                targetPath = s;
                cout << "Payload found at: " << targetPath << endl;
                
                break;
            }
        }

        if (targetPath.empty()) {
            cout << "Could not find example.txt on any drive." << endl;
        }
    }

    return 0;
}