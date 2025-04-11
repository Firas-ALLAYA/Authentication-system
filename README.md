# Secure Authentication System

This project is a **secure authentication system** implemented in C++. It allows users to register, log in, and manage their accounts securely. The system uses modern cryptographic techniques, including the **Argon2** password hashing algorithm provided by the **Libsodium** library, to ensure the security of user credentials.

---

## Features

- **User Registration**:
  - Users can create an account by providing a username and a password.
  - Passwords are validated for strength (minimum 12 characters, including uppercase, lowercase, digits, and special characters).
  - Passwords are securely hashed using the **Argon2** algorithm.

- **User Login**:
  - Users can log in using their username and password.
  - The system verifies the password against the stored hash.

- **Password Hashing**:
  - Uses the **Libsodium** library for secure password hashing with **Argon2**.
  - Each password is hashed with a unique salt for added security.

- **Terminal Interface**:
  - Provides a user-friendly command-line interface with colored output and progress bars.

- **Database**:
  - User credentials (username, password hash, and salt) are stored in a local file (`users.txt`).

---

## Requirements

- **Libsodium**: The project uses the Libsodium library for cryptographic operations. Make sure Libsodium is installed on your system.
  - Installation guide: [Libsodium Installation](https://doc.libsodium.org/installation)

- **C++ Compiler**: A C++ compiler that supports C++11 or later (e.g., `g++`, `clang++`).

---

## How to Use

1. **Clone the Repository**:
   ```bash
   git clone https://github.com/torinomendez/Authentication-system.git
   cd Authentication-system
   
2. **Install Libsodium**:
  Follow the installation instructions for your operating system: Libsodium Installation Guide.

4. **Compile the Project**:
   Use a C++ compiler to compile the project: g++ -std=c++11 -o final.cpp -lsodium

5. **Run the Program**:
   Execute the compiled binary:./final
   
---

## Project Structure

  main.cpp: The main file containing the program logic.

  Terminal Class: Handles terminal interactions (e.g., printing headers, progress bars, colored messages).

  User Class: Represents a user with a username and password.

  PasswordStrengthChecker Class: Validates password strength.

  PasswordHasher Class: Handles password hashing using Libsodium.

  Database Class: Manages user data storage and retrieval.

---

## Diagrams

  The project includes automatically generated diagrams created using Doxygen. These diagrams provide a visual representation of the code structure, including class relationships and function call graphs.
  
  1.**How to Generate Diagrams**
  
  Install Doxygen and Graphviz:
    Doxygen: Doxygen Installation
    Graphviz: Graphviz Installation

  2.**Generate Documentation**:

  Run the following command in the project directory: doxygen Doxyfile
  
  3.**View Diagrams**:
    Open the generated index.html file in the docs/html directory to view the diagrams.

---

## Dependencies

**Libsodium**: For cryptographic operations (password hashing with Argon2).

.



