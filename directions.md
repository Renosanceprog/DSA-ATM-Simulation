Requirements:

1. Use ADT LIST Operations in Link List implementation Array of Structures (OOP/C++) or Pointer (Link List) Implementation of ADT List
2. 2 Modules
    1. Registration Module – Enrolment to a new ATM Account
        - Assign pin code and save to atm card (account no. and pin)
    2. Transaction Module
        1. Balance Inquiry
        2. Withdraw
        3. Deposit
        4. Fund Transfer (for enrolled accounts only)
        5. Change PIN Code
        6. Other transactions (Optional)
3. Turn in the source code and the video recording for the presentation/demonstration
4. Both partners should turn in the project. (Source Code and Video)
5. Project duration: 4 weeks

Features for every account:

Use Flash Drive as ATM Card

Use PIN Code (Encrypted)

The Pin Code will be Stored in the ATM Card for checking.

The List will also store the PIN Code for cross checking.

LIST INFORMATION

Account Number (5 digits)

Account Name

Birthday

Contact number

Initial Deposit (Min. 5000)

Pin code

PIN CODE: MAX of 6 digits/ 4 digits need to press ENTER key

FLASH DRIVE AS ATM CARD:

Check the pin.code file in card if not present the program will ask to “Please insert card.”

# Gameplan
 # Features
- account setup:
    * Account Number
        * must not be same
        * must be 5 digits
        * integers only
    * Account Name
        * must be atleast 5 characters long
        * no digits
    * Birthday
        * format: MM/DD/YYYY
    * Contact number
        * format: +63 000 000 0000
        * no characters
    * Initial Deposit (Min. 5000)
        * can be float or integers
        * no characters
    * Pin code
        * 4 digits
        * integers only
- account types:
    - deposit account
        - general account where money comes in
    - savings account
        - optional inner account under card holder where you can put money that compounds
- Balance Inquiry
    - asks for deposit or savings account to check balance
“Deposit Balance: 495.95 PHP”
“Savings Balance: 8.95 PHP”
- Withdraw
    - asks for deposit or savings account
    - min of 100 php and max of 10,000 php
- Deposit
    - can only deposit to deposit acc, savings takes money from deposit acc
- fund transfer
    - can only do fund transfers from deposit account
    - transfers money from one card holder to another
- Change pin code
    - *must not be same
    - *must be 4 digits
    - *integers only

# need to research
how to do frontend obfuscation
how to do backend encryption
 how to make program detect usb in pc
