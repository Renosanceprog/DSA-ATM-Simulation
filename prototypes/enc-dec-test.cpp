#include <iostream>
#define KEYVALUE 187211

using namespace std;

int encryptCode(int pin){return pin ^ KEYVALUE ^ (KEYVALUE/2) ^ (KEYVALUE*13);}

int main(int argc, char const *argv[])
{
    int pin = 2528650;
    int nip = encryptCode(pin);
    cout<<nip<<endl;
    cout<<encryptCode(nip)<<endl;
    if (pin == encryptCode(nip)) cout<<"decrypted succesfully";
    return 0;
}
