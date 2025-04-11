/**
 * @file main.cpp
 * @brief Main file for the Secure Authentication System.
 * @details This file contains the implementation of a secure authentication system
 *          that allows users to register, log in, and manage their accounts.
 */

 #include <iostream>
 #include <string>
 #include <cctype>
 #include <random>
 #include <iomanip>
 #include <ctime>
 #include <cstdlib>
 #include <sstream>
 #include <sodium.h>
 #include <fstream>
 #include <map>
 #include <limits>
 #include <thread>
 #include <chrono>
 #include <conio.h>
 
 using namespace std;
 
 /**
  * @brief Securely gets a password input from the user without displaying it in plain text.
  * @param prompt The prompt message to display to the user.
  * @return The password entered by the user.
  */
 string getPasswordFromUser(const string& prompt) {
     string password;
     cout << prompt;
 
     char ch;
     // Read characters one by one without echoing them on the console
     while ((ch = _getch()) != '\r') {  // '\r' (Enter key) terminates input
         if (ch == '\b') {  // Handle backspace key
             if (!password.empty()) {  // Only delete if password is not empty
                 password.pop_back();  // Remove last character from password
                 cout << "\b \b";  // Move cursor back, erase character, move back again
             }
         } else {  // Any other character (except Enter)
             password.push_back(ch);  // Store character in password string
             cout << '*';  // Print '*' to mask input
         }
     }
     cout << endl;  // Move to a new line after password entry
     return password;  // Return the entered password
 }
 
 /**
  * @namespace TerminalColors
  * @brief Contains ANSI escape sequences for terminal text formatting.
  */
 namespace TerminalColors {
     const string Reset = "\033[0m";  ///< Resets text color and style settings
     const string Red = "\033[31m";   ///< Red text color
     const string Green = "\033[32m"; ///< Green text color
     const string Yellow = "\033[33m"; ///< Yellow text color
     const string Blue = "\033[34m";  ///< Blue text color
     const string Magenta = "\033[35m"; ///< Magenta text color
     const string Cyan = "\033[36m";  ///< Cyan text color
     const string Bold = "\033[1m";   ///< Bold text style
 }
 
 /**
  * @class Terminal
  * @brief Provides static methods to interact with the terminal.
  */
 class Terminal {
 public:
     /**
      * @brief Clears the terminal screen.
      */
     static void clearScreen() {
         system("cls");
     }
 
     /**
      * @brief Prints a header with the title centered and surrounded by a border.
      * @param title The title to display in the header.
      */
     static void printHeader(const string& title) {
         int width = 60;  // Set the width for the header
         string line(width, '=');  // Create a line of '=' characters
         clearScreen();  // Clear the screen before printing the header
         
         // Print the header with title centered and color formatting
         cout << TerminalColors::Bold << TerminalColors::Blue
              << line << endl
              << setw((width + title.length()) / 2) << title << endl
              << line << endl
              << TerminalColors::Reset << endl;
     }
 
     /**
      * @brief Prints a success message in green.
      * @param message The success message to display.
      */
     static void printSuccess(const string& message) {
         cout << TerminalColors::Green << message << TerminalColors::Reset << endl;
     }
 
     /**
      * @brief Prints an error message in red.
      * @param message The error message to display.
      */
     static void printError(const string& message) {
         cout << TerminalColors::Red << message << TerminalColors::Reset << endl;
     }
 
     /**
      * @brief Prints a warning message in yellow.
      * @param message The warning message to display.
      */
     static void printWarning(const string& message) {
         cout << TerminalColors::Yellow << message << TerminalColors::Reset << endl;
     }
 
     /**
      * @brief Prints an informational message in cyan.
      * @param message The informational message to display.
      */
     static void printInfo(const string& message) {
         cout << TerminalColors::Cyan << message << TerminalColors::Reset << endl;
     }
 
     /**
      * @brief Prints a progress bar with the current progress and total steps.
      * @param progress The current progress.
      * @param total The total steps.
      */
     static void printProgressBar(int progress, int total) {
         const int barWidth = 50;  // Width of the progress bar
         float percentage = (float)progress / total;  // Calculate progress percentage
         int pos = barWidth * percentage;  // Determine how much of the bar is filled
         cout << "[" << string(pos, '=') << ">" 
              << string(barWidth - pos, ' ') << "] " 
              << int(percentage * 100.0) << " %" << endl;
     }
 
     /**
      * @brief Waits for the user to press Enter before continuing.
      */
     static void waitForEnter() {
         cout << endl << TerminalColors::Bold << "Press ENTER to continue..." << TerminalColors::Reset;
         cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Wait for the Enter key
     }
 
     /**
      * @brief Displays a loading message with dots, simulating a loading process.
      * @param message The loading message to display.
      * @param milliseconds The duration of the loading animation.
      */
     static void loading(const string& message, int milliseconds = 1500) {
         cout << message;
         cout.flush();  // Ensure the message is printed immediately
         for (int i = 0; i < 3; i++) {
             this_thread::sleep_for(chrono::milliseconds(milliseconds / 3));  // Wait for a short period
             cout << ".";  // Print a dot to simulate loading
             cout.flush();  // Ensure the dot is printed immediately
         }
         cout << endl;
     }
 };
 
 /**
  * @class PasswordStrengthChecker
  * @brief Abstract class defining methods to check and calculate password strength.
  */
 class PasswordStrengthChecker {
 public:
     /**
      * @brief Checks if the password meets strength criteria.
      * @param pw The password to check.
      * @return true if the password is strong, false otherwise.
      */
     virtual bool checkStrength(const string& pw) = 0;
 
     /**
      * @brief Calculates the password's strength as a percentage.
      * @param pw The password to evaluate.
      * @return An integer from 0 to 100 representing the strength percentage.
      */
     virtual int calculateStrengthPercentage(const string& pw) = 0;
 
     /**
      * @brief Virtual destructor.
      */
     virtual ~PasswordStrengthChecker() = default;
 };
 
 /**
  * @class StandardPasswordChecker
  * @brief Implements password strength checks with rules for length, character types, and calculates strength percentage.
  */
 class StandardPasswordChecker : public PasswordStrengthChecker {
 public:
     /**
      * @brief Checks if the password meets basic strength criteria.
      * @param pw The password to check.
      * @return true if the password is valid, false otherwise.
      */
     bool checkStrength(const string& pw) override {
         // Ensure the password is at least 12 characters long
         if (pw.length() < 12) {
             Terminal::printError("Password length must be at least 12 characters!");
             return false;
         }
 
         bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
 
         // Check each character in the password for specific criteria
         for (char c : pw) {
             if (islower(c)) hasLower = true;
             else if (isupper(c)) hasUpper = true;
             else if (isdigit(c)) hasDigit = true;
             else hasSpecial = true;
         }
 
         // Print error messages if any of the required components are missing
         if (!hasLower) Terminal::printError("Missing lowercase character");
         if (!hasUpper) Terminal::printError("Missing uppercase character");
         if (!hasDigit) Terminal::printError("Missing digit");
         if (!hasSpecial) Terminal::printError("Missing special character");
 
         // If all the required components are present, the password is valid
         if (hasLower && hasUpper && hasDigit && hasSpecial) {
             Terminal::printSuccess("Valid password!");
             return true;
         }
         return false;
     }
 
     /**
      * @brief Calculates the password's strength as a percentage.
      * @param pw The password to evaluate.
      * @return An integer from 0 to 100 representing the strength percentage.
      */
     int calculateStrengthPercentage(const string& pw) override {
         int score = min(40, (int)(pw.length() * 3.33));  // Calculate score based on password length
 
         bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
 
         // Check each character in the password for specific criteria
         for (char c : pw) {
             if (islower(c)) hasLower = true;
             else if (isupper(c)) hasUpper = true;
             else if (isdigit(c)) hasDigit = true;
             else hasSpecial = true;
         }
 
         // Add points for each of the components present in the password
         if (hasLower) score += 15;
         if (hasUpper) score += 15;
         if (hasDigit) score += 15;
         if (hasSpecial) score += 15;
 
         return min(100, score);  // Ensure the score does not exceed 100
     }
 };
 
 /**
  * @class User
  * @brief Represents a user with username and password, and handles password validation and account creation.
  */
 class User {
 private:
     string username;  ///< Stores the username of the user
     string password;  ///< Stores the password of the user
     PasswordStrengthChecker* strengthChecker;  ///< Pointer to an object that checks password strength
 
 public:
     /**
      * @brief Constructor: Initializes the strengthChecker with an instance of StandardPasswordChecker.
      */
     User() : strengthChecker(new StandardPasswordChecker()) {}
 
     /**
      * @brief Sets the password after checking its strength.
      * @param pw The password to set.
      * @return true if the password is valid and set successfully, false otherwise.
      */
     bool setPassword(const string& pw) {
         if (!strengthChecker->checkStrength(pw)) return false;
 
         int strength = strengthChecker->calculateStrengthPercentage(pw);
         cout << "Password strength: ";
         Terminal::printProgressBar(strength, 100);
 
         if (strength < 70) 
             Terminal::printWarning("Password could be stronger");
         else if (strength >= 90) 
             Terminal::printSuccess("Excellent password!");
 
         string confirm = getPasswordFromUser("Confirm password: ");
         if (pw != confirm) {
             Terminal::printError("Passwords don't match");
             return false;
         }
 
         password = pw;
         return true;
     }
 
     /**
      * @brief Sets the username.
      * @param name The username to set.
      */
     void setUsername(const string& name) { username = name; }
 
     /**
      * @brief Gets the username.
      * @return The username.
      */
     string getUsername() const { return username; }
 
     /**
      * @brief Gets the password.
      * @return The password.
      */
     string getPassword() const { return password; }
 
     /**
      * @brief Destructor: Deletes the strengthChecker object to free up memory.
      */
     ~User() { delete strengthChecker; }
 };
 
 /**
  * @class PasswordHasher
  * @brief Provides static methods for hashing passwords using the Libsodium library.
  */
 class PasswordHasher {
 public:
     /**
      * @brief Initializes the libsodium library for cryptographic operations.
      * @return true if initialization is successful, false otherwise.
      */
     static bool initialize() {
         return sodium_init() >= 0 ? true : (Terminal::printError("Libsodium init failed"), false);
     }
 
     /**
      * @brief Generates a random salt for password hashing.
      * @return The salt as a hexadecimal string.
      */
     static string generateSalt() {
         unsigned char salt[crypto_pwhash_SALTBYTES];
         randombytes_buf(salt, sizeof salt);
 
         char hex[2 * crypto_pwhash_SALTBYTES + 1];
         for (size_t i = 0; i < sizeof salt; i++)
             snprintf(&hex[i*2], 3, "%02x", salt[i]);
         return string(hex);
     }
 
     /**
      * @brief Hashes the password using the given salt.
      * @param password The password to hash.
      * @param salt_hex The salt in hexadecimal format.
      * @return The hashed password as a hexadecimal string.
      */
     static string hashPassword(const string& password, const string& salt_hex) {
         Terminal::loading("Securely hashing password");
 
         unsigned char salt[crypto_pwhash_SALTBYTES];
         for (size_t i = 0; i < crypto_pwhash_SALTBYTES; i++)
             salt[i] = stoi(salt_hex.substr(i*2, 2), nullptr, 16);
 
         unsigned char hash[32];
         if (crypto_pwhash(hash, sizeof hash, password.c_str(), password.size(),
                           salt, crypto_pwhash_OPSLIMIT_MODERATE, 
                           crypto_pwhash_MEMLIMIT_MODERATE, crypto_pwhash_ALG_DEFAULT) != 0)
             throw runtime_error("Hashing failed");
 
         char hex[65];
         for (size_t i = 0; i < sizeof hash; i++)
             snprintf(&hex[i*2], 3, "%02x", hash[i]);
         return string(hex);
     }
 
     /**
      * @brief Verifies if the given password matches the stored hash.
      * @param password The password to verify.
      * @param hash The stored hash.
      * @param salt The salt used for hashing.
      * @return true if the password matches the hash, false otherwise.
      */
     static bool verifyPassword(const string& password, const string& hash, const string& salt) {
         try {
             return hash == hashPassword(password, salt);
         } catch (...) {
             return false;
         }
     }
 };
 
 /**
  * @class Database
  * @brief Provides static methods to interact with a database of users.
  */
 class Database {
 private:
     static map<string, pair<string, string>> users;  ///< A map to store users and their credentials (hash, salt)
     static const string filename;  ///< The filename to save/load user data
 
 public:
     /**
      * @brief Loads user data from a file into the 'users' map.
      */
     static void loadUsers() {
         ifstream file(filename);
         if (!file) return;
 
         string line;
         while (getline(file, line)) {
             size_t pos1 = line.find(','), pos2 = line.find(',', pos1 + 1);
             if (pos1 == string::npos || pos2 == string::npos) continue;
 
             users[line.substr(0, pos1)] = make_pair(
                 line.substr(pos1 + 1, pos2 - pos1 - 1),
                 line.substr(pos2 + 1)
             );
         }
     }
 
     /**
      * @brief Saves the current users data from the 'users' map to the file.
      */
     static void saveUsers() {
         ofstream file(filename);
         for (const auto& [user, data] : users)
             file << user << "," << data.first << "," << data.second << endl;
     }
 
     /**
      * @brief Checks if a user exists in the 'users' map.
      * @param username The username to check.
      * @return true if the user exists, false otherwise.
      */
     static bool userExists(const string& username) {
         return users.find(username) != users.end();
     }
 
     /**
      * @brief Adds a new user to the 'users' map with the provided credentials.
      * @param username The username of the new user.
      * @param hash The hashed password.
      * @param salt The salt used for hashing.
      * @return true if the user is added successfully, false otherwise.
      */
     static bool addUser(const string& username, const string& hash, const string& salt) {
         if (userExists(username)) return false;
         users[username] = {hash, salt};
         saveUsers();
         return true;
     }
 
     /**
      * @brief Retrieves the hash and salt for a given username.
      * @param username The username to retrieve credentials for.
      * @param hash The hash to store.
      * @param salt The salt to store.
      * @return true if credentials are found, false otherwise.
      */
     static bool getCredentials(const string& username, string& hash, string& salt) {
         auto it = users.find(username);
         if (it == users.end()) return false;
         hash = it->second.first;
         salt = it->second.second;
         return true;
     }
 
     /**
      * @brief Returns the number of users in the 'users' map.
      * @return The number of users.
      */
     static int userCount() { return users.size(); }
 };
 
 map<string, pair<string, string>> Database::users;  ///< Static member variable that holds user data
 const string Database::filename = "users.txt";  ///< Static constant string for the filename to store user data
 
 /**
  * @brief Displays the login screen and verifies user credentials.
  */
 void loginScreen() {
     Terminal::printHeader("User Login");
     string username, password;
     cout << "Username: ";
     getline(cin, username);
     if (username.empty()) {
         Terminal::printError("Username required");
         Terminal::waitForEnter();
         return;
     }
 
     password = getPasswordFromUser("Password: ");
     string storedHash, storedSalt;
     if (!Database::getCredentials(username, storedHash, storedSalt)) {
         Terminal::printError("User not found");
         Terminal::waitForEnter();
         return;
     }
 
     if (PasswordHasher::verifyPassword(password, storedHash, storedSalt)) {
         Terminal::printSuccess("Login successful!");
         cout << TerminalColors::Magenta << "\nWelcome to your secure account, " 
              << username << "!" << TerminalColors::Reset << endl;
     } else {
         Terminal::printError("Invalid credentials");
     }
     Terminal::waitForEnter();
 }
 
 /**
  * @brief Displays the registration screen and prompts the user to create a new account.
  */
 void registrationScreen() {
     Terminal::printHeader("New Account Registration");
 
     User newUser;
     string username;
     cout << "Username: ";
     getline(cin, username);
     if (username.empty()) {
         Terminal::printError("Username required");
         Terminal::waitForEnter();
         return;
     }
 
     for (char c : username) {
         if (!isalnum(c)) {
             Terminal::printError("Only alphanumeric characters allowed");
             Terminal::waitForEnter();
             return;
         }
     }
 
     if (Database::userExists(username)) {
         Terminal::printError("Username already taken");
         Terminal::waitForEnter();
         return;
     }
 
     newUser.setUsername(username);
     Terminal::printInfo("Password Requirements:\n"
         "- Minimum 12 characters\n"
         "- At least 1 uppercase, 1 lowercase\n"
         "- At least 1 digit and 1 special character");
 
     bool passwordSet = false;
     while (!passwordSet) {
         string pw = getPasswordFromUser("Enter password: ");
         if (pw.empty()) {
             Terminal::printError("Password cannot be empty");
             continue;
         }
         passwordSet = newUser.setPassword(pw);
     }
 
     string salt = PasswordHasher::generateSalt();
     string hash = PasswordHasher::hashPassword(newUser.getPassword(), salt);
     if (Database::addUser(username, hash, salt)) {
         Terminal::printSuccess("Account created successfully!");
     } else {
         Terminal::printError("Account creation failed");
     }
     Terminal::waitForEnter();
 }
 
 /**
  * @brief Main function to run the authentication system.
  * @return 0 on successful execution.
  */
 int main() {
     if (!PasswordHasher::initialize()) return 1;
     Database::loadUsers();
 
     while (true) {
         Terminal::printHeader("Secure Authentication System");
         cout << TerminalColors::Bold << "[Main Menu]\n" << TerminalColors::Reset
              << "Registered users: " << Database::userCount() << "\n\n"
              << "1. Login\n2. Register\n3. Exit\n\nChoice (1-3): ";
 
         int choice;
         if (!(cin >> choice)) {
             cin.clear();
             cin.ignore(numeric_limits<streamsize>::max(), '\n');
             Terminal::printError("Invalid input");
             this_thread::sleep_for(chrono::seconds(1));
             continue;
         }
         cin.ignore();
 
         switch (choice) {
             case 1:
                 loginScreen();
                 break;
             case 2:
                 registrationScreen();
                 break;
             case 3:
                 Terminal::printSuccess("Goodbye!");
                 return 0;
             default:
                 Terminal::printError("Invalid choice");
                 this_thread::sleep_for(chrono::seconds(1));
         }
     }
 }
