#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

#define main backend_test_main
#include "barebackend.cpp"
#undef main

void clearScreen();
void header(string title);
void pauseScreen();
int readKey();
string getInput(bool hide, int maxLength);

bool isDigits(string s);
bool validAccountNumber(string s);
bool validPin(string s);

int getChoice(int min, int max);
int getAccountNumber();
int getPin(string text);
char getDriveLetter();

void mainMenu(ATM &atm);
void registerScreen(ATM &atm);
bool loginScreen(ATM &atm, int &accountNumber);
void transactionMenu(ATM &atm, int accountNumber);
void balanceScreen(ATM &atm);
void underDevelopment(string title);

int main()
{
    BankDatabase db;

    db.loadFromFile();

    ATM atm(&db);

    mainMenu(atm);

    return 0;
}

void mainMenu(ATM &atm)
{
    int accountNumber = 0;
    int choice;

    while (true)
    {
        clearScreen();

        header("SERIAL BANKING SYSTEM");

        cout << "[1] Login\n";
        cout << "[2] Register\n";
        cout << "[3] Exit\n\n";

        cout << "Enter choice: ";

        choice = getChoice(1, 3);

        switch (choice)
        {
        case 1:
            if (loginScreen(atm, accountNumber))
            {
                transactionMenu(atm, accountNumber);
            }

            break;

        case 2:
            registerScreen(atm);
            break;

        case 3:
            clearScreen();

            header("SERIAL BANKING SYSTEM");

            cout << "Thank you for using our banking system.\n\n";

            return;
        }
    }
}

void registerScreen(ATM &atm)
{
    Account newAcc;

    string name;
    string birthday;
    string contact;
    string input;

    int confirmPin;
    char drive;

    clearScreen();

    header("ACCOUNT REGISTRATION");

    while (true)
    {
        cout << "Account Number (5 digits): ";

        input = getInput(false, 5);

        if (validAccountNumber(input))
        {
            newAcc.accountNumber = stoi(input);
            break;
        }

        cout << "Invalid account number.\n";
        cout << "Enter exactly 5 digits.\n\n";
    }

    while (true)
    {
        cout << "Account Name: ";

        name = getInput(false, 50);

        if (!name.empty())
            break;

        cout << "Account name cannot be empty.\n\n";
    }

    strncpy(
        newAcc.accountName,
        name.c_str(),
        sizeof(newAcc.accountName) - 1);

    newAcc.accountName[sizeof(newAcc.accountName) - 1] = '\0';

    cout << "Birthday (MM/DD/YYYY): ";

    birthday = getInput(false, 10);

    strncpy(
        newAcc.birthday,
        birthday.c_str(),
        sizeof(newAcc.birthday) - 1);

    newAcc.birthday[sizeof(newAcc.birthday) - 1] = '\0';

    while (true)
    {
        cout << "Contact Number (10 digits): ";

        contact = getInput(false, 10);

        if (contact.length() == 10 &&
            isDigits(contact))
        {
            break;
        }

        cout << "Contact number must contain 10 digits.\n\n";
    }

    strncpy(
        newAcc.contact,
        contact.c_str(),
        sizeof(newAcc.contact) - 1);

    newAcc.contact[sizeof(newAcc.contact) - 1] = '\0';

    while (true)
    {
        cout << "Initial Deposit (Minimum PHP 5000): ";

        input = getInput(false, 15);

        stringstream ss(input);

        float amount;

        if (ss >> amount)
        {
            if (amount >= 5000)
            {
                newAcc.depositBalance = amount;
                break;
            }
        }

        cout << "Initial deposit must be at least PHP 5000.\n\n";
    }

    newAcc.savingsBalance = 0.0f;
    newAcc.isSavings = false;

    while (true)
    {
        newAcc.pinCode = getPin("Create PIN: ");

        confirmPin = getPin("Confirm PIN: ");

        if (newAcc.pinCode == confirmPin)
        {
            break;
        }

        cout << "\nPINs do not match.\n";
        cout << "Please try again.\n\n";
    }

    cout << "\n";

    drive = getDriveLetter();

    cout << "\nCreating account...\n";

    int result = atm.registerAccount(newAcc, drive);

    cout << "\n========================================\n";

    if (result == 0)
    {
        cout << "ACCOUNT SUCCESSFULLY CREATED!\n\n";

        cout << "Account Number: "
             << newAcc.accountNumber
             << "\n";

        cout << "Account Name: "
             << newAcc.accountName
             << "\n";

        cout << "Initial Balance: PHP "
             << fixed
             << setprecision(2)
             << newAcc.depositBalance
             << "\n";

        cout << "\nATM card created on drive "
             << drive
             << ":\\\n";
    }

    else if (result == 1)
    {
        cout << "REGISTRATION FAILED!\n\n";

        cout << "The USB already contains "
             << "an ATM PIN file.\n";
    }

    else if (result == 2)
    {
        cout << "REGISTRATION FAILED!\n\n";

        cout << "The selected USB drive "
             << "cannot be accessed.\n";
    }

    else if (result == 3)
    {
        cout << "REGISTRATION FAILED!\n\n";

        cout << "The account number "
             << "already exists.\n";
    }

    else
    {
        cout << "REGISTRATION FAILED!\n\n";

        cout << "Unknown error.\n";
    }

    pauseScreen();
}

bool loginScreen(ATM &atm, int &accountNumber)
{
    clearScreen();

    header("ACCOUNT LOGIN");

    cout << "Please insert your ATM flash drive.\n\n";

    accountNumber = getAccountNumber();

    int pin = getPin("PIN: ");

    cout << "\nChecking account...\n";

    if (atm.authenticateUser(accountNumber, pin))
    {
        cout << "\n========================================\n";
        cout << "          LOGIN SUCCESSFUL!\n";
        cout << "========================================\n";

        pauseScreen();

        return true;
    }

    cout << "\n========================================\n";
    cout << "            LOGIN FAILED!\n";
    cout << "========================================\n\n";

    cout << "Please check:\n";
    cout << "- Account Number\n";
    cout << "- PIN\n";
    cout << "- ATM Flash Drive\n";

    pauseScreen();

    return false;
}

void transactionMenu(ATM &atm, int accountNumber)
{
    int choice;

    while (true)
    {
        clearScreen();

        header("TRANSACTION MENU");

        cout << "Account Number: "
             << accountNumber
             << "\n\n";

        cout << "[1] Balance Inquiry\n";
        cout << "[2] Withdraw\n";
        cout << "[3] Deposit\n";
        cout << "[4] Fund Transfer\n";
        cout << "[5] Change PIN\n";
        cout << "[6] Logout\n\n";

        cout << "Enter choice: ";

        choice = getChoice(1, 6);

        switch (choice)
        {
        case 1:
            balanceScreen(atm);
            break;

        case 2:
            underDevelopment("WITHDRAW");
            break;

        case 3:
            underDevelopment("DEPOSIT");
            break;

        case 4:
            underDevelopment("FUND TRANSFER");
            break;

        case 5:
            underDevelopment("CHANGE PIN");
            break;

        case 6:
            atm.logout();

            clearScreen();

            header("LOGOUT");

            cout << "Account successfully logged out.\n";

            pauseScreen();

            return;
        }
    }
}

void balanceScreen(ATM &atm)
{
    int choice;

    clearScreen();

    header("BALANCE INQUIRY");

    cout << "[1] Deposit Balance\n";
    cout << "[2] Savings Balance\n";
    cout << "[3] Back\n\n";

    cout << "Enter choice: ";

    choice = getChoice(1, 3);

    if (choice == 1)
    {
        clearScreen();

        header("DEPOSIT BALANCE");

        cout << "Available Deposit Balance:\n\n";

        cout << "PHP "
             << fixed
             << setprecision(2)
             << atm.checkBalance(false)
             << "\n";

        pauseScreen();
    }

    else if (choice == 2)
    {
        clearScreen();

        header("SAVINGS BALANCE");

        cout << "Available Savings Balance:\n\n";

        cout << "PHP "
             << fixed
             << setprecision(2)
             << atm.checkBalance(true)
             << "\n";

        pauseScreen();
    }

    else
    {
        return;
    }
}

void underDevelopment(string title)
{
    clearScreen();

    header(title);

    cout << "----------------------------------------\n";
    cout << "           UNDER DEVELOPMENT\n";
    cout << "----------------------------------------\n\n";

    cout << "This feature is not yet available.\n\n";

    pauseScreen();
}

int getAccountNumber()
{
    string input;

    while (true)
    {
        cout << "Account Number: ";

        input = getInput(false, 5);

        if (validAccountNumber(input))
        {
            return stoi(input);
        }

        cout << "Account number must contain "
             << "exactly 5 digits.\n\n";
    }
}

int getPin(string text)
{
    string input;

    while (true)
    {
        cout << text;

        input = getInput(true, 4);

        if (validPin(input))
        {
            return stoi(input);
        }

        cout << "PIN must contain exactly "
             << "4 digits.\n\n";
    }
}

char getDriveLetter()
{
    string input;

    while (true)
    {
        cout << "USB Drive Letter (D-Z): ";

        input = getInput(false, 1);

        if (input.length() == 1)
        {
            char drive = toupper(
                static_cast<unsigned char>(input[0]));

            if (drive >= 'D' &&
                drive <= 'Z')
            {
                return drive;
            }
        }

        cout << "Invalid drive letter.\n";
        cout << "Enter a letter from D to Z.\n\n";
    }
}

int getChoice(int min, int max)
{
    string input;
    int choice;

    while (true)
    {
        input = getInput(false, 2);

        if (isDigits(input))
        {
            choice = stoi(input);

            if (choice >= min &&
                choice <= max)
            {
                return choice;
            }
        }

        cout << "Invalid choice.\n";
        cout << "Enter choice: ";
    }
}

bool isDigits(string s)
{
    if (s.empty())
    {
        return false;
    }

    for (int i = 0;
         i < static_cast<int>(s.length());
         i++)
    {
        if (!isdigit(
                static_cast<unsigned char>(s[i])))
        {
            return false;
        }
    }

    return true;
}

bool validAccountNumber(string s)
{
    if (s.length() != 5)
    {
        return false;
    }

    if (!isDigits(s))
    {
        return false;
    }

    if (s[0] == '0')
    {
        return false;
    }

    return true;
}

bool validPin(string s)
{
    if (s.length() != 4)
    {
        return false;
    }

    return isDigits(s);
}

int readKey()
{
#ifdef _WIN32

    return _getch();

#else

    struct termios oldSettings;
    struct termios newSettings;

    tcgetattr(
        STDIN_FILENO,
        &oldSettings);

    newSettings = oldSettings;

    newSettings.c_lflag &=
        ~(ICANON | ECHO);

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &newSettings);

    int ch = getchar();

    tcsetattr(
        STDIN_FILENO,
        TCSANOW,
        &oldSettings);

    return ch;

#endif
}

string getInput(bool hide, int maxLength)
{
    string input = "";

    int ch;

    while (true)
    {
        ch = readKey();

        if (ch == 13 || ch == 10)
        {
            cout << endl;
            break;
        }

        else if (ch == 8 || ch == 127)
        {
            if (!input.empty())
            {
                input.pop_back();

                cout << "\b \b";
                cout.flush();
            }
        }

#ifdef _WIN32

        else if (ch == 0 || ch == 224)
        {
            readKey();
        }

#endif

        else if (
            isprint(
                static_cast<unsigned char>(ch)) &&
            static_cast<int>(input.length()) < maxLength)
        {
            input += static_cast<char>(ch);

            if (hide)
            {
                cout << "*";
            }

            else
            {
                cout << static_cast<char>(ch);
            }

            cout.flush();
        }
    }

    return input;
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void header(string title)
{
    cout << "========================================\n";

    cout << "          "
         << title
         << "\n";

    cout << "========================================\n\n";
}

void pauseScreen()
{
    cout << "\nPress ENTER to continue...";

    cout.flush();

    int ch;

    do
    {
        ch = readKey();

    } while (
        ch != 13 &&
        ch != 10);

    cout << endl;
}