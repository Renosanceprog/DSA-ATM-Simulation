#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>

// LINK TO BACKEND (Make sure barebackend.cpp is in the same folder and has NO main() function!)
#include "barebackend.cpp"

using namespace std;

/*========== TEXT COLORS & NAV ENUMS ==========*/
#define C_RESET  "\033[0m"      
#define C_GREEN  "\033[1;32m"   
#define C_YELLOW "\033[1;33m"  
#define C_CYAN   "\033[1;36m"   
#define C_RED    "\033[1;31m"    
#define C_BLUE   "\033[1;34m"   
#define C_MAG    "\033[1;35m"    

#define KEY_UP 72       
#define KEY_DOWN 80     
#define KEY_ENTER 13    
#define KEY_ESC 27      

/*========== INPUT WHITELISTS ==========*/
const string VALID_NUMBERS = "0123456789";
const string VALID_DECIMALS = "0123456789.";
const string VALID_NAME = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ .-,";
const string VALID_DRIVE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

/*========== SFX ENUMS ==========*/
typedef enum {
    SFX_MENU_MOVE, SFX_MENU_KEY_TYPING, SFX_MENU_SUCCESS, 
    SFX_MENU_INVALID, SFX_MENU_BLOCKED, SFX_MENU_ESCAPE, SFX_MENU_CONTINUE
} SoundID;

/*========== AUDIO ENGINE ==========*/
DWORD WINAPI SoundThread(LPVOID lpParam)
{
    SoundID id = (SoundID)(intptr_t)lpParam; 
    switch (id) {
        case SFX_MENU_MOVE:       Beep(1800, 5); break;
        case SFX_MENU_KEY_TYPING: Beep(1200, 10); break;
        case SFX_MENU_SUCCESS:    Beep(523, 100); Beep(659, 100); Beep(784, 100); Beep(1046, 200); break;
        case SFX_MENU_INVALID:    Beep(150, 100); break;
        case SFX_MENU_BLOCKED:    Beep(100, 50); Beep(100, 50); break;
        case SFX_MENU_ESCAPE:     Beep(400, 50); Beep(300, 80); break;
        case SFX_MENU_CONTINUE:   Beep(800, 50); Beep(1200, 50); break;
    }
    return 0;
}

void playAudio(SoundID id) {
    HANDLE hThread = CreateThread(NULL, 0, SoundThread, (LPVOID)(intptr_t)id, 0, NULL);
    if (hThread != NULL) CloseHandle(hThread); 
}

/*========== UI CORE FUNCTIONS ==========*/
void initWindow()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    SMALL_RECT windowSize = {0, 0, 79, 29};
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    COORD bufferSize = {80, 30};
    SetConsoleScreenBufferSize(hOut, bufferSize);
    
    HWND consoleWindow = GetConsoleWindow();
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

void drawBorder(const string& color)
{
    cout << "\033[2J" << color << "\033[1;1H\xC9";
    for (int i = 0; i < 78; i++) cout << "\xCD";
    cout << "\xBB";
    for (int i = 2; i < 30; i++) cout << "\033[" << i << ";1H\xBA\033[" << i << ";80H\xBA";
    cout << "\033[30;1H\xC8";
    for (int i = 0; i < 78; i++) cout << "\xCD";
    cout << "\xBC" << C_RESET << "\033[2;3H";
}

void printCentered(int row, const string& text, const string& color)
{
    int col = (80 - text.length()) / 2;
    if (col < 2) col = 2; 
    cout << "\033[" << row << ";" << col << "H" << color << text << C_RESET;
}

void clearInnerScreen() 
{
    for (int i = 2; i < 29; i++) {
        cout << "\033[" << i << ";2H";
        for (int j = 0; j < 78; j++) cout << " ";
    }
}

int getKeyPress() 
{
    int ch = _getch();
    if (ch == 0 || ch == 224) return _getch();
    return ch; 
} 

void printMenu(int start_row, const string& title, const vector<string>& options, int cursor_pos) 
{
    printCentered(start_row, title, C_YELLOW);
    int max_len = 0;
    for (const auto& opt : options) if (opt.length() > max_len) max_len = opt.length();
    int block_start_col = ((80 - (max_len + 8)) / 2); 

    for (size_t i = 0; i < options.size(); i++) 
    {
        cout << "\033[" << start_row + 2 + i << ";" << block_start_col << "H";
        if (cursor_pos == i + 1) {
            cout << C_CYAN << "  [ > ] " << options[i];
            for(int p = 0; p < (max_len - options[i].length()); p++) cout << " "; 
            cout << "  " << C_RESET;
        } else {
            cout << "        " << options[i];
            for(int p = 0; p < (max_len - options[i].length()); p++) cout << " "; 
            cout << "  ";
        }
    }
}

int runInteractiveMenu(int start_row, const string& title, const vector<string>& options) 
{
    int cursor_pos = 1; 
    int num_options = options.size();
    printMenu(start_row, title, options, cursor_pos); 

    while (true) 
    { 
        int key = getKeyPress();
        if (key == KEY_UP) {
            playAudio(SFX_MENU_MOVE);
            cursor_pos = (cursor_pos == 1) ? num_options : cursor_pos - 1;
            printMenu(start_row, title, options, cursor_pos); 
        } 
        else if (key == KEY_DOWN) {
            playAudio(SFX_MENU_MOVE);
            cursor_pos = (cursor_pos == num_options) ? 1 : cursor_pos + 1;
            printMenu(start_row, title, options, cursor_pos); 
        } 
        else if (key == KEY_ENTER) {
            playAudio(SFX_MENU_CONTINUE);
            return cursor_pos; 
        }
        else if (key == KEY_ESC) {
            playAudio(SFX_MENU_ESCAPE);
            return -1; 
        }
    }
}

/*========== INPUT ENGINES ==========*/
bool getValidInput(int row, int col, string& buffer, int max_len, const string& valid_chars, bool obfuscated)
{
    buffer.clear();
    
    // FIX: Prevent the input field from wrapping around and breaking the right border
    if (max_len > 78 - col) max_len = 78 - col; 
    
    cout << "\033[" << row << ";" << col << "H";
    for (int i = 0; i < max_len; i++) cout << " ";
    
    while (true) 
    {
        cout << "\033[" << row << ";" << col << "H";
        for (size_t i = 0; i < buffer.length(); i++) cout << (obfuscated ? '*' : buffer[i]);
        
        // NEW: Draw a block caret so the user knows where they are typing
        if (buffer.length() < max_len) {
            cout << "\033[7m \033[0m\b"; // Inverted space (solid block), then backspace
        } else {
            cout << " \b"; // Clear leftover char if at max length
        }

        int ch = _getch();
        if (ch == 0 || ch == 224) { _getch(); continue; }

        if (ch == KEY_ESC) return false; 
        else if (ch == KEY_ENTER) {
            if (buffer.length() > 0) {
                cout << " \b"; // Wipe the caret cleanly before returning
                return true; 
            }
        }
        else if (ch == 8) { 
            if (!buffer.empty()) {
                buffer.pop_back();
                // Erase character and caret cleanly
                cout << "\033[" << row << ";" << col + buffer.length() << "H   ";
                playAudio(SFX_MENU_KEY_TYPING);
            }
        }
        else {
            if (buffer.length() < max_len && valid_chars.find((char)ch) != string::npos) {
                buffer += (char)ch;
                playAudio(SFX_MENU_KEY_TYPING);
            } else if (buffer.length() >= max_len) {
                playAudio(SFX_MENU_BLOCKED);
            }
        }
    }
}

bool getDateInput(int row, int col, string& buffer)
{
    buffer.clear();
    // Clear 10 spaces visually for DD/MM/YYYY
    cout << "\033[" << row << ";" << col << "H          "; 
    
    while (true) 
    {
        cout << "\033[" << row << ";" << col << "H";
        for (size_t i = 0; i < buffer.length(); i++) {
            cout << buffer[i];
            // Visually inject slashes after DD and MM
            if (i == 1 || i == 3) cout << "/"; 
        }
        
        // NEW: Draw a block caret for the date field
        if (buffer.length() < 8) {
            cout << "\033[7m \033[0m\b"; 
        } else {
            cout << " \b";
        }

        int ch = _getch();
        if (ch == 0 || ch == 224) { _getch(); continue; }

        if (ch == KEY_ESC) return false; 
        else if (ch == KEY_ENTER) {
            if (buffer.length() == 8) {
                cout << " \b"; // Wipe caret before returning
                return true; 
            }
        }
        else if (ch == 8) { 
            if (!buffer.empty()) {
                buffer.pop_back();
                // Wipes line to redraw slashes correctly
                cout << "\033[" << row << ";" << col << "H          "; 
                playAudio(SFX_MENU_KEY_TYPING);
            }
        }
        else {
            if (buffer.length() < 8 && VALID_NUMBERS.find((char)ch) != string::npos) {
                buffer += (char)ch;
                playAudio(SFX_MENU_KEY_TYPING);
            } else if (buffer.length() >= buffer.length()) {
                playAudio(SFX_MENU_BLOCKED);
            }
        }
    }
}

/*========== MENU STATE FUNCTIONS ==========*/

void runCheckBalance(ATM& atm)
{
    vector<string> options = {"Deposit Account", "Savings Account"};
    
    while (true) 
    {
        clearInnerScreen();
        int choice = runInteractiveMenu(10, "BALANCE INQUIRY", options);
        
        if (choice == -1) return; // User pressed ESC, go back to Dashboard
        
        clearInnerScreen();
        printCentered(10, "--- ACCOUNT BALANCE ---", C_CYAN);
        
        char buffer[100];
        if (choice == 1) {
            snprintf(buffer, sizeof(buffer), "Current Deposit Balance: PHP %.2f", atm.checkBalance(false));
            printCentered(14, buffer, C_GREEN);
        } else if (choice == 2) {
            snprintf(buffer, sizeof(buffer), "Current Savings Balance: PHP %.2f", atm.checkBalance(true));
            printCentered(14, buffer, C_GREEN);
        }
        
        printCentered(22, "[ Press ENTER to return to menu ]", C_YELLOW);
        while (getKeyPress() != KEY_ENTER);
    }
}

void runDeposit(ATM& atm)
{
    clearInnerScreen();
    printCentered(8, "--- DEPOSIT FUNDS ---", C_CYAN);
    printCentered(25, "[ Press ESC to cancel ]", C_YELLOW);
    
    cout << "\033[14;20H" << "Enter Amount: PHP ";
    
    auto printError = [](const string& msg) {
        cout << "\033[27;2H"; 
        for(int i = 0; i < 76; i++) cout << " "; 
        printCentered(27, msg, C_RED);
    };
    auto clearError = []() {
        cout << "\033[27;2H";
        for(int i = 0; i < 76; i++) cout << " "; 
    };

    string amountStr;

    while (true) 
    {
        // Ask for the deposit amount (Max 10 characters, Decimal numbers only)
        if (!getValidInput(14, 38, amountStr, 10, VALID_DECIMALS, false)) return;
        
        if (amountStr.length() > 0) {
            float amount = stof(amountStr);
            
            if (amount >= 100.0f) { // Example sanity check: Minimum 100 PHP deposit
                clearError();
                printCentered(18, " Processing deposit... please wait. ", C_YELLOW);
                
                if (atm.deposit(amount)) {
                    clearInnerScreen();
                    printCentered(12, "Deposit Successful!", C_GREEN);
                    
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), "New Deposit Balance: PHP %.2f", atm.checkBalance(false));
                    printCentered(14, buffer, C_RESET);
                    
                    printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
                    while (getKeyPress() != KEY_ENTER);
                    return; 
                } else {
                    printError("Transaction failed! Session may have expired.");
                    return;
                }
            } else {
                printError("Minimum deposit amount is PHP 100.");
            }
        } else {
            printError("Please enter a valid amount.");
        }
    }
}

void runWithdraw(ATM& atm)
{
    vector<string> options = {"From Deposit Account", "From Savings Account"};
    bool isProceed = true;

    auto printError = [](const string& msg) {
        cout << "\033[27;2H"; 
        for(int i = 0; i < 76; i++) cout << " "; 
        printCentered(27, msg, C_RED);
    };
    auto clearError = []() {
        cout << "\033[27;2H";
        for(int i = 0; i < 76; i++) cout << " "; 
    };

    while (true) 
    {
        clearInnerScreen();
        int choice = runInteractiveMenu(10, "WITHDRAW FUNDS", options);
        
        if (choice == -1) return; // User pressed ESC, go back to Dashboard
        
        bool isSavings = (choice == 2);
        float curBal = isSavings ? atm.checkBalance(true) : atm.checkBalance(false);
        string accountName = isSavings ? "Savings" : "Deposit";

        
        if (curBal == 0) {
            printError("Could not start Transaction, " + accountName +" has Zero Balance.");
            printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
            while (getKeyPress() != KEY_ENTER);
            isProceed = false;
        }
        else {
            isProceed = true;
            clearInnerScreen();
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "Current Balance: PHP %.2f", atm.checkBalance(false));
            printCentered(10, buffer, C_GREEN);
            printCentered(8, "--- WITHDRAW FROM " + (isSavings ? string("SAVINGS") : string("DEPOSIT")) + " ---", C_CYAN);
            printCentered(25, "[ Press ESC to cancel ]", C_YELLOW);
            
            cout << "\033[14;20H" << "Enter Amount: PHP ";
        }

        string amountStr;
        while (isProceed) 
        {
            // If they press ESC here, it breaks this inner loop and takes them back to the Deposit/Savings choice menu
            if (!getValidInput(14, 38, amountStr, 10, VALID_DECIMALS, false)) break; 
            
            if (amountStr.length() > 0) {
                float amount = stof(amountStr);
                if (amount >= 100.0f) { // Real ATMs usually dispense a minimum of 100
                    clearError();
                    printCentered(18, " Processing withdrawal... please wait. ", C_YELLOW);
                    
                    if (atm.withdraw(amount, isSavings)) {
                        clearInnerScreen();
                        printCentered(12, "Withdrawal Successful!", C_GREEN);
                        printCentered(14, "Please take your cash.", C_RESET);
                        
                        char buffer[100];
                        snprintf(buffer, sizeof(buffer), "New %s Balance: PHP %.2f", accountName.c_str(), atm.checkBalance(isSavings));
                        printCentered(16, buffer, C_RESET);
                        
                        printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
                        while (getKeyPress() != KEY_ENTER);
                        return; // Pop all the way back to main hub
                    } else {
                        // Erase the processing text
                        cout << "\033[18;2H"; 
                        for(int i=0; i<76; i++) cout << " "; 
                        
                        printError("Transaction Failed: Insufficient " + accountName + " Balance.");
                    }
                } else {
                    printError("Minimum withdrawal amount is PHP 100.");
                }
            } else {
                printError("Please enter a valid amount.");
            }
        }
    }
}

void runSavingsTransfer(ATM& atm)
{
    vector<string> options = {"Deposit to Savings", "Savings to Deposit"};
    bool isProceed = true;

    auto printError = [](const string& msg) {
        cout << "\033[27;2H"; 
        for(int i = 0; i < 76; i++) cout << " "; 
        printCentered(27, msg, C_RED);
    };
    auto clearError = []() {
        cout << "\033[27;2H";
        for(int i = 0; i < 76; i++) cout << " "; 
    };

    while (true) 
    {
        clearInnerScreen();
        int choice = runInteractiveMenu(10, "INTERNAL TRANSFER", options);
        
        if (choice == -1) return;
        
        // choice 1: Deposit->Savings (fromSavings = false)
        // choice 2: Savings->Deposit (fromSavings = true)
        bool fromSavings = (choice == 2);
        float curBal = fromSavings ? atm.checkBalance(true) : atm.checkBalance(false);
        string sourceName = fromSavings ? "Savings" : "Deposit";
        string destName = fromSavings ? "Deposit" : "Savings";

        if (curBal == 0) {
            printError("Could not start Transaction, " + sourceName +" has Zero Balance.");
            printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
            while (getKeyPress() != KEY_ENTER);
            isProceed = false;
        }
        else {
            isProceed = true;
            clearInnerScreen();
            char buffer[100];
            snprintf(buffer, sizeof(buffer), "Current Balance: PHP %.2f", atm.checkBalance(false));
            printCentered(10, buffer, C_GREEN);
            printCentered(8, "--- WITHDRAW FROM " + (fromSavings ? string("SAVINGS") : string("DEPOSIT")) + " ---", C_CYAN);
            printCentered(25, "[ Press ESC to cancel ]", C_YELLOW);
            
            cout << "\033[14;20H" << "Enter Amount: PHP ";
        }

        string amountStr;
        while(isProceed) 
        {
            if (!getValidInput(14, 38, amountStr, 10, VALID_DECIMALS, false)) break;
            
            if (amountStr.length() > 0) {
                float amount = stof(amountStr);
                if (amount > 0) {
                    clearError();
                    printCentered(18, " Processing transfer... please wait. ", C_YELLOW);
                    
                    if (atm.savingsTransfer(amount, fromSavings)) {
                        clearInnerScreen();
                        printCentered(12, "Transfer Successful!", C_GREEN);
                        
                        char buf1[100], buf2[100];
                        snprintf(buf1, sizeof(buf1), "New Deposit Balance: PHP %.2f", atm.checkBalance(false));
                        snprintf(buf2, sizeof(buf2), "New Savings Balance: PHP %.2f", atm.checkBalance(true));
                        
                        printCentered(15, buf1, C_RESET);
                        printCentered(16, buf2, C_RESET);
                        
                        printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
                        while (getKeyPress() != KEY_ENTER);
                        return;
                    } else {
                        // Erase processing text
                        cout << "\033[18;2H"; 
                        for(int i=0; i<76; i++) cout << " "; 
                        
                        printError("Transfer Failed: Insufficient " + sourceName + " Balance.");
                    }
                } else {
                    printError("Enter an amount greater than 0.");
                }
            } else {
                printError("Please enter a valid amount.");
            }
        }
    }
}

void runFundTransfer(ATM& atm)
{
    auto printError = [](const string& msg) {
        cout << "\033[27;2H"; 
        for(int i = 0; i < 76; i++) cout << " "; 
        printCentered(27, msg, C_RED);
    };
    auto clearError = []() {
        cout << "\033[27;2H";
        for(int i = 0; i < 76; i++) cout << " "; 
    };

    float curBal = atm.checkBalance(false); // Fund transfers happen from the Deposit account

    if (curBal == 0) {
        clearInnerScreen();
        printCentered(12, "--- FUND TRANSFER ---", C_CYAN);
        printError("Could not start Transaction. Deposit Account has Zero Balance.");
        printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
        while (getKeyPress() != KEY_ENTER);
        return;
    }

    clearInnerScreen();
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Current Deposit Balance: PHP %.2f", curBal);
    printCentered(8, buffer, C_GREEN);
    
    printCentered(10, "--- TRANSFER FUNDS TO ANOTHER ACCOUNT ---", C_CYAN);
    printCentered(25, "[ Press ESC to cancel ]", C_YELLOW);

    cout << "\033[14;15H" << "Receiver Account No (5 digits) : ";
    cout << "\033[16;15H" << "Amount to Transfer         PHP : ";

    string accStr, amountStr;
    int step = 0;

    while (step >= 0 && step <= 2) 
    {
        switch (step) 
        {
            case 0: // Receiver Account
                if (!getValidInput(14, 48, accStr, 5, VALID_NUMBERS, false)) return;
                
                if (accStr.length() == 5) {
                    step++; clearError();
                } else {
                    printError("Account Number must be exactly 5 digits.");
                }
                break;

            case 1: // Amount
                if (!getValidInput(16, 48, amountStr, 10, VALID_DECIMALS, false)) return;
                
                if (amountStr.length() > 0 && stof(amountStr) > 0) {
                    step++; clearError();
                } else {
                    printError("Please enter a valid amount greater than 0.");
                }
                break;

            case 2: // Submit to Backend
                printCentered(20, " Processing transfer... please wait. ", C_YELLOW);
                
                int receiverAcc = stoi(accStr);
                float amount = stof(amountStr);
                
                int status = atm.fundTransfer(receiverAcc, amount);
                
                if (status == 0) {
                    clearInnerScreen();
                    printCentered(12, "Transfer Successful!", C_GREEN);
                    
                    char buf[100];
                    snprintf(buf, sizeof(buf), "New Deposit Balance: PHP %.2f", atm.checkBalance(false));
                    printCentered(15, buf, C_RESET);
                    
                    printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
                    while (getKeyPress() != KEY_ENTER);
                    return;
                }
                else if (status == 1) {
                    // Erase processing text
                    cout << "\033[20;2H"; for(int i=0; i<76; i++) cout << " ";
                    printError("Transfer Failed: Insufficient Balance.");
                    step = 1; // Jump back to Amount field
                }
                else if (status == 2) {
                    cout << "\033[20;2H"; for(int i=0; i<76; i++) cout << " ";
                    printError("Transfer Failed: Receiver Account does not exist.");
                    step = 0; // Jump back to Account field
                }
                else {
                    cout << "\033[20;2H"; for(int i=0; i<76; i++) cout << " ";
                    printError("Session error. Please log out and try again.");
                    return;
                }
                break;
        }
    }
}
void runChangePin(ATM& atm)
{
    clearInnerScreen();
    printCentered(8, "--- CHANGE ACCOUNT PIN ---", C_CYAN);
    printCentered(25, "[ Press ESC to cancel ]", C_YELLOW);

    cout << "\033[12;20H" << "Enter Current 4-Digit PIN : ";
    cout << "\033[14;20H" << "Enter NEW 4-Digit PIN     : ";
    cout << "\033[16;20H" << "Confirm NEW 4-Digit PIN   : ";

    auto printError = [](const string& msg) {
        cout << "\033[27;2H"; 
        for(int i = 0; i < 76; i++) cout << " "; 
        printCentered(27, msg, C_RED);
    };
    auto clearError = []() {
        cout << "\033[27;2H";
        for(int i = 0; i < 76; i++) cout << " "; 
    };

    string oldPinStr, newPinStr, confPinStr;
    int step = 0;

    while (step >= 0 && step <= 3) 
    {
        switch (step) 
        {
            case 0: // Current PIN
                if (!getValidInput(12, 48, oldPinStr, 4, VALID_NUMBERS, true)) return;
                
                if (oldPinStr.length() == 4) {
                    step++; clearError();
                } else {
                    printError("PIN must be exactly 4 digits.");
                }
                break;

            case 1: // New PIN
                if (!getValidInput(14, 48, newPinStr, 4, VALID_NUMBERS, true)) return;
                
                if (newPinStr.length() == 4) {
                    step++; clearError();
                } else {
                    printError("PIN must be exactly 4 digits.");
                }
                break;

            case 2: // Confirm New PIN
                if (!getValidInput(16, 48, confPinStr, 4, VALID_NUMBERS, true)) return;
                
                if (newPinStr == confPinStr) {
                    step++; clearError();
                } else {
                    printError("New PINs do not match! Please try again.");
                    confPinStr.clear();
                    cout << "\033[16;48H    "; // Visually erase just the confirm box
                }
                break;

            case 3: // Submit to Backend
                printCentered(20, " Updating Security Settings... please wait. ", C_YELLOW);
                
                int oldPin = stoi(oldPinStr);
                int newPin = stoi(newPinStr);
                
                int status = atm.changePin(oldPin, newPin);
                
                if (status == 0) {
                    clearInnerScreen();
                    printCentered(12, "PIN Changed Successfully!", C_GREEN);
                    printCentered(14, "Your USB Drive has been securely updated.", C_RESET);
                    
                    printCentered(22, "[ Press ENTER to return to Dashboard ]", C_YELLOW);
                    while (getKeyPress() != KEY_ENTER);
                    return;
                }
                else if (status == 1) {
                    cout << "\033[20;2H"; for(int i=0; i<76; i++) cout << " ";
                    printError("Error: The Current PIN you entered is incorrect.");
                    
                    // Wipe the current PIN box and kick them to step 0
                    oldPinStr.clear();
                    cout << "\033[12;48H    ";
                    step = 0; 
                }
                else if (status == 2) {
                    cout << "\033[20;2H"; for(int i=0; i<76; i++) cout << " ";
                    printError("Error: New PIN cannot be the same as your Current PIN.");
                    
                    // Wipe the new and confirm PIN boxes and kick them to step 1
                    newPinStr.clear();
                    confPinStr.clear();
                    cout << "\033[14;48H    ";
                    cout << "\033[16;48H    ";
                    step = 1;
                }
                break;
        }
    }
}

void runMainHub(ATM& atm)
{
    vector<string> hubMenu = {
        "Check Balance",
        "Deposit",
        "Withdraw",
        "Savings Transfer",
        "Fund Transfer",
        "Change PIN",
        "Logout"
    };

    while (true) 
    {
        clearInnerScreen();
        int choice = runInteractiveMenu(7, "MAIN DASHBOARD", hubMenu);

        if (choice != -1 && choice != 7 && !atm.isSessionValid()) {
            playAudio(SFX_MENU_INVALID);
            clearInnerScreen();
            printCentered(12, "SECURITY ALERT", C_RED);
            printCentered(14, "USB Drive Ejected! Session Terminated.", C_YELLOW);
            printCentered(22, "[ Press ENTER to return to Main Menu ]", C_RESET);
            while (getKeyPress() != KEY_ENTER);
            return; // Kicks them completely out of the Hub
        }
        switch (choice) {
            case 1: runCheckBalance(atm); break;
            case 2: runDeposit(atm); break;
            case 3: runWithdraw(atm); break;
            case 4: runSavingsTransfer(atm); break;
            case 5: runFundTransfer(atm); break;
            case 6: runChangePin(atm); break;
            case 7: 
            case -1: // Logout or ESC
                atm.logout();
                return; // Pops stack back to main menu
        }
    }
}

void runLogin(ATM& atm)
{
    // ==========================================
    // PHASE 1: REAL-LIFE USB DETECTION
    // ==========================================
    while (true) {
        clearInnerScreen();
        printCentered(10, "--- ATM CARD DETECTION ---", C_CYAN);
        
        bool usbFound = false;
        for (int i = 68; i <= 90; i++) { // ASCII 68 = 'D', 90 = 'Z'
            string path = string(1, (char)i) + ":\\pin.code";
            ifstream file(path);
            if (file.is_open()) {
                usbFound = true;
                break;
            }
        }

        if (!usbFound) {
            printCentered(14, "Please insert your registered ATM USB Drive.", C_YELLOW);
            printCentered(16, "[ Press ENTER to scan again | Press ESC to cancel ]", C_RESET);
            
            int key;
            do { key = getKeyPress(); } while (key != KEY_ENTER && key != KEY_ESC);
            
            if (key == KEY_ESC) return; // User gave up, return to main menu
            continue; // They pressed enter, loop back and check USBs again
        }
        
        break; // USB found! Move to the actual login form.
    }

    // ==========================================
    // PHASE 2: AUTHENTICATION FORM
    // ==========================================
    clearInnerScreen();
    printCentered(6, "--- SECURE LOGIN ---", C_CYAN);
    printCentered(25, "[ Press ESC to cancel at any time ]", C_YELLOW);

    cout << "\033[10;20H" << "Account Number (5 digits) : ";
    cout << "\033[12;20H" << "PIN Code (4 digits)       : ";

    auto printError = [](const string& msg) {
        cout << "\033[27;2H"; 
        for(int i=0; i<76; i++) cout << " "; 
        printCentered(27, msg, C_RED);
    };
    auto clearError = []() {
        cout << "\033[27;2H";
        for(int i=0; i<76; i++) cout << " "; 
    };

    string accStr, pinStr;
    int step = 0;

    while (step >= 0 && step <= 2) 
    {
        switch(step)
        {
            case 0: // Account Number
                if (!getValidInput(10, 48, accStr, 5, VALID_NUMBERS, false)) return;
                
                if (accStr.length() == 5) {
                    step++; clearError();
                } else {
                    printError("Account Number must be exactly 5 digits.");
                }
                break;

            case 1: // PIN Code
                if (!getValidInput(12, 48, pinStr, 4, VALID_NUMBERS, true)) return;
                
                if (pinStr.length() == 4) {
                    step++; clearError();
                } else {
                    printError("PIN must be exactly 4 digits.");
                }
                break;
                
        case 2: // Submit to Backend
            {
                printCentered(16, " Authenticating... please wait. ", C_YELLOW);
                
                int accNum = stoi(accStr);
                int pinNum = stoi(pinStr);

                if (atm.authenticateUser(accNum, pinNum)) {
                    // Login Success!
                    playAudio(SFX_MENU_SUCCESS);
                    
                    clearInnerScreen();
                    printCentered(12, "Login Successful!", C_GREEN);
                    
                    // Greet the user by name
                    string welcome = "Welcome back, " + atm.getAccountName() + "!";
                    printCentered(14, welcome, C_RESET);
                    
                    printCentered(22, "[ Press ENTER to continue ]", C_YELLOW);
                    while(getKeyPress() != KEY_ENTER);
                    
                    runMainHub(atm);
                    return; 
                } else {
                    // Login Failed!
                    playAudio(SFX_MENU_INVALID);
                    
                    cout << "\033[16;2H"; 
                    for(int i=0; i<76; i++) cout << " "; 
                    
                    printError("Login Failed: Invalid Account, PIN, or wrong USB.");
                    
                    pinStr.clear();
                    cout << "\033[12;48H    "; 
                    step = 1; 
                }
                break;
            }
        }
    }
}

void runRegister(ATM& atm)
{
    clearInnerScreen();
    printCentered(4, "--- OPEN NEW ACCOUNT ---", C_CYAN);
    printCentered(25, "[ Press ESC at any time to cancel ]", C_YELLOW);

    cout << "\033[8;15H"  << "Account Number (5 digits) : ";
    cout << "\033[10;15H" << "Account Name              : ";
    cout << "\033[12;15H" << "Birthday (DD/MM/YYYY)     : ";
    cout << "\033[14;15H" << "Contact (11 digits)       : ";
    cout << "\033[16;15H" << "Initial Deposit       PHP : ";
    cout << "\033[18;15H" << "Create 4-Digit PIN        : ";
    cout << "\033[20;15H" << "Confirm 4-Digit PIN       : ";
    cout << "\033[22;15H" << "Insert USB & Enter Letter : ";

    auto printError = [](const string& msg) {
        cout << "\033[27;2H"; 
        for(int i=0; i<76; i++) cout << " "; 
        printCentered(27, msg, C_RED);
    };
    auto clearError = []() {
        cout << "\033[27;2H";
        for(int i=0; i<76; i++) cout << " "; 
    };

    string accStr, nameStr, bdayStr, contactStr, depStr, pinStr, confPinStr, driveStr;
    int step = 0;

    while (step >= 0 && step <= 9) 
    {
        switch (step) 
        {
            case 0: // Account Number
                if (!getValidInput(8, 43, accStr, 5, VALID_NUMBERS, false)) return;
                
                if (accStr.length() == 5 && stoi(accStr) > 0) { 
                    step++; clearError(); 
                } else { 
                    printError("Account Number must be exactly 5 digits."); 
                }
                break;

            case 1: // Account Name
                if (!getValidInput(10, 43, nameStr, 50, VALID_NAME, false)) return;
                
                if (nameStr.length() > 2) { 
                    step++; clearError(); 
                } else { 
                    printError("Name is too short."); 
                }
                break;

            case 2: // Birthday (Uses the new Date Input Engine)
                if (!getDateInput(12, 43, bdayStr)) return;
                
                step++; clearError(); 
                break;

            case 3: // Contact
                if (!getValidInput(14, 43, contactStr, 11, VALID_NUMBERS, false)) return;
                
                if (contactStr.length() == 11) { 
                    step++; clearError(); 
                } else { 
                    printError("Contact must be exactly 11 digits (e.g. 09...)"); 
                }
                break;

            case 4: // Initial Deposit
                if (!getValidInput(16, 43, depStr, 10, VALID_DECIMALS, false)) return;
                
                if (depStr.length() > 0 && stof(depStr) >= 5000.0f) { 
                    step++; clearError(); 
                } else { 
                    printError("Minimum initial deposit is PHP 5000."); 
                }
                break;

            case 5: // Create PIN
                if (!getValidInput(18, 43, pinStr, 4, VALID_NUMBERS, true)) return;
                
                if (pinStr.length() == 4) { 
                    step++; clearError(); 
                } else { 
                    printError("PIN must be exactly 4 digits."); 
                }
                break;

            case 6: // Confirm PIN
                if (!getValidInput(20, 43, confPinStr, 4, VALID_NUMBERS, true)) return;
                
                if (pinStr == confPinStr) { 
                    step++; clearError(); 
                } else { 
                    printError("PINs do not match! Try again.");
                    confPinStr.clear();
                    cout << "\033[20;43H    "; 
                }
                break;
            case 7: // USB Drive Letter
                if (!getValidInput(22, 43, driveStr, 1, VALID_DRIVE, false)) return;
                
                if (driveStr.length() == 1) { 
                    driveStr[0] = toupper(driveStr[0]); // Force uppercase
                    cout << "\033[22;43H" << driveStr[0]; // Visually update to uppercase
                    step++; clearError(); 
                } else { 
                    printError("Enter a valid drive letter (e.g. D, E, F)"); 
                }
                break;

        case 8: // Submitting to Backend
            {
                printCentered(27, " Processing transaction... please wait. ", C_YELLOW);

                Account newAcc;
                newAcc.accountNumber = stoi(accStr);
                newAcc.depositBalance = stof(depStr);
                newAcc.pinCode = stoi(pinStr);
                newAcc.isSavings = false;
                newAcc.savingsBalance = 0.0f;
                
                // Copy strings and FORCE the null-terminator at the very end of the array
                strncpy(newAcc.accountName, nameStr.c_str(), sizeof(newAcc.accountName) - 1);
                newAcc.accountName[sizeof(newAcc.accountName) - 1] = '\0';

                strncpy(newAcc.contact, contactStr.c_str(), sizeof(newAcc.contact) - 1);
                newAcc.contact[sizeof(newAcc.contact) - 1] = '\0';

                // Convert 8-digit buffer into standard DD/MM/YYYY format for the struct
                string formattedBday = bdayStr.substr(0,2) + "/" + bdayStr.substr(2,2) + "/" + bdayStr.substr(4,4);
                strncpy(newAcc.birthday, formattedBday.c_str(), sizeof(newAcc.birthday) - 1);
                newAcc.birthday[sizeof(newAcc.birthday) - 1] = '\0';

                // Call the actual ATM backend method
                int status = atm.registerAccount(newAcc, driveStr[0]);

                if (status == 0) {
                    clearInnerScreen();
                    printCentered(14, "Account Created Successfully!", C_GREEN);
                    printCentered(16, "Please keep your USB drive safe.", C_RESET);
                    printCentered(25, "[ Press Enter to return to Menu ]", C_YELLOW);
                    while (getKeyPress() != KEY_ENTER);
                    return; 
                } 
                else if (status == 1) {
                    printError("USB already contains an account! Use a different USB.");
                    step = 7; 
                } 
                else if (status == 2) {
                    printError("Invalid USB Drive! Is it plugged in?");
                    step = 7; 
                } 
                else if (status == 3) {
                    printError("Account Number already taken! Choose a different one.");
                    step = 0; 
                }
                break;
            }
        }
    }
}

void runMainMenu(ATM& atm)
{
    vector<string> mainMenu = {
        "Login",
        "Register Account",
        "Quit Application"
    };

    // The Bank Logo (Notice the \" to safely print double quotes)
    vector<string> logo = {
        "888888b.   888b    888 8888888b.  ",
        "888  \"88b  8888b   888 888   Y88b ",
        "888  .88P  88888b  888 888    888 ",
        "8888888K.  888Y88b 888 888   d88P ",
        "888  \"Y88b 888 Y88b888 8888888P\"  ",
        "888    888 888  Y88888 888 T88b   ",
        "888   d88P 888   Y8888 888  T88b  ",
        "8888888P\"  888    Y888 888   T88b "
    };

    // True Color RGB escape codes (R;G;B) for a Green to Dark Green gradient
    vector<string> gradient = {
        "\033[38;2;0;255;0m",  // Brightest Green
        "\033[38;2;0;225;0m",
        "\033[38;2;0;195;0m",
        "\033[38;2;0;165;0m",
        "\033[38;2;0;135;0m",
        "\033[38;2;0;105;0m",
        "\033[38;2;0;75;0m",   
        "\033[38;2;0;45;0m"    // Darkest Green
    };

    while (true) 
    {
        clearInnerScreen();
        
        // Print the logo line-by-line starting at row 3
        for (size_t i = 0; i < logo.size(); i++) {
            // Apply the corresponding gradient color for each line
            printCentered(3 + i, logo[i], gradient[i]); 
        }
        
        // Run the menu starting at row 14, below the logo
        int choice = runInteractiveMenu(14, "WELCOME TO THE BANK OF R", mainMenu);
        
        if (choice == 1) runLogin(atm);
        else if (choice == 2) runRegister(atm);
        else if (choice == 3 || choice == -1) {
            clearInnerScreen();
            printCentered(14, "Shutting down...", C_RED);
            Sleep(1000);
            break; 
        }
    }
}

/*========== ENTRY POINT ==========*/
int main()
{
    initWindow();
    drawBorder(C_GREEN);

    // Boot up the database and link it to the ATM
    BankDatabase db = BankDatabase();
    db.loadFromFile();
    ATM atm(&db); 

    runMainMenu(atm); // Enter the root menu

    cout << C_RESET << "\033[31;1H"; 
    return 0;
}