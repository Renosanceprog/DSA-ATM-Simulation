#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>

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

/*========== DUMMY ATM CLASS FOR COMPILATION ==========*/
// DELETE THIS ENTIRE BLOCK WHEN MERGING WITH THE BACKEND
class ATM {
public:
    bool sessionActive = false;
    void logout() { sessionActive = false; }
    // Dummy backend methods
    bool authenticateUser(int acc, int pin) { sessionActive = true; return true; }
};
/*======================================================*/

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
            cursor_pos = (cursor_pos == 1) ? num_options : cursor_pos - 1;
            printMenu(start_row, title, options, cursor_pos); 
        } 
        else if (key == KEY_DOWN) {
            cursor_pos = (cursor_pos == num_options) ? 1 : cursor_pos + 1;
            printMenu(start_row, title, options, cursor_pos); 
        } 
        else if (key == KEY_ENTER) return cursor_pos; 
        else if (key == KEY_ESC) return -1; 
    }
}

/*========== HELPER: TODO SCREEN ==========*/
void showTodoScreen(const string& title)
{
    clearInnerScreen();
    printCentered(12, "--- " + title + " ---", C_CYAN);
    printCentered(14, "= TODO input sanitation and action interface =", C_RED);
    printCentered(16, "Press ESC to return.", C_RESET);
    while (getKeyPress() != KEY_ESC);
}

/*========== MENU STATE FUNCTIONS ==========*/

void runCheckBalance(ATM& atm)
{
    vector<string> options = {"Deposit Account", "Savings Account"};
    while (true) {
        clearInnerScreen();
        int choice = runInteractiveMenu(10, "BALANCE INQUIRY", options);
        if (choice == 1) showTodoScreen("CHECK DEPOSIT BALANCE");
        else if (choice == 2) showTodoScreen("CHECK SAVINGS BALANCE");
        else if (choice == -1) return; // ESC pops stack
    }
}

void runDeposit(ATM& atm)
{
    showTodoScreen("DEPOSIT FUNDS");
}

void runWithdraw(ATM& atm)
{
    vector<string> options = {"From Deposit Account", "From Savings Account"};
    while (true) {
        clearInnerScreen();
        int choice = runInteractiveMenu(10, "WITHDRAW FUNDS", options);
        if (choice == 1) showTodoScreen("WITHDRAW FROM DEPOSIT");
        else if (choice == 2) showTodoScreen("WITHDRAW FROM SAVINGS");
        else if (choice == -1) return; 
    }
}

void runSavingsTransfer(ATM& atm)
{
    vector<string> options = {"To Savings Account", "From Savings Account"};
    while (true) {
        clearInnerScreen();
        int choice = runInteractiveMenu(10, "SAVINGS TRANSFER", options);
        if (choice == 1) showTodoScreen("TRANSFER TO SAVINGS");
        else if (choice == 2) showTodoScreen("TRANSFER FROM SAVINGS");
        else if (choice == -1) return;
    }
}

void runFundTransfer(ATM& atm)
{
    showTodoScreen("FUND TRANSFER");
}

void runChangePin(ATM& atm)
{
    showTodoScreen("CHANGE PIN");
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
    // Temporarily routing straight to hub for skeleton testing.
    // Replace with real input later.
    showTodoScreen("ACCOUNT LOGIN FLOW");
    
    // Simulate successful login
    if (atm.authenticateUser(0, 0)) {
        runMainHub(atm);
    }
}

void runRegister(ATM& atm)
{
    showTodoScreen("ACCOUNT REGISTRATION FLOW");
}

void runMainMenu(ATM& atm)
{
    vector<string> mainMenu = {
        "Login",
        "Register Account",
        "Quit Application"
    };

    while (true) 
    {
        clearInnerScreen();
        printCentered(4, "WELCOME TO THE ATM", C_GREEN);
        
        int choice = runInteractiveMenu(10, "MAIN MENU", mainMenu);
        
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
    drawBorder(C_BLUE);

    ATM atm; // Initialize the state machine
    runMainMenu(atm); // Enter the root menu

    cout << C_RESET << "\033[31;1H"; 
    return 0;
}