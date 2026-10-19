
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <Windows.h>
#include <commdlg.h>

#pragma comment(lib, "Comdlg32.lib")

using namespace std;

class GroceryTracker {
private:
    map<string, int> frequencies;
    map<string, string> displayNames;

    string normalize(const string& item) const {
        string result = item;

        transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) {
                return static_cast<char>(tolower(c));
            });

        return result;
    }

public:
    bool loadItems(const string& filename) {
        ifstream inputFile(filename);

        if (!inputFile.is_open()) {
            cerr << "\nERROR: Could not open the selected file:\n"
                << filename << '\n';
            return false;
        }

        string item;
        int recordsRead = 0;

        while (getline(inputFile, item)) {
            size_t first = item.find_first_not_of(" \t\r\n");

            if (first == string::npos) {
                continue;
            }

            size_t last = item.find_last_not_of(" \t\r\n");
            item = item.substr(first, last - first + 1);

            string key = normalize(item);

            if (frequencies.find(key) == frequencies.end()) {
                displayNames[key] = item;
                frequencies[key] = 1;
            }
            else {
                frequencies[key]++;
            }

            recordsRead++;
        }

        if (inputFile.bad()) {
            cerr << "ERROR: A problem occurred while reading the file.\n";
            return false;
        }

        cout << "\nInput file opened successfully.\n";
        cout << "File: " << filename << '\n';
        cout << "Grocery records read: " << recordsRead << '\n';

        return true;
    }

    bool isEmpty() const {
        return frequencies.empty();
    }

    bool createBackup(const string& filename) const {
        ofstream outputFile(filename);

        if (!outputFile.is_open()) {
            cerr << "ERROR: Could not create backup file:\n"
                << filename << '\n';
            return false;
        }

        for (const auto& entry : frequencies) {
            outputFile << displayNames.at(entry.first)
                << " " << entry.second << '\n';
        }

        outputFile.close();

        if (outputFile.fail()) {
            cerr << "ERROR: Failed while saving the backup file.\n";
            return false;
        }

        return true;
    }

    void searchItem(const string& item) const {
        string key = normalize(item);
        auto found = frequencies.find(key);

        if (found != frequencies.end()) {
            cout << displayNames.at(key)
                << " appears " << found->second
                << " time(s).\n";
        }
        else {
            cout << item << " appears 0 time(s).\n";
        }
    }

    void displayAll() const {
        cout << "\nItem Frequencies\n";
        cout << "----------------\n";

        for (const auto& entry : frequencies) {
            cout << displayNames.at(entry.first)
                << ": " << entry.second << '\n';
        }
    }

    void displayHistogram() const {
        cout << "\nItem Frequency Histogram\n";
        cout << "------------------------\n";

        for (const auto& entry : frequencies) {
            cout << displayNames.at(entry.first) << ": ";

            for (int i = 0; i < entry.second; i++) {
                cout << '*';
            }

            cout << '\n';
        }
    }
};

void displayMenu() {
    cout << "\n========== Corner Grocer ==========\n";
    cout << "1. Search for an item\n";
    cout << "2. Display all item frequencies\n";
    cout << "3. Display item frequency histogram\n";
    cout << "4. Exit\n";
    cout << "Enter your choice (1-4): ";
}

string selectInputFile() {
    char filename[MAX_PATH] = {};

    OPENFILENAMEA dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.hwndOwner = nullptr;
    dialog.lpstrFilter =
        "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    dialog.lpstrFile = filename;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrTitle = "Select CS210 Project Three Input File";
    dialog.Flags =
        OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST |
        OFN_HIDEREADONLY;

    if (GetOpenFileNameA(&dialog)) {
        return string(filename);
    }

    return "";
}

string getExecutableFolder() {
    char path[MAX_PATH] = {};

    DWORD length = GetModuleFileNameA(nullptr, path, MAX_PATH);

    if (length == 0 || length >= MAX_PATH) {
        return "";
    }

    string fullPath(path);
    size_t slash = fullPath.find_last_of("\\/");

    if (slash == string::npos) {
        return "";
    }

    return fullPath.substr(0, slash + 1);
}

int main() {
    cout << "====================================\n";
    cout << "     CORNER GROCER TRACKER\n";
    cout << "====================================\n";
    cout << "Select CS210_Project_Three_Input_File.txt\n";

    string inputPath = selectInputFile();

    if (inputPath.empty()) {
        cerr << "\nNo input file was selected.\n";
        cerr << "Run the program again and select the text file.\n";
        system("pause");
        return 1;
    }

    GroceryTracker tracker;

    if (!tracker.loadItems(inputPath)) {
        system("pause");
        return 1;
    }

    if (tracker.isEmpty()) {
        cerr << "\nERROR: The selected file contains no grocery items.\n";
        system("pause");
        return 1;
    }

    string executableFolder = getExecutableFolder();

    if (executableFolder.empty()) {
        cerr << "ERROR: Could not determine the executable folder.\n";
        system("pause");
        return 1;
    }

    string backupPath = executableFolder + "frequency.dat";

    if (!tracker.createBackup(backupPath)) {
        system("pause");
        return 1;
    }

    cout << "\nBackup created successfully:\n"
        << backupPath << '\n';

    string choice;

    while (true) {
        displayMenu();

        if (!getline(cin, choice)) {
            break;
        }

        size_t first = choice.find_first_not_of(" \t\r\n");

        if (first == string::npos) {
            cout << "Invalid choice. Enter 1, 2, 3, or 4.\n";
            continue;
        }

        size_t last = choice.find_last_not_of(" \t\r\n");
        choice = choice.substr(first, last - first + 1);

        if (choice == "1") {
            string item;

            cout << "Enter the grocery item to search for: ";

            if (!getline(cin, item)) {
                break;
            }

            size_t itemStart =
                item.find_first_not_of(" \t\r\n");

            if (itemStart == string::npos) {
                cout << "Please enter an item name.\n";
                continue;
            }

            size_t itemEnd =
                item.find_last_not_of(" \t\r\n");

            item = item.substr(
                itemStart, itemEnd - itemStart + 1);

            tracker.searchItem(item);
        }
        else if (choice == "2") {
            tracker.displayAll();
        }
        else if (choice == "3") {
            tracker.displayHistogram();
        }
        else if (choice == "4") {
            cout << "Thank you for using Corner Grocer. Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid choice. Enter 1, 2, 3, or 4.\n";
        }
    }

    return 0;
}
