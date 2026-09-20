#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>

using namespace std;

/*========== 1. TEXT COLORS (ANSI codes) ==========*/
#define C_RESET  "\033[0m"      
#define C_GREEN  "\033[1;32m"   
#define C_YELLOW "\033[1;33m"  
#define C_CYAN   "\033[1;36m"   
#define C_RED    "\033[1;31m"    
#define C_BLUE   "\033[1;34m"   
#define C_MAG    "\033[1;35m"    
#define C_INVERT "\033[1;30m\033[47m" 

/*========== 2. UI NAV ENUMS (Key Codes) ==========*/
#define KEY_UP 72       
#define KEY_DOWN 80     
#define KEY_ENTER 13    
#define KEY_ESC 27      

/*========== 3. INIT WINDOW ==========*/
void initWindow()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    
    // Enable support for ANSI color codes
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // Lock window size at 80x30
    SMALL_RECT windowSize = {0, 0, 79, 29};
    SetConsoleWindowInfo(hOut, TRUE, &windowSize);
    
    COORD bufferSize = {80, 30};
    SetConsoleScreenBufferSize(hOut, bufferSize);
    
    // Prevent window resizing
    HWND consoleWindow = GetConsoleWindow();
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
    
    // Hide the blinking console cursor for cleaner UI
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hOut, &cursorInfo);
}

/*========== 4. CREATE BORDER ==========*/
void drawBorder(const string& color)
{
    // Clear screen atomically once before drawing
    cout << "\033[2J"; 
    
    // Set color and draw top corners and line
    cout << color;
    cout << "\033[1;1H\xC9";
    for (int i = 0; i < 78; i++) cout << "\xCD";
    cout << "\xBB";
    
    // Draw left and right sides
    for (int i = 2; i < 30; i++)
    {
        cout << "\033[" << i << ";1H\xBA\033[" << i << ";80H\xBA";
    }
    
    // Draw bottom corners and line
    cout << "\033[30;1H\xC8";
    for (int i = 0; i < 78; i++) cout << "\xCD";
    cout << "\xBC";
    
    // Reset cursor to a safe spot inside the border
    cout << C_RESET << "\033[2;3H";
}

/*========== 5. PRINT CENTERED ==========*/
void printCentered(int row, const string& text, const string& color)
{
    // Calculate center column position for an 80-char wide screen
    int len = text.length();
    int col = (80 - len) / 2;
    
    // Safety check for long strings touching the border
    if (col < 2) col = 2; 
    
    cout << "\033[" << row << ";" << col << "H" << color << text << C_RESET;
}

// Helper function to clear the inside of the border to prevent screen tearing
void clearInnerScreen() 
{
    for (int i = 2; i < 29; i++) {
        cout << "\033[" << i << ";2H";
        for (int j = 0; j < 78; j++) cout << " ";
    }
}

/*========== 6. UNIVERSAL MENU SYSTEM ==========*/
int getKeyPress() 
{
    int ch = _getch();
    // Handle arrow keys (which send two bytes: 0 or 224, then the key code)
    if (ch == 0 || ch == 224) { 
        ch = _getch();
        return ch;
    }
    return ch; 
} 

void printMenu(int start_row, const string& title, const vector<string>& options, int cursor_pos) 
{
    printCentered(start_row, title, C_YELLOW);
    
    // Find the longest option text to perfectly center the block
    int max_len = 0;
    for (const auto& opt : options) {
        if (opt.length() > max_len) max_len = opt.length();
    }

    int block_start_col = ((80 - (max_len + 8)) / 2); // +8 for the "  [ > ] " prefix space

    for (size_t i = 0; i < options.size(); i++) 
    {
        // Move cursor to the correct row/col
        cout << "\033[" << start_row + 2 + i << ";" << block_start_col << "H";
        
        if (cursor_pos == i + 1) {
            // Highlighted
            cout << C_CYAN << "  [ > ] " << options[i];
            // Pad spaces to overwrite previous cursor traces
            for(int p = 0; p < (max_len - options[i].length()); p++) cout << " "; 
            cout << "  " << C_RESET;
        } else {
            // Normal
            cout << "        " << options[i];
            for(int p = 0; p < (max_len - options[i].length()); p++) cout << " "; 
            cout << "  ";
        }
    }
}

// Returns the 1-based index of the chosen option, or -1 if ESC is pressed
int runInteractiveMenu(int start_row, const string& title, const vector<string>& options) 
{
    int cursor_pos = 1; 
    int num_options = options.size();
    int key;

    // Print initial state
    printMenu(start_row, title, options, cursor_pos); 

    while (true) 
    { 
        key = getKeyPress();
        
        if (key == KEY_UP) {
            cursor_pos--;
            if (cursor_pos < 1) cursor_pos = num_options;
            printMenu(start_row, title, options, cursor_pos); 
        } 
        else if (key == KEY_DOWN) {
            cursor_pos++;
            if (cursor_pos > num_options) cursor_pos = 1;
            printMenu(start_row, title, options, cursor_pos); 
        } 
        else if (key == KEY_ENTER) {
            return cursor_pos; // User selected an option
        }
        else if (key == KEY_ESC) {
            return -1; // User cancelled
        }
    }
}

/*========== SKELETON TEST (MAIN) ==========*/
int main()
{
    initWindow();
    drawBorder(C_BLUE);

    vector<string> mainMenu = {
        "Login",
        "Register Account",
        "Quit Application"
    };

    while (true) 
    {
        clearInnerScreen();
        printCentered(4, "WELCOME TO THE ATM", C_GREEN);
        
        int choice = runInteractiveMenu(7, "MAIN MENU", mainMenu);
        
        if (choice == 3 || choice == -1) {
            clearInnerScreen();
            printCentered(14, "Shutting down...", C_RED);
            Sleep(1000);
            break; 
        }
        else if (choice == 1) {
            // Placeholder for Login
            clearInnerScreen();
            printCentered(14, "--- LOGIN SCREEN REACHED ---", C_YELLOW);
            printCentered(16, "Press ESC to return.", C_RESET);
            while (getKeyPress() != KEY_ESC); // Wait for ESC
        }
        else if (choice == 2) {
            // Placeholder for Register
            clearInnerScreen();
            printCentered(14, "--- REGISTRATION SCREEN REACHED ---", C_CYAN);
            printCentered(16, "Press ESC to return.", C_RESET);
            while (getKeyPress() != KEY_ESC); // Wait for ESC
        }
    }

    // Restore terminal colors before closing
    cout << C_RESET << "\033[31;1H"; 
    return 0;
}