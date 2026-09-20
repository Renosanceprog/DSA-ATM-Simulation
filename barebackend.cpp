#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>

using namespace std;
#define DBFILE ".\\savefile\\db.csv" //TEMPORARY SAVE LOCATION
#define KEYVALUE 187211
#define PINFILE "pin.code"

struct Account {
    int accountNumber;
    char accountName[51];
    char birthday[11];
    char contact[11];
    float depositBalance;
    bool isSavings;
    float savingsBalance;
    int pinCode;
    Account()
    {
        accountNumber = 0;
        depositBalance = 0.0f;
        savingsBalance = 0.0f;
        pinCode = 0;
        isSavings = false;
        
        accountName[0] = '\0';
        birthday[0] = '\0';
        contact[0] = '\0';
    }
};

class BankDatabase {
private:
    struct Node
    {
        Account data;
        Node* next;
        Node(Account data){this->data = data; next = nullptr;}
    };
    Node* head;

public:
    BankDatabase()
    {
        head = nullptr;
    }

    // ADT List Operations
    bool insertAccount(Account newAcc)
    {
        if (head == nullptr) head = new Node(newAcc);
        else
        {
            Node *p = head;
            while (p->next != nullptr && newAcc.accountNumber != p->data.accountNumber) p = p->next;
            if (newAcc.accountNumber == p->data.accountNumber) return false;
            p->next = new Node(newAcc);
        }
        return true;
    }
    bool getAccount(int accountNumber, Account &outAccount)
    {
        Node *p = head;
        while (p != nullptr && p->data.accountNumber != accountNumber) p = p->next;
        if (p == nullptr) return false;
        else
        {
            outAccount = p->data;
            return true;
        }
    }
    bool updateAccount(Account updatedAcc)
    {
        Node *p = head;
        while (p != nullptr && p->data.accountNumber != updatedAcc.accountNumber) p = p->next;
        if (p == nullptr) return false;
        else
        {
            p->data = updatedAcc;
            saveToFile();
            return true;
        }
    }
    
    // File I/O
    bool loadFromFile()
    {
        ifstream file(DBFILE);

        if (!file.is_open()) return false;
        
        string line;
        string tmp;
        
        while (getline(file, line))
        {
            if (line.empty()) continue;
            Account newNode;
            stringstream s(line);
            // Account Number
            getline(s, tmp, ',');
            newNode.accountNumber = stoi(tmp);

            // Account Name
            getline(s, tmp, ',');
            strncpy(newNode.accountName, tmp.c_str(), sizeof(newNode.accountName));
            newNode.accountName[sizeof(newNode.accountName)-1] = '\0';

            // Birthday
            getline(s, tmp, ',');
            strncpy(newNode.birthday, tmp.c_str(), sizeof(newNode.birthday));
            newNode.birthday[sizeof(newNode.birthday)-1] = '\0';

            // Contact
            getline(s, tmp, ',');
            strncpy(newNode.contact, tmp.c_str(), sizeof(newNode.contact));
            newNode.contact[sizeof(newNode.contact)-1] = '\0';

            // Deposit Balance
            getline(s, tmp, ',');
            newNode.depositBalance = stof(tmp);

            // Savings flag
            getline(s, tmp, ',');
            if (stoi(tmp) == 1) newNode.isSavings = true;
            else newNode.isSavings = false;

            // Savings Balance
            getline(s, tmp, ',');
            newNode.savingsBalance = stof(tmp);

            // Pin Code
            getline(s, tmp, ',');
            newNode.pinCode = stoi(tmp);

            insertAccount(newNode);
        }
        return true;
    }
    bool saveToFile()
    {
        ofstream file(DBFILE);

        if (!file.is_open()) return false;

        Node *p = head;
        while (p != nullptr)
        {
            file<<p->data.accountNumber<<","
            <<p->data.accountName<<","
            <<p->data.birthday<<","
            <<p->data.contact<<","
            <<p->data.depositBalance<<","
            <<p->data.isSavings<<","
            <<p->data.savingsBalance<<","
            <<p->data.pinCode<<"\n";
            p = p->next;
        }
        file.close();
        return true;
    }
};

class ATM {
private:
    BankDatabase* db;
    
    Account currentSession;
    bool sessionActive;

    int encryptCode(int pin){return pin ^ KEYVALUE ^ (KEYVALUE/2) ^ (KEYVALUE*13);}
    
    bool detectUSBAndVerifyPin(int expectedPin)
    {
        string codeStr;
        int codeInt;
        for (int i = 68; i <= 90; i++) {
            string s = string(1, (char)i) + ":\\pin.code";
            ifstream file(s);
            if (file.is_open())
            {
                getline(file, codeStr);
                codeInt = stoi(codeStr);
                if (codeInt == expectedPin) return true;
            }
        }
        return false;
    }

public:
    ATM(BankDatabase* database)
    {
        db = database;
        currentSession = Account();
        sessionActive = false;
    }
    // getter
    string getAccountName() { return currentSession.accountName; }

    // Registration & Authentication
    int registerAccount(Account newAcc, char driveLetter) // 0 = Success, 1 = Account already exists on drive, 2 = invalid drive, 3 = duplicate account
    { 
        string s = string(1, driveLetter) + ":\\" + PINFILE;
        fstream file1(s);
        if (file1.is_open()) return 1;

        ofstream file2(s);
        if (!file2.is_open()) return 2;

        newAcc.pinCode = encryptCode(newAcc.pinCode);
        if (!db->insertAccount(newAcc))
        {
            file2.close();
            remove(s.c_str());
            return 3;
        }
        
        file2<<newAcc.pinCode;
        db->saveToFile();
        return 0;
    };

    bool authenticateUser(int accountNumber, int inputPin)
    {
        int pinInt = encryptCode(inputPin);
        db->getAccount(accountNumber, currentSession);
        if (pinInt == currentSession.pinCode && detectUSBAndVerifyPin(pinInt))
        {
            sessionActive = true;
            return true;
        }
        logout();
        return false;
    }
    void logout(){currentSession = Account(); sessionActive = false;}
    
    float checkBalance(bool checkSavings)
    {
        if (!sessionActive) return 0.0f;
        if (checkSavings) return currentSession.savingsBalance;
        return currentSession.depositBalance;
    }
    
    bool deposit(float amount)
    {
        if (!sessionActive) return false;
        currentSession.depositBalance += amount;
        db->updateAccount(currentSession);
        return true;
    }
    
    bool withdraw(float amount, bool fromSavings)
    {
        if (!sessionActive) return false;

        float *balance = (fromSavings) ? &currentSession.savingsBalance : &currentSession.depositBalance;
        if (!(amount > *balance))
        {
            *balance -= amount;
            db->updateAccount(currentSession);
            return true;
        }
        return false;
    }
    bool savingsTransfer(float amount, bool fromSavings){
        if (!sessionActive) return false;

        float *source = (fromSavings) ? &currentSession.savingsBalance : &currentSession.depositBalance;
        float *dest = (!fromSavings) ? &currentSession.savingsBalance : &currentSession.depositBalance;
        if (!(amount > *source))
        {
            *source -= amount;
            *dest += amount;
            db->updateAccount(currentSession);
            return true;
        }
        return false;
    }
    int fundTransfer(int receiverAccNum, float amount){ // 0 = success, -1 = no session running, 1 = insufficient balance, 2 = receiver account does not exist
        if (!sessionActive) return -1;
        if (amount > currentSession.depositBalance) return 1;

        Account receiver;
        if (!db->getAccount(receiverAccNum, receiver)) return 2;

        currentSession.depositBalance -= amount;
        receiver.depositBalance += amount;

        db->updateAccount(receiver);
        db->updateAccount(currentSession);
        return 0;
    }
    
int changePin(int oldPin, int newPin) // 0 = success, 1 = entered old pin does not match current pin, 2 = new pin is same as old pin
    {
        int encOld = encryptCode(oldPin);
        int encNew = encryptCode(newPin);

        if (encOld != currentSession.pinCode) return 1;
        if (encNew == currentSession.pinCode) return 2;

        currentSession.pinCode = encNew;
        
        for (int i = 68; i <= 90; i++) {
            string s = string(1, (char)i) + ":\\" + PINFILE;
            fstream file(s, ios::in); 
            if (file.is_open()) {
                file.close();
                ofstream outFile(s, ios::trunc); 
                outFile << encNew;
                outFile.close();
                break;
            }
        }

        db->updateAccount(currentSession);
        return 0;
    }
};