// ============================================================
//  Warehouse Automation System
//  Case Study 82 — Console-based C++ Application
//  Author  : Expert C++ Engineer
//  Standard: C++17
// ============================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>

// ============================================================
//  CLASS: Product
//  Represents a single inventory item in the warehouse.
// ============================================================
class Product {
private:
    int         productID;          // Unique numeric identifier
    std::string name;               // Human-readable product name
    int         currentQuantity;    // Units currently in stock
    int         reorderThreshold;   // Alert fires when qty <= this value

public:
    // ── Constructors ──────────────────────────────────────────
    Product()
        : productID(0), name(""), currentQuantity(0), reorderThreshold(0) {}

    Product(int id, const std::string& n, int qty, int threshold)
        : productID(id), name(n), currentQuantity(qty), reorderThreshold(threshold) {}

    // ── Getters ───────────────────────────────────────────────
    int         getProductID()       const { return productID; }
    std::string getName()            const { return name; }
    int         getCurrentQuantity() const { return currentQuantity; }
    int         getReorderThreshold()const { return reorderThreshold; }

    // ── Setters ───────────────────────────────────────────────
    void setProductID(int id)               { productID = id; }
    void setName(const std::string& n)      { name = n; }
    void setCurrentQuantity(int qty)        { currentQuantity = qty; }
    void setReorderThreshold(int threshold) { reorderThreshold = threshold; }

    // ── Convenience mutators ──────────────────────────────────
    void addQuantity(int amount)    { currentQuantity += amount; }
    void removeQuantity(int amount) { currentQuantity -= amount; }

   
    std::string toCSV() const {
        return std::to_string(productID) + "," +
               name                       + "," +
               std::to_string(currentQuantity) + "," +
               std::to_string(reorderThreshold);
    }
};

// ============================================================
//  CLASS: WarehouseSystem
//  Manages the inventory vector, file I/O, and all operations.
// ============================================================
class WarehouseSystem {
private:
    std::vector<Product> inventory;          // Runtime storage
    const std::string    DATA_FILE = "inventory.csv"; // Persistent file

    // ── Internal helper: find product index by ID ─────────────
    int findProductIndex(int id) const {
        for (int i = 0; i < static_cast<int>(inventory.size()); ++i) {
            if (inventory[i].getProductID() == id) return i;
        }
        return -1; // Not found
    }

    // ── Internal helper: generate next available ID ───────────
    int generateNextID() const {
        if (inventory.empty()) return 1001;
        int maxID = 0;
        for (const auto& p : inventory)
            maxID = std::max(maxID, p.getProductID());
        return maxID + 1;
    }

    // ── Internal helper: safe integer input ───────────────────
    int safeIntInput(const std::string& prompt, int minVal = 0) {
        int value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value && value >= minVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  [!] Invalid input. Please enter a whole number"
                      << (minVal > 0 ? " >= " + std::to_string(minVal) : "") << ".\n";
        }
    }

public:
    // ── Constructor: load data on startup ─────────────────────
    WarehouseSystem() { loadFromFile(); }

    // ── Destructor: save data on exit ─────────────────────────
    ~WarehouseSystem() { saveToFile(); }

    // ============================================================
    //  FILE HANDLING: loadFromFile()
    //  Reads inventory.csv at startup. Each line: id,name,qty,threshold
    // ============================================================
    void loadFromFile() {
        std::ifstream inFile(DATA_FILE);
        if (!inFile.is_open()) {
            
            std::cout << "  [i] No data file found. Starting with empty inventory.\n";
            return;
        }

        std::string line;
        int linesLoaded = 0;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue; 

            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> fields;

            while (std::getline(ss, token, ','))
                fields.push_back(token);

            if (fields.size() < 4) continue; 

            try {
                int         id        = std::stoi(fields[0]);
                std::string name      = fields[1];
                int         qty       = std::stoi(fields[2]);
                int         threshold = std::stoi(fields[3]);
                inventory.emplace_back(id, name, qty, threshold);
                ++linesLoaded;
            } catch (...) {
                std::cerr << "  [!] Skipping malformed line: " << line << "\n";
            }
        }
        inFile.close();
        std::cout << "  [i] Loaded " << linesLoaded << " product(s) from " << DATA_FILE << ".\n";
    }

    // ============================================================
    //  FILE HANDLING: saveToFile()
    //  Overwrites inventory.csv with current in-memory state.
    // ============================================================
    void saveToFile() const {
        std::ofstream outFile(DATA_FILE, std::ios::trunc);
        if (!outFile.is_open()) {
            std::cerr << "  [!] ERROR: Could not open " << DATA_FILE << " for writing!\n";
            return;
        }

        outFile << "# Warehouse Inventory Data File\n";
        outFile << "# Format: ProductID,Name,CurrentQuantity,ReorderThreshold\n";

        for (const auto& p : inventory)
            outFile << p.toCSV() << "\n";

        outFile.close();
        std::cout << "\n  [i] Inventory saved to " << DATA_FILE << ".\n";
    }

    // ============================================================
    //  METHOD: addStock()
    //  Adds quantity to an existing item OR creates a new item.
    // ============================================================
    void addStock() {
        std::cout << "\n========================================\n";
        std::cout << "         ADD STOCK\n";
        std::cout << "========================================\n";
        std::cout << "  1. Add to existing product\n";
        std::cout << "  2. Create a new product\n";
        std::cout << "  0. Back to menu\n";
        std::cout << "  Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            // ── Add to existing ──────────────────────────────
            if (inventory.empty()) {
                std::cout << "  [!] Inventory is empty. Please create a new product first.\n";
                return;
            }
            generateReport(); // Show current stock so user knows IDs

            int id  = safeIntInput("\n  Enter Product ID to restock: ", 1);
            int idx = findProductIndex(id);

            if (idx == -1) {
                std::cout << "  [!] Product ID " << id << " not found.\n";
                return;
            }

            int qty = safeIntInput("  Enter quantity to add: ", 1);
            inventory[idx].addQuantity(qty);

            std::cout << "  [✓] Added " << qty << " unit(s) to \""
                      << inventory[idx].getName() << "\". New stock: "
                      << inventory[idx].getCurrentQuantity() << "\n";

        } else if (choice == 2) {
            // ── Create new product ────────────────────────────
            std::cout << "\n  Enter new product name: ";
            std::string name;
            std::getline(std::cin, name);

            if (name.empty()) {
                std::cout << "  [!] Name cannot be empty.\n";
                return;
            }
            // Prevent comma in name (would break CSV)
            if (name.find(',') != std::string::npos) {
                std::cout << "  [!] Product name must not contain commas.\n";
                return;
            }

            int qty       = safeIntInput("  Enter initial quantity   : ", 0);
            int threshold = safeIntInput("  Enter reorder threshold  : ", 1);
            int newID     = generateNextID();

            inventory.emplace_back(newID, name, qty, threshold);

            std::cout << "  [✓] New product created!\n"
                      << "      ID: " << newID << "  |  Name: " << name
                      << "  |  Qty: " << qty   << "  |  Threshold: " << threshold << "\n";

        } else if (choice != 0) {
            std::cout << "  [!] Invalid option.\n";
        }
    }

    // ============================================================
    //  METHOD: removeStock()
    //  Removes quantity from an existing item with validation.
    // ============================================================
    void removeStock() {
        std::cout << "\n========================================\n";
        std::cout << "         REMOVE STOCK\n";
        std::cout << "========================================\n";

        if (inventory.empty()) {
            std::cout << "  [!] Inventory is empty.\n";
            return;
        }

        generateReport(); // Show current stock

        int id  = safeIntInput("\n  Enter Product ID to remove stock from: ", 1);
        int idx = findProductIndex(id);

        if (idx == -1) {
            std::cout << "  [!] Product ID " << id << " not found.\n";
            return;
        }

        int qty = safeIntInput("  Enter quantity to remove: ", 1);

        // ── Validation: Insufficient stock check ─────────────
        if (qty > inventory[idx].getCurrentQuantity()) {
            std::cout << "  [!] INSUFFICIENT STOCK ERROR!\n"
                      << "      Requested: " << qty << " unit(s)  |  "
                      << "Available: " << inventory[idx].getCurrentQuantity() << " unit(s).\n"
                      << "      Operation cancelled.\n";
            return;
        }

        inventory[idx].removeQuantity(qty);

        std::cout << "  [✓] Removed " << qty << " unit(s) from \""
                  << inventory[idx].getName() << "\". Remaining stock: "
                  << inventory[idx].getCurrentQuantity() << "\n";

        // Immediately alert if now below threshold
        if (inventory[idx].getCurrentQuantity() <= inventory[idx].getReorderThreshold()) {
            std::cout << "  ⚠  WARNING: Stock for \"" << inventory[idx].getName()
                      << "\" is now at or below reorder threshold ("
                      << inventory[idx].getReorderThreshold() << ")!\n";
        }
    }

    // ============================================================
    //  METHOD: generateReport()
    //  Prints a neatly aligned table of the entire inventory.
    // ============================================================
    void generateReport() const {
        std::cout << "\n========================================\n";
        std::cout << "         INVENTORY REPORT\n";
        std::cout << "========================================\n";

        if (inventory.empty()) {
            std::cout << "  [i] No products in inventory.\n";
            return;
        }

        // ── Table header ──────────────────────────────────────
        std::cout << std::left
                  << std::setw(8)  << "ID"
                  << std::setw(25) << "Product Name"
                  << std::setw(12) << "Qty"
                  << std::setw(12) << "Threshold"
                  << "Status\n";
        std::cout << std::string(65, '-') << "\n";

        // ── Table rows (loop through vector) ──────────────────
        for (const auto& p : inventory) {
            std::string status = (p.getCurrentQuantity() <= p.getReorderThreshold())
                                 ? "⚠  LOW STOCK"
                                 : "OK";

            std::cout << std::left
                      << std::setw(8)  << p.getProductID()
                      << std::setw(25) << p.getName()
                      << std::setw(12) << p.getCurrentQuantity()
                      << std::setw(12) << p.getReorderThreshold()
                      << status << "\n";
        }

        std::cout << std::string(65, '-') << "\n";
        std::cout << "  Total products: " << inventory.size() << "\n";
    }

    // ============================================================
    //  METHOD: checkReorderAlerts()
    //  Loops through inventory; prints alert when qty <= threshold.
    // ============================================================
    void checkReorderAlerts() const {
        std::cout << "\n========================================\n";
        std::cout << "         REORDER ALERTS\n";
        std::cout << "========================================\n";

        bool alertFound = false;

        for (const auto& p : inventory) {
            // ── Core reorder check ────────────────────────────
            if (p.getCurrentQuantity() <= p.getReorderThreshold()) {
                alertFound = true;
                std::cout << "  ⚠  REORDER ALERT!\n"
                          << "     Product  : " << p.getName()          << "\n"
                          << "     ID       : " << p.getProductID()     << "\n"
                          << "     In Stock : " << p.getCurrentQuantity()<< " unit(s)\n"
                          << "     Threshold: " << p.getReorderThreshold()<< " unit(s)\n"
                          << "     Action   : Please restock immediately!\n"
                          << "     " << std::string(40, '-') << "\n";
            }
        }

        if (!alertFound) {
            std::cout << "  [✓] All products are sufficiently stocked. No alerts.\n";
        }
    }

    // ============================================================
    //  METHOD: runMenu()
    //  Main while-loop + switch-statement console menu.
    // ============================================================
    void runMenu() {
        int choice = -1;

        while (choice != 5) {
            std::cout << "\n╔══════════════════════════════════════╗\n";
            std::cout << "║   WAREHOUSE AUTOMATION SYSTEM  v1.0  ║\n";
            std::cout << "╠══════════════════════════════════════╣\n";
            std::cout << "║  1. Add Stock                        ║\n";
            std::cout << "║  2. Remove Stock                     ║\n";
            std::cout << "║  3. View Inventory Report            ║\n";
            std::cout << "║  4. View Reorder Alerts              ║\n";
            std::cout << "║  5. Exit & Save                      ║\n";
            std::cout << "╚══════════════════════════════════════╝\n";
            std::cout << "  Enter choice (1-5): ";

            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "  [!] Please enter a number between 1 and 5.\n";
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            switch (choice) {
                case 1: addStock();          break;
                case 2: removeStock();       break;
                case 3: generateReport();    break;
                case 4: checkReorderAlerts();break;
                case 5:
                    std::cout << "\n  Saving data and exiting. Goodbye!\n";
                    break;
                default:
                    std::cout << "  [!] Invalid choice. Please enter 1–5.\n";
            }
        }
    }
};

// ============================================================
//  ENTRY POINT
// ============================================================
int main() {
    std::cout << "========================================\n";
    std::cout << "   WAREHOUSE AUTOMATION SYSTEM — INIT  \n";
    std::cout << "========================================\n";

    WarehouseSystem warehouse; // Constructor auto-loads data from file
    warehouse.runMenu();       // Run until user selects Exit
    // Destructor auto-saves data to file on scope exit

    return 0;
}
