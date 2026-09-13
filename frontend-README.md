# ATM Frontend UI/UX & Integration Guidelines

## 1. Architectural Philosophy
The backend (database, file I/O, encryption, and transaction math) is fully complete. The frontend is exclusively responsible for **View** (MS-DOS style rendering), **Controller** (menu routing), and **Data Sanitation** (strictly validating user input *before* it touches the backend). 

**Strict Technical Constraints:**
* **No External GUI/TUI Libraries:** Do not use `<curses.h>`, `<ncurses.h>`, or any third-party windowing systems. 
* **First-Principles Rendering:** Use standard `<iostream>`, `<iomanip>`, and system calls (`system("cls")` or ANSI escape codes) to clear the screen and draw ASCII boxes.
* **Synchronous Flow:** The UI should be a continuous loop (`while(true)`) that draws a screen, waits for input, processes it, and redraws.
* **Responsive Input (Obfuscation):** Standard `std::cin >>` is banned for critical inputs to avoid buffer overflow and visible PINs. All user input must be built manually using `<conio.h>` `_getch()` to process keystrokes one by one.

---

## 2. The Input Buffer System (Crucial)

To achieve MS-DOS terminal responsiveness and PIN masking (obfuscation), you must implement a custom string-building loop using `_getch()`. 

**Implementation Requirements for Input Helper:**
1. **Listen:** Loop `_getch()` to catch keystrokes instantly.
2. **Backspace Handling:** If the user presses Backspace (`\b` or `8`), remove the last character from the string, and print `\b \b` to the console to visually erase the character.
3. **Submit:** If the user presses Enter (`\r` or `13`), break the loop and return the accumulated string.
4. **Masking:** If the input is a PIN, `cout << '*'` instead of the actual character. Otherwise, echo the character they typed.

---

## 3. Data Sanitation Rules (Frontend Responsibility)

The backend expects perfectly formatted data. If the user inputs garbage, the frontend must reject it with an error message and make them type it again. Do not pass invalid data to the `ATM` class.

**Account Setup Restrictions:**
* **Account Number:** Exactly 5 digits. Must be strictly numeric (`isdigit`).
* **Account Name:** Minimum 5 characters long. No numbers allowed (check `!isdigit` on all chars).
* **Birthday:** Must exactly match length and format `MM/DD/YYYY`.
* **Contact Number:** Must exactly match format `+63 XXX XXX XXXX` (Total length 16 chars).
* **Initial Deposit:** Float or integer. Must be `>= 5000`. No alphabet characters.
* **PIN Code:** Exactly 4 digits. Numeric only. 
* **Drive Letter (USB):** Must be a single uppercase char between 'D' and 'Z'.

---

## 4. UI/UX Style Guide (MS-DOS Aesthetic)

Screens should be contained in neat ASCII frames. Keep text centered or neatly aligned. Clear the screen between state changes.

**Example Main Menu Look:**
```text
==================================================
              BANK OF DOS - TERMINAL              
==================================================

    [1] LOGIN TO ACCOUNT
    [2] REGISTER NEW ACCOUNT
    [3] EXIT

==================================================
Select an option: _