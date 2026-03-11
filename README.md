# 📦 Warehouse Automation System (Case Study 82)

A robust, console-based inventory management system built entirely in **C++**. This project digitizes manual warehouse tracking by utilizing Object-Oriented Programming (OOP) principles and persistent file handling to ensure accurate stock management.

---

## 📖 About the Project

**Problem Statement:** Manual warehouse management leads to stock mismatch. This system automates warehouse inventory control.

In traditional warehouse setups, human error during manual stock entry and exit often results in severe discrepancies between recorded paper inventory and actual physical stock. This individual project solves that logistical challenge by providing a secure, automated digital ledger. It actively validates stock removals to prevent negative inventory and persistently saves data to a local CSV file, ensuring zero data loss between sessions.

---

## ✨ Key Features

* **Dynamic Stock Entry:** Easily add stock to existing items or generate unique IDs for entirely new products.
* **Validated Stock Removal:** Engineered conditional logic actively blocks users from removing more stock than is physically available, completely preventing negative stock mismatch.
* **Automated Reorder Alerts:** The system continuously monitors stock levels and flags items falling below their designated safety thresholds with a `⚠ LOW` warning.
* **Data Persistence (File I/O):** Utilizes the C++ `<fstream>` library to automatically load data from and save data to `inventory.csv`. 
* **Formatted Reporting:** Generates clean, aligned tabular inventory reports directly in the console using `<iomanip>`.



---
Out put 


choice 2: 

<img width="645" height="693" alt="Screenshot 2026-03-11 at 5 13 55 PM" src="https://github.com/user-attachments/assets/d43387a3-cf17-4b64-a6b1-663312afc163" />

choice 3: 

<img width="676" height="685" alt="Screenshot 2026-03-11 at 5 11 32 PM" src="https://github.com/user-attachments/assets/243fe328-d41d-4cf9-9667-473089b9c085" />


## 🛠️ Technology Stack

* **Language:** C++ (Standard C++11 / C++17)
* **Core Libraries:** `<iostream>`, `<fstream>`, `<vector>`, `<sstream>`, `<iomanip>`
* **Concepts Applied:** Object-Oriented Programming (Encapsulation, Abstraction), Data Structures (Dynamic Vectors), and File Handling.



---


