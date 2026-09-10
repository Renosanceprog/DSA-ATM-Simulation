#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>

using namespace std;
#define DBFILE ".\\savefile\\db.csv" //TEMPORARY SAVE LOCATION


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
    //~BankDatabase();

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
        saveToFile();
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

//REMOVE THIS DEBUGGER METHODS WHEN WE ARE PASSING THIS TO RENE
    void debugPrintAll()
    {
        if (head == nullptr) {
            std::cout << "[DEBUG] Database is empty (Head is null).\n";
            return;
        }

        std::cout << "\n[DEBUG] --- CURRENT DATABASE STATE ---\n";
        Node* current = head;
        int nodeCount = 0;

        while (current != nullptr) {
            std::cout << "Node " << nodeCount << " -> " 
                      << "Acc: " << current->data.accountNumber << " | "
                      << "Name: " << current->data.accountName << " | "
                      << "Birthday:   " << current->data.birthday << " | "
                      << "Contact:    " << current->data.contact << " | "
                      << "Dep: PHP " << std::fixed << std::setprecision(2) << current->data.depositBalance << " | "
                      << "Sav: PHP " << current->data.savingsBalance << " | "
                      << "PIN: " << current->data.pinCode << "\n";
            
            current = current->next;
            nodeCount++;
        }
        std::cout << "[DEBUG] Total Nodes: " << nodeCount << "\n--------------------------------------\n";
    }

    // 2. Check if a specific account actually holds the right exact data
    void debugPrintAccount(int accNum)
    {
        Node* current = head;
        while (current != nullptr) {
            if (current->data.accountNumber == accNum) {
                std::cout << "\n[DEBUG] --- ACCOUNT FOUND ---\n";
                std::cout << "Account No: " << current->data.accountNumber << "\n";
                std::cout << "Name:       " << current->data.accountName << "\n";
                std::cout << "Birthday:   " << current->data.birthday << "\n";
                std::cout << "Contact:    " << current->data.contact << "\n";
                std::cout << "Deposit:    PHP " << current->data.depositBalance << "\n";
                std::cout << "Savings:    PHP " << current->data.savingsBalance << "\n";
                std::cout << "PIN:        " << current->data.pinCode << "\n";
                std::cout << "-----------------------------\n";
                return;
            }
            current = current->next;
        }
        std::cout << "[DEBUG] Account " << accNum << " not found in memory.\n";
    }

    // 3. Quick integrity check (useful to ensure no dangling pointers during deletes/inserts)
    int debugCountNodes()
    {
        int count = 0;
        Node* current = head;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
        return count;
    }
};

class ATM {
private:
    BankDatabase* db;

    // Helper method for the USB requirement
    bool checkUSBForPin(int expectedPin); 

public:
    ATM(BankDatabase* database);

    // Transaction Module Signatures
    int registerAccount(Account newAcc); // Returns status code (e.g., 0 for success, 1 for duplicate ID)
    bool authenticateUser(int accountNumber); // Triggers the USB check
    
    float checkBalance(int accountNumber, bool checkSavings);
    bool withdraw(int accountNumber, float amount, bool fromSavings);
    bool deposit(int accountNumber, float amount);
    bool fundTransfer(int senderAccNum, int receiverAccNum, float amount);
    bool changePin(int accountNumber, int oldPin, int newPin);
};

int main(int argc, char const *argv[])
{

    BankDatabase db = BankDatabase();
    db.loadFromFile();
    db.debugPrintAll();
    db.saveToFile();
    return 0;
}
