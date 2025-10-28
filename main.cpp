#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cmath>
#include <limits>
#include <algorithm>
#include <set>
#include <sstream>
#include <fstream>

// ANSI escape codes for colors
#define RESET       "\033[0m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLD        "\033[1m"
#define UNDERLINE   "\033[4m"

using namespace std;

struct Book {
    int id;
    string title;
    int copies;

    // Define an equality operator to compare books by their ID
    bool operator == (const Book& other) const {
        return id == other.id;
    }
};

struct BorrowedBookDetails {
    int id;
    string dateBorrow;
    string dateReturn;
    int overdueFee = 0;
};

struct Borrower {
    int id;
    string lastName, firstName, middleInitial;
    vector<BorrowedBookDetails> borrowedBooks; // Stores multiple borrowed books
};


vector<Book> fictionBooks;
vector<Book> nonFictionBooks;
vector<Book> scienceBooks;
vector<Book> mysteryBooks;
vector<Book> romanceBooks;
vector<Book> biographyBooks;
vector<Book> historyBooks;
vector<Book> technologyBooks;
vector<Book> childrenBooks;
vector<Book> artBooks;
vector<Borrower> borrowers;
set<int> uniqueBookIDs;
set<int> uniqueBorrowerIDs;

const string BOOKS_FILE = "books.txt";
const string BORROWERS_FILE = "borrowers.txt";

void displayMainMenu();
void displayAddMenu();
void addBook();
void displayMenu();
void displayBooks();
void displaySearchMenu();
void searchBook();
void addBorrower();
void viewBorrowers();
void searchBorrower();
void borrowBook();
void displayBorrowedDetails(const Borrower& borrower);
void returnBook();
void displayTableHeader();
void displayTable(const vector<Book>& books, const string& header);
void displayBorrowerTableHeader();
void displayBorrowerTable(const Borrower& borrower);
int calculateOverdueFee(const string& borrowDate, const string& returnDate);
void saveBooks();
void loadBooks();
void saveBorrowers();
void loadBorrowers();
void displayLogo();

bool isValidDate(const string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    int year, month, day;
    if (sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day) != 3) {
        return false;
    }

    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }

    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) {
        return false;
    }

    if (month == 2) {
        bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
        if (day > (isLeap ? 29 : 28)) {
            return false;
        }
    }
    return true;
}

int main() {
    loadBooks();      // Load books at the start
    loadBorrowers();
    displayMainMenu();
    borrowBook();
    return 0;
}

void saveBooks() {
    ofstream outFile(BOOKS_FILE);
    if (outFile.is_open()) {
        for (const auto& book : fictionBooks) {
            outFile << "Fiction," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : nonFictionBooks) {
            outFile << "NonFiction," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : scienceBooks) {
            outFile << "Science Fiction & Fantasy," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : mysteryBooks) {
            outFile << "Mystery & Thriller," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : romanceBooks) {
            outFile << "Romance," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : biographyBooks) {
            outFile << "Biography & Autobiography," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : historyBooks) {
            outFile << "History," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : technologyBooks) {
            outFile << "Science & Technology," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : childrenBooks) {
            outFile << "Children's Book," << book.id << "," << book.title << "," << book.copies << endl;
        }
        for (const auto& book : artBooks) {
            outFile << "Art & Design," << book.id << "," << book.title << "," << book.copies << endl;
        }

        outFile.close();
    } else {
        cout << "Error opening books file for writing.\n";
    }
}

void loadBooks() {
    ifstream inFile(BOOKS_FILE);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            string category;
            Book book;
            getline(ss, category, ',');
            ss >> book.id;
            ss.ignore();  // Ignore the comma
            getline(ss, book.title, ',');
            ss >> book.copies;

            // Add book to the appropriate category
            if (category == "Fiction") fictionBooks.push_back(book);
            else if (category == "NonFiction") nonFictionBooks.push_back(book);
            else if (category == "Science Fiction & Fantasy") scienceBooks.push_back(book);
            else if (category == "Mystery & Thriller") mysteryBooks.push_back(book);
            else if (category == "Romance") romanceBooks.push_back(book);
            else if (category == "Biography & Autobiography") biographyBooks.push_back(book);
            else if (category == "History") historyBooks.push_back(book);
            else if (category == "Science & Technology") scienceBooks.push_back(book);
            else if (category == "Children's Book") childrenBooks.push_back(book);
            else if (category == "Art & Design") artBooks.push_back(book);
        }
        inFile.close();
    } else {
        cout << "Error opening books file for reading.\n";
    }
}

void saveBorrowers() {
    ofstream outFile(BORROWERS_FILE);
    if (outFile.is_open()) {
        for (const auto& borrower : borrowers) {
            outFile << borrower.id << "," << borrower.lastName << "," << borrower.firstName << ","
                    << borrower.middleInitial << ",";

            if (borrower.borrowedBooks.empty()) {
                outFile << "~~~";
            } else {
                // Save borrowed book details
                for (const auto& book : borrower.borrowedBooks) {
                    outFile << book.id << "~" << book.dateBorrow << "~" << book.dateReturn << "~" << book.overdueFee << "~";
                }
                outFile.seekp(-1, ios_base::cur); // Remove the last '~'
            }
            outFile << "\n";
        }
        outFile.close();
    } else {
        cout << "Error opening borrowers file for writing.\n";
    }
}

void loadBorrowers() {
    ifstream inFile(BORROWERS_FILE);
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            stringstream ss(line);
            Borrower borrower;
            string borrowedBookID, dateBorrow, dateReturn, strOverdueFee;

            ss >> borrower.id;
            ss.ignore();  // Ignore the comma
            getline(ss, borrower.lastName, ',');
            getline(ss, borrower.firstName, ',');
            getline(ss, borrower.middleInitial, ',');

            // Read borrowed book details
            while (getline(ss, borrowedBookID, '~')) {
                if (borrowedBookID.length() == 0) break; // No borrowed books
                getline(ss, dateBorrow, '~');
                getline(ss, dateReturn, '~');
                getline(ss, strOverdueFee, '~');

                BorrowedBookDetails borrowedBook;
                borrowedBook.id = stoi(borrowedBookID);
                borrowedBook.dateBorrow = dateBorrow;
                borrowedBook.dateReturn = dateReturn;
                try {
                    borrowedBook.overdueFee = stoi(strOverdueFee);
                } catch (const invalid_argument& e) {
                    borrowedBook.overdueFee = 0; // Default to 0 if conversion fails
                }

                borrower.borrowedBooks.push_back(borrowedBook);
            }

            borrowers.push_back(borrower);
            uniqueBorrowerIDs.insert(borrower.id); // Ensure unique IDs
        }
        inFile.close();
    } else {
        cout << "Error opening borrowers file for reading.\n";
    }
}

void displayLogo(){
    cout << CYAN << BOLD << "    ________      __  __      ______     __         ______    __     ______              "<< RESET << endl;
    cout << CYAN << BOLD << "   /\\   ____\\    /\\ \\_\\ \\    /\\  ___\\   /\\ \\       /\\  ___\\  /\\ \\   /\\  ___\\                             "<< RESET <<endl;
    cout << CYAN << BOLD << "   \\ \\____   \\   \\ \\  __ \\   \\ \\  __\\   \\ \\ \\____  \\ \\  __\\  \\ \\ \\  \\ \\  __\\                       "<< RESET <<endl;
    cout << CYAN << BOLD << "    \\/\\_______\\   \\ \\_\\ \\_\\   \\ \\_____\\  \\ \\_____\\  \\ \\_\\     \\ \\_\\  \\ \\_____\\                                "<< RESET <<endl;
    cout << CYAN << BOLD << "     \\/_______/    \\/_/\\/_/    \\/_____/   \\/_____/   \\/_/      \\/_/   \\/_____/                 "<< RESET <<endl;
    cout << BLUE << BOLD << "\n\t\t==== Library Book Borrowing System ====\n\n" << RESET;
}

void displayMainMenu() {

    int choice;
    do {

        displayLogo();
        cout << BLUE << BOLD << "\n\t====MAIN MENU====\n" << RESET;
        cout << "\t[1] Add Menu\n";
        cout << "\t[2] Display Menu\n";
        cout << "\t[3] Search Menu\n";
        cout << "\t[4] Borrow Book\n";
        cout << "\t[5] Return Book\n";
        cout << "\t[6] Exit\n";
        cout << BLUE << BOLD << "\tEnter your choice: " << RESET;
        cin >> choice;

        switch (choice) {
            case 1:
                displayAddMenu();
                break;
            case 2:
                displayMenu();
                break;
            case 3:
                displaySearchMenu();
                break;
            case 4:
                borrowBook();
                break;
            case 5:
                returnBook();
                break;
            case 6:
                char confirm;
                cout << RED << BOLD <<"\tAre you sure you want to exit the system? (Y/N): " RESET;
                cin >> confirm;
                saveBooks();
                saveBorrowers();
                if (confirm == 'Y' || confirm == 'y') {
                    cout << "\tExiting system. Goodbye!\n";
                    exit(0);
                } else {
                    choice = 0; // Reset the choice to prevent exiting
                     system("CLS");
                }
                break;
           default:
            cout << RED << BOLD <<"\tInvalid choice. Please try again.\n" << RESET;
            cout << BLUE << BOLD <<"\n\tPress Enter to return to the main menu..." << RESET;
            cin.clear();
            cin.ignore();
            cin.get();
            system("CLS"); // Use "clear" for Unix/Linux systems
            break;

        }
    } while (choice != 6);
}

void displayTableHeader() {
    cout << "\t----------------------------------------------------\n";
    cout << "\t| ID        | Title                      | Copies  |\n";
    cout << "\t----------------------------------------------------\n";
}

void displayTable(const vector<Book>& books, const string& header = "") {
    for (const auto& book : books) {

        cout << "\t| " << setw(10) << right << book.id << "| "
             << setw(25) << left << book.title.substr(0, 25) << "  | "
             << setw(7) << right << book.copies << " |\n";
    }
    cout << "\t----------------------------------------------------\n";
}

void displayAddMenu() {
    int choice;
    do {
        system("CLS");
        displayLogo();
        cout << BLUE << BOLD << "\n\t==== Add Menu ====\n" << RESET;
        cout << "\t[1] Add Book\n";
        cout << "\t[2] Add Borrower\n";
        cout << "\t[3] Return to Main Menu\n";
        cout << BLUE << BOLD << "\tEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                addBorrower();
                break;
            case 3:
                system("CLS");
                return;  // Return to the main menu
            default:
                cout << RED << BOLD << "\tInvalid choice. Please try again.\n" << RESET;
                cout << BLUE << BOLD << "\n\tPress Enter to return to the Main menu..." << RESET;
                cin.clear();
                cin.ignore();
                cin.get();
                system("CLS"); // Use "clear" for Unix/Linux systems
                displayMainMenu();
        }
    } while (choice != 3);
}


void addBook() {
    int category;

    // Clear the screen at the start
    system("CLS");  // Use "clear" for Unix/Linux systems

    displayLogo();
    cout << BLUE << BOLD << "\n\tSELECT BOOK CATEGORY:\n" << RESET;
    cout << "\t[1] Fiction\n";
    cout << "\t[2] Non-Fiction\n";
    cout << "\t[3] Science Fiction & Fantasy\n";
    cout << "\t[4] Mystery & Thriller\n";
    cout << "\t[5] Romance\n";
    cout << "\t[6] Biography & Autobiography\n";
    cout << "\t[7] History\n";
    cout << "\t[8] Science & Technology\n";
    cout << "\t[9] Children's Book\n";
    cout << "\t[10] Art & Design\n";
    cout << BLUE << BOLD << "\tEnter category: " << RESET;
    cin >> category;

    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (category < 1 || category > 10) {
        cout << RED << BOLD <<"\tInvalid category. Please enter a number between 1 and 10.\n" << RESET;
        return;
    }

    system("CLS");
    Book newBook;
     displayLogo();
    cout << "\tEnter Book ID: ";
    cin >> newBook.id;

    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (uniqueBookIDs.find(newBook.id) != uniqueBookIDs.end()) {
        cout << RED << BOLD << "\tError: Book ID must be unique. Book not added.\n" << RESET;
        cout << BLUE << BOLD << "\n\tPress Enter to return to the Main menu..." << RESET;
        cin.clear();
        cin.ignore();
        cin.get();
        system("CLS"); // Use "clear" for Unix/Linux systems
        displayMainMenu();
    }

    cout << "\tEnter Book Title: ";
    getline(cin, newBook.title);

    cout << "\tEnter Number of Copies: ";
    string copiesInput;
    getline(cin, copiesInput);
    if (!copiesInput.empty()) {
        newBook.copies = stoi(copiesInput);
    } else {
        cout << RED << BOLD << "\tInvalid number of copies. Please enter a valid number.\n" << RESET;
        return;
    }

    // Add the book to the respective category
    switch (category) {
        case 1: fictionBooks.push_back(newBook); break;
        case 2: nonFictionBooks.push_back(newBook); break;
        case 3: scienceBooks.push_back(newBook); break;
        case 4: mysteryBooks.push_back(newBook); break;
        case 5: romanceBooks.push_back(newBook); break;
        case 6: biographyBooks.push_back(newBook); break;
        case 7: historyBooks.push_back(newBook); break;
        case 8: technologyBooks.push_back(newBook); break;
        case 9: childrenBooks.push_back(newBook); break;
        case 10: artBooks.push_back(newBook); break;
        default:
            cout << "Invalid category. Book not added.\n";
            return;
    }

    uniqueBookIDs.insert(newBook.id);

    // Display the recently added book
    cout << GREEN << BOLD <<"\n\tBOOK ADDED SUCCESSFULLY!\n" << RESET;
    cout << BLUE << BOLD << "\tRecently Added Book Details:\n" << RESET;

    displayTableHeader();
    displayTable({newBook});

    // Clear input buffer and screen after displaying the book
    cin.clear();
    cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu...";
    cin.get();  // Wait for the user to press Enter
    system("CLS");  // Use "clear" for Unix/Linux systems
    displayMainMenu();
}

void displayCategoryBooks(vector<Book>* books, const string& categoryName) {


    cout << BLUE << BOLD << "\n\tCategory: " << categoryName << "\n" << RESET;

    if (books->empty()) {
        cout << YELLOW << BOLD << "\tNo books available in this category.\n" << RESET;
    } else {
        // Call the displayTable function to display the table
        displayTableHeader();
        displayTable(*books);
    }

    // Clear input buffer
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Pause before returning
    cout << BLUE << BOLD << "\n\tPress Enter to return to the previous menu..." << RESET;
    cin.get();
    system("CLS");  // Use "clear" for Unix/Linux systems
}

void displayMenu() {
    int displayChoice;
    do {
        system("CLS");
        displayLogo();
        cout << BLUE << BOLD << "\n\t==== Display Menu ====\n" << RESET;
        cout << "\t[1] Display Books\n";
        cout << "\t[2] View Borrowers\n";
        cout << "\t[3] Back to Main Menu\n";
        cout << BLUE << BOLD << "\tEnter your choice: " << RESET;
        cin >> displayChoice;

        switch (displayChoice) {
            case 1:
                displayBooks();
                break;
            case 2:
                viewBorrowers();
                break;
            case 3:
                system("CLS");
                return;

                  // Return to the main menu
            default:
                cout << RED << BOLD << "\tInvalid choice. Please try again.\n" << RESET;
                cout << BLUE << BOLD << "\n\tPress Enter to return to the Main menu..." << RESET;
                cin.clear();
                cin.ignore();
                cin.get();
                system("CLS"); // Use "clear" for Unix/Linux systems
                displayMainMenu();

        }
    } while (displayChoice != 3);
}

void displayBooks() {
    system("CLS");  // Clear the screen
    int category;

    displayLogo();
    cout << BLUE << BOLD << "\n\tSelect Book Category:\n" << RESET;
    cout << "\t[1] Fiction\n";
    cout << "\t[2] Non-Fiction\n";
    cout << "\t[3] Science Fiction & Fantasy\n";
    cout << "\t[4] Mystery & Thriller\n";
    cout << "\t[5] Romance\n";
    cout << "\t[6] Biography & Autobiography\n";
    cout << "\t[7] History\n";
    cout << "\t[8] Science & Technology\n";
    cout << "\t[9] Children's Book\n";
    cout << "\t[10] Art & Design\n";
    cout << "\t[11] Display all books\n";
    cout << BLUE << BOLD << "\tEnter category: " << RESET;
    cin >> category;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << BOLD << "\tInvalid input. Please enter a valid number.\n" << RESET;
        return;
    }

    vector<pair<vector<Book>*, string>> categories = {
        {&fictionBooks, "\tFiction"},
        {&nonFictionBooks, "\tNon-Fiction"},
        {&scienceBooks, "\tScience Fiction & Fantasy"},
        {&mysteryBooks, "\tMystery & Thriller"},
        {&romanceBooks, "\tRomance"},
        {&biographyBooks, "\tBiography & Autobiography"},
        {&historyBooks, "\tHistory"},
        {&technologyBooks, "\tScience & Technology"},
        {&childrenBooks, "\tChildren's Book"},
        {&artBooks, "\tArt & Design"},
    };

    if (category >= 1 && category <= 10) {
        displayCategoryBooks(categories[category - 1].first, categories[category - 1].second);
    } else if (category == 11) {
        vector<Book> allBooks;

        for (const auto& cat : categories) {
            allBooks.insert(allBooks.end(), cat.first->begin(), cat.first->end());
        }

        cout << BLUE << BOLD << "\n\tDisplaying all books:\n" << RESET;
        if (allBooks.empty()) {
            cout << YELLOW << BOLD <<"\tNo books available to display.\n" << RESET;
        } else {
            displayTableHeader();
            displayTable(allBooks);
        }

        // Pause before returning
        cout << BLUE << BOLD << "\n\tPress Enter to return to the menu..." << RESET;
        cin.ignore();
        cin.get();
        system("CLS");
    } else {
        cout << RED << BOLD <<"\tInvalid category.\n" << RESET;
    }
}


void searchBook() {

    system("CLS");  // Use "clear" for Unix/Linux systems

    int choice;
    int bookID;
    displayLogo();

    cout << "\tEnter Book ID: ";
    cin >> bookID;

    // Function to find and manage books
    auto findAndManageBook = [&](vector<Book>& books) -> bool {
        auto it = find_if(books.begin(), books.end(), [bookID](const Book& book) {
            return book.id == bookID;
        });

        if (it != books.end()) {
            cout << GREEN << BOLD << "\n\tBook Found:\n" << RESET;

            displayTableHeader();

            // Display the found book details
            vector<Book> foundBook = {*it};  // Create a vector with the found book
            displayTable(foundBook);  // Display the book in table format

            cout << "\t[1] Edit\n";
            cout << "\t[2] Delete\n";
            cout << "\t[3] Go Back to Main Menu\n";
            cout << BLUE << BOLD << "\tEnter your choice: " << RESET;
            cin >> choice;

            switch (choice) {
                case 1: {
                    // Ask which part of the book the user wants to edit
                    int editChoice;
                    cout << BLUE << BOLD << "\n\tWhat would you like to edit?\n" << RESET;
                    cout << "\t[1] Title\n";
                    cout << "\t[2] Number of Copies\n";
                    cout << "\t[3] Both Title and Copies\n";
                    cout << BLUE << BOLD << "\tEnter your choice: " << RESET;
                    cin >> editChoice;

                    if (editChoice == 1) {
                        cout << BOLD <<"\tEnter new title: " << RESET;
                        cin.ignore(); // Clear input buffer
                        getline(cin, it->title);
                        cout << GREEN << BOLD << "\tBook title updated successfully.\n" << RESET;
                        system("CLS");
                    }
                    else if (editChoice == 2) {
                        cout << BOLD << "\tEnter new number of copies: " << RESET;
                        cin >> it->copies;
                        cout << GREEN << BOLD << "\tNumber of copies updated successfully.\n" << RESET;
                        system("CLS");
                    }
                    else if (editChoice == 3) {
                        cout << BOLD << "\tEnter new title: " << RESET;
                        cin.ignore(); // Clear input buffer
                        getline(cin, it->title);
                        cout << BOLD << "\tEnter new number of copies: " << RESET;
                        cin >> it->copies;
                        cout << GREEN << BOLD <<"\tBook updated successfully.\n" << RESET;
                        system("CLS");
                    }
                    else {
                        cout << RED << BOLD << "\tInvalid choice. Returning to Main Menu.\n" << RESET;
                    }

                    displayLogo();

                    // Display the updated book details in a tabular format
                    cout << BLUE << BOLD << "\n\tUpdated Book:\n" << RESET;

                    displayTableHeader();  // Display the table header

                    vector<Book> updatedBook = {*it};  // Create a vector with the updated book

                    displayTable(updatedBook);  // Display the updated book in table format
                    // Pause before returning to the search menu
                    cout << BLUE << BOLD << "\n\tPress Enter to return to the Search Menu..." << RESET;
                    cin.ignore();
                    cin.get();
                    break;
                }
                case 2:
                    char confirm;
                    cout << RED << BOLD << "\tAre you sure you want to delete this book? (y/n): " << RESET;
                    cin >> confirm;
                    if (confirm == 'y' || confirm == 'Y') {
                        books.erase(it); // Remove the book from the list
                        cout << GREEN << BOLD << "\tBook deleted successfully.\n" << RESET;
                    } else {
                        cout << GREEN << BOLD << "\tBook deletion canceled.\n" << RESET;
                    }
                     // Pause before returning to the search menu
                    cout << BLUE << BOLD << "\n\tPress Enter to return to the Search Menu..." << RESET;
                    cin.ignore();
                    cin.get();
                    break;
                case 3:
                    cout << "\tReturning to Search Menu.\n";
                    return true;
                default:
                    cout << RED << BOLD << "Invalid choice. Please try again.\n" << RESET;
                    cout << BLUE << BOLD <<"\nPress Enter to return to the main menu..." << RESET;
                    cin.clear();
                    cin.ignore();
                    cin.get();
                    system("CLS"); // Use "clear" for Unix/Linux systems
            }
            return true;
        }
        return false;
    };

    // Search in fictionBooks and nonFictionBooks
    if (!findAndManageBook(fictionBooks) && !findAndManageBook(nonFictionBooks) && !findAndManageBook(scienceBooks)  && !findAndManageBook(mysteryBooks ) && !findAndManageBook(romanceBooks) && !findAndManageBook(biographyBooks) && !findAndManageBook(historyBooks) && !findAndManageBook(technologyBooks) && !findAndManageBook(childrenBooks) && !findAndManageBook(artBooks)) {
        cout << RED << BOLD << "\tBook not found.\n" << RESET;

        // Pause before returning to the search menu
        cout << BLUE << BOLD << "\n\tPress Enter to return to the Search Menu..." << RESET;
        cin.ignore();
        cin.get();
    }
}

void displaySearchMenu() {
    int choice;
    do {
        system("CLS"); // Clear the screen
        displayLogo();
        cout << BLUE << BOLD << "\t==== Search Menu ====\n" << RESET;
        cout << "\t[1] Search Book\n";
        cout << "\t[2] Search Borrower\n";
        cout << "\t[3] Return to Main Menu\n";
        cout << BLUE << BOLD << "\tEnter your choice: " << RESET;
        cin >> choice;

        switch (choice) {
            case 1:
                searchBook();
                break;
            case 2:
                searchBorrower();
                break;
            case 3:
                cout << "\tReturning to Main Menu.\n";
                system("CLS");
                return;
            default:
                cout << RED << BOLD << "\tInvalid choice. Please try again.\n" << RESET;
                cout << BLUE << BOLD << "\n\tPress Enter to return to the Main menu..." << RESET;
                cin.clear();
                cin.ignore();
                cin.get();
                system("CLS"); // Use "clear" for Unix/Linux systems
                displayMainMenu();
        }
    } while (choice != 3);
}

void searchBorrower() {
    int borrowerID;
    system("CLS");
    displayLogo();
    cout << BLUE << BOLD << "\tEnter Borrower ID to search: " << RESET;
    cin >> borrowerID;

    // Find borrower by ID
    auto it = find_if(borrowers.begin(), borrowers.end(), [borrowerID](const Borrower& b) {
        return b.id == borrowerID;
    });

    if (it != borrowers.end()) {
        cout << GREEN << BOLD << "\n\tBorrower Found:\n" << RESET;
        cout << "\t--------------------------------------\n";
        cout << "\t| " << setw(10) << "ID" << " | " << setw(20) << "Name" << "  |\n";
        cout << "\t--------------------------------------\n";
        cout << "\t| " << setw(10) << it->id << " | "
             << setw(20) << it->firstName + it->middleInitial + " " + it->lastName << "  |\n";
        cout << "\t--------------------------------------\n";



        // Provide options
        int choice;
        cout << "\n\t[1] View Borrowed Books\n";
        cout << "\t[2] Return to Main Menu\n";
        cout << BLUE << BOLD << "\tEnter your choice: " << RESET;
        cin >> choice;

        if (choice == 1) {
            if (it->borrowedBooks.empty()) {
                cout << RED << BOLD << "\tNo borrowed books.\n" << RESET;
            } else {
                system("CLS");
                displayLogo();
                cout << "\n\tBorrowed Books:\n";
                cout << "\t----------------------------------------------------------------------\n";
                cout << "\t| " << setw(20) << "Book Title"
                        << " \t| " << setw(20) << "Date Borrowed"
                        << " \t| " << setw(20) << "Date Returned" << " |\n";
                cout << "\t----------------------------------------------------------------------\n";

                for (const auto& book : it->borrowedBooks) {
                    string bookTitle;
                    for (const auto& category : {fictionBooks, nonFictionBooks, scienceBooks, mysteryBooks, romanceBooks,
                                                 biographyBooks, historyBooks, technologyBooks, childrenBooks, artBooks}) {
                        auto it = find_if(category.begin(), category.end(), [&book](const Book& b) {
                            return b.id == book.id;
                        });
                        if (it != category.end()) {
                            bookTitle = it->title;
                            break;
                        }
                    }

                    cout << "\t| " << setw(20) << bookTitle
                         << " \t| " << setw(20) << book.dateBorrow
                         << " \t| " << setw(20) << (book.dateReturn.empty() ? "Not Returned" : book.dateReturn) << " |\n";
                    cout << "\t----------------------------------------------------------------------\n";
                }
            }

            // Wait for user input to continue
            cout << BLUE << BOLD << "\n\tPress any key to return to the menu...\n" << RESET;
            cin.ignore();
            cin.get();
        } else if (choice == 2) {
            return; // Go back to main menu
        } else {
            cout << RED << BOLD << "\tInvalid choice. Returning to main menu.\n" << RESET;
        }
    } else {
        cout << RED << BOLD << "\tBorrower with ID " << borrowerID << " not found.\n" << RESET;
        cout << BLUE << BOLD << "\n\tPress Enter to return to the Search Menu..." << RESET;
        cin.ignore();
        cin.get();
    }
}


void addBorrower() {
    Borrower newBorrower;
    system("CLS");
    displayLogo();
    cout << "\tEnter Borrower ID: ";
    cin >> newBorrower.id;

    // Check if the ID is unique
    if (uniqueBorrowerIDs.find(newBorrower.id) != uniqueBorrowerIDs.end()) {
        cout << RED << BOLD << "\tError: Borrower ID must be unique. Borrower not added.\n" << RESET;
        return;
    }


    cin.ignore();
     // Clear the input buffer
    cout << "\tEnter Last Name: ";
    getline(cin, newBorrower.lastName);
    cout << "\tEnter First Name: ";
    getline(cin, newBorrower.firstName);
    cout << "\tEnter Middle Initial: ";
    getline(cin, newBorrower.middleInitial);

    borrowers.push_back(newBorrower);
    uniqueBorrowerIDs.insert(newBorrower.id); // Add the ID to the unique set
    cout << GREEN << BOLD << "\tBorrower added successfully!\n" << RESET;

    // Display the recently added borrower in table format
    displayBorrowerTableHeader();
    displayBorrowerTable(newBorrower);
    cout << "\n\tPress Enter to return to the Display Menu...";

    cin.clear(); // Clear any error flags

    cin.get(); // Waits for the user to press Enter


}

void displayBorrowerTableHeader() {
    cout << "\t----------------------------------------------------------------------------------------------------\n";
    cout << "\t| ID        | Full Name                | Book                | Borrowed Date | Return Date | Fee   |\n";
    cout << "\t----------------------------------------------------------------------------------------------------\n";

}

void displayBorrowerTable(const Borrower& borrower) {
    if (borrower.borrowedBooks.empty()) {

        cout << "\t| " << left << setw(10) << borrower.id
             << "| " << setw(25) << borrower.firstName + " " + borrower.middleInitial + " " + borrower.lastName
             << "| " << setw(20) << "No books borrowed"  // Indicating no books borrowed
             << "| " << setw(14) << "N/A"               // No borrow date
             << "| " << setw(12) << "N/A"               // No return date
             << "| " << setw(6) << "N/A" << "|\n";
    } else {
        for (const auto& bookDetails : borrower.borrowedBooks) {
            string bookTitle;
            for (const auto& category : {fictionBooks, nonFictionBooks, scienceBooks, mysteryBooks, romanceBooks,
                                            biographyBooks, historyBooks, technologyBooks, childrenBooks, artBooks}) {
                auto it = find_if(category.begin(), category.end(), [&bookDetails](const Book& b) {
                    return b.id == bookDetails.id;
                });
                if (it != category.end()) {
                    bookTitle = it->title;
                    break;
                }
            }

            cout << "\t| " << left << setw(10) << borrower.id
                 << "| " << setw(25) << borrower.firstName + " " + borrower.middleInitial + " " + borrower.lastName
                 << "| " << setw(20) << bookTitle
                 << "| " << setw(14) << bookDetails.dateBorrow
                 << "| " << setw(12) << bookDetails.dateReturn
                 << "| " << setw(6) << bookDetails.overdueFee << "|\n";
        }
    }
    cout << "\t----------------------------------------------------------------------------------------------------\n";
}

void viewBorrowers() {
    cout << BLUE << BOLD << "\n\t==== List of Borrowers ====\n" << RESET;

    if (borrowers.empty()) {
        cout << RED << BOLD << "\tNo borrowers found.\n" << RESET;
    } else {
        // Display table header
        displayBorrowerTableHeader();

        // Display details for each borrower
        for (const auto& borrower : borrowers) {
            displayBorrowerTable(borrower);
        }
    }

    // Pause to allow the user to see the output
    cout << BLUE << BOLD << "\n\tPress Enter to return to the Display Menu..." << RESET;

    cin.clear(); // Clear any error flags
    cin.ignore(); // Discard leftover input
    cin.get(); // Waits for the user to press Enter
}

void borrowBook() {
    int borrowerID, bookID;
    string date;

    system("CLS");
    displayLogo();  // Use "clear" for Unix/Linux systems

    cout << "\tEnter Borrower ID: ";
    cin >> borrowerID;

    bool borrowerFound = false;
    for (auto& borrower : borrowers) {
        if (borrower.id == borrowerID) {
            borrowerFound = true;
            break;
        }
    }

    if (!borrowerFound) {
        cout << RED << BOLD << "\tError: Borrower ID not found. Please enter a valid Borrower ID.\n" << RESET;
        return;
    }

    cout << "\tEnter Book ID: ";
    cin >> bookID;

    bool bookFound = false;
    for (auto& book : fictionBooks) {
        if (book.id == bookID && book.copies > 0) {
            bookFound = true;
            break;
        }
    }
    if (!bookFound) {
        for (auto& book : nonFictionBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : scienceBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : mysteryBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : romanceBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : biographyBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : historyBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : technologyBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : childrenBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }

        for (auto& book : artBooks) {
            if (book.id == bookID && book.copies > 0) {
                bookFound = true;
                break;
            }
        }
    }

    if (!bookFound) {
        cout << RED << BOLD << "\tError: Book ID not found or no copies available.\n" << RESET;
        return;
    }

    cin.ignore();
    cout << "\tEnter Date of Borrow (YYYY-MM-DD): ";
    getline(cin, date);

    if (!isValidDate(date)) {
        cout << RED << BOLD << "\tInvalid date format. Please enter a valid date (YYYY-MM-DD).\n" RESET;
        cin.clear();
        cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu..." << RESET;
        cin.get();  // Wait for the user to press Enter
        system("CLS");  // Use "clear" for Unix/Linux systems
        displayMainMenu();
    }

    for (auto& borrower : borrowers) {
        if (borrower.id == borrowerID) {
            // Check Fiction books
            for (auto& book : fictionBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Fiction category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }

            // Repeat for other categories
            for (auto& book : fictionBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Non-Fiction category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }

            for (auto& book : nonFictionBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Non-Fiction category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }
            for (auto& book : scienceBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Science category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }
            for (auto& book : mysteryBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Mystery category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }
            for (auto& book : romanceBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Romance category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }

            for (auto& book : biographyBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Biography category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }
            for (auto& book : historyBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from History category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }
            for (auto& book : technologyBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Technology category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }
            for (auto& book : childrenBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Children category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }
            for (auto& book : artBooks) {
                if (book.id == bookID && book.copies > 0) {
                    borrower.borrowedBooks.push_back({book.id, date, ""});
                    book.copies--;
                    cout << GREEN << BOLD << "\tBook borrowed successfully from Art category!\n" << RESET;
                    displayBorrowedDetails(borrower);
                    return;
                }
            }

            cout << RED << BOLD << "\tBook not found or no copies available in any category.\n" << RESET;
            return;
        }
    }

    cout << RED << BOLD << "\tBorrowing failed. Borrower ID not found.\n" << RESET;
}

// Function definition for displaying borrower details
void displayBorrowedDetails(const Borrower& borrower) {
    cout << "\t--------------------------------------------------------------\n";
    cout << "\t| Borrower Name       | Book Title          | Date Borrowed  |\n";
    cout << "\t--------------------------------------------------------------\n";

    if (borrower.borrowedBooks.empty()) {
    cout << "\t| " << left << setw(58) << "No books borrowed yet." << " |\n";
    cout << "\t--------------------------------------------------------------\n";
    } else {
    // If there are borrowed books, print them
        for (const auto& borrowedBook : borrower.borrowedBooks) {
            string bookTitle;
            for (const auto& category : {fictionBooks, nonFictionBooks, scienceBooks, mysteryBooks, romanceBooks,
                                            biographyBooks, historyBooks, technologyBooks, childrenBooks, artBooks}) {
                auto it = find_if(category.begin(), category.end(), [&borrowedBook](const Book& b) {
                    return b.id == borrowedBook.id;
                });
                if (it != category.end()) {
                    bookTitle = it->title;
                    break;
                }
            }

            cout << "\t| " << left << setw(20) << borrower.firstName + " " + borrower.middleInitial + " " + borrower.lastName
                 << "| " << setw(20) << bookTitle
                 << "| " << setw(14) << borrowedBook.dateBorrow << " |\n";
        }
        cout << "\t--------------------------------------------------------------\n";
    cin.clear();
    cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu..." << RESET;
    cin.get();  // Wait for the user to press Enter
    system("CLS");  // Use "clear" for Unix/Linux systems
    displayMainMenu();
    }
}

void returnBook() {
    int borrowerID, bookID;
    string returnDate;

    system("CLS");
    displayLogo();
    // Input borrower and book IDs and the return date
    cout << "\tEnter Borrower ID: ";
    cin >> borrowerID;

    // Check if the borrower ID is valid
    bool borrowerFound = false;
    for (auto& borrower : borrowers) {
        if (borrower.id == borrowerID) {
            borrowerFound = true;
            break;
        }
    }

        if (!borrowerFound) {
            cout << RED << BOLD << "\tError: Borrower ID is not valid.\n" << RESET;
            cin.clear();
            cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu..." << RESET;
            cin.ignore();
            cin.get();  // Wait for the user to press Enter
            system("CLS");  // Use "clear" for Unix/Linux systems
            displayMainMenu();
        }

    cout << "\tEnter Book ID: ";
    cin >> bookID;

    // Check if the book ID is valid in any book category
    bool bookFound = false;
    for (auto& category : {fictionBooks, nonFictionBooks, scienceBooks, mysteryBooks, romanceBooks,
                           biographyBooks, historyBooks, technologyBooks, childrenBooks, artBooks}) {
        for (auto& book : category) {
            if (book.id == bookID) {
                bookFound = true;
                break;
            }
        }
        if (bookFound) {
            break;
        }
    }

    if (!bookFound) {
        cout << RED << BOLD << "\tError: Book ID is not valid or not found in the inventory.\n" << RESET;
        cin.clear();
        cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu..." << RESET;
        cin.ignore();
        cin.get();  // Wait for the user to press Enter
        system("CLS");  // Use "clear" for Unix/Linux systems
        displayMainMenu();
    }

    cin.ignore();
    cout << "\tEnter Date of Return (YYYY-MM-DD): ";
    getline(cin, returnDate);

    // Iterate through the list of borrowers
    for (auto& borrower : borrowers) {
        if (borrower.id == borrowerID && !borrower.borrowedBooks.empty()) {  // Check if the borrower exists and has borrowed books
            bool bookFoundInBorrowedBooks = false;

            // Find the book that matches the bookID in the borrower's list of borrowed books
            for (auto& borrowedBook : borrower.borrowedBooks) {
                if (bookFoundInBorrowedBooks) break;
                if (borrowedBook.id == bookID && borrowedBook.dateReturn == "") {

                    int overdueFee = calculateOverdueFee(borrowedBook.dateBorrow, returnDate);  // Calculate the overdue fee
                    borrowedBook.dateReturn = returnDate;  // Set the return date
                    borrowedBook.overdueFee = overdueFee;  // Update the borrower's overdue fee

                    if (overdueFee == 0) {
                        // Case 1: On-time return
                        cout << GREEN << BOLD << "\tBook returned SUCCESSFULLY.\n" << RESET;

                        displayBorrowerTableHeader();

                        // Display the borrower details using displayBorrowerTable
                        displayBorrowerTable(borrower);

                            cin.clear();
                            cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu...";
                            cin.get();  // Wait for the user to press Enter
                            //system("CLS");  // Use "clear" for Unix/Linux systems
                            //displayMainMenu();
                    } else {
                        string bookTitle;
                        for (const auto& category : {fictionBooks, nonFictionBooks, scienceBooks, mysteryBooks, romanceBooks,
                                                    biographyBooks, historyBooks, technologyBooks, childrenBooks, artBooks}) {
                            auto it = find_if(category.begin(), category.end(), [&borrowedBook](const Book& b) {
                                return b.id == borrowedBook.id;
                            });
                            if (it != category.end()) {
                                bookTitle = it->title;
                                break;
                            }
                        }

                        // Case 2: Late return with fee
                        cout << GREEN << BOLD << "\tBook returned SUCCESSFULLY." << RESET;
                        cout << RED << BOLD << "But, you need to pay for not following the rules.\n" << RESET;
                        cout << CYAN << BOLD << "\n\t--- E-Receipt ---\n" << RESET;
                        cout << "\tBorrower's Name: " << borrower.firstName + " " + borrower.middleInitial + " " + borrower.lastName << "\n";
                        cout << "\tBook Title: " << bookTitle << "\n";
                        cout << "\tDate Borrowed: " << borrowedBook.dateBorrow << "\n";
                        cout << "\tDate Returned: " << borrowedBook.dateReturn << "\n";
                        cout << RED << BOLD << "\tOverdue Fee: " << overdueFee << " pesos\n" << RESET;
                        cout << "\t------------------\n";
                        cin.clear();
                        cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu..." << RESET;
                        cin.get();  // Wait for the user to press Enter
                    }

                    // Return the book to inventory
                    for (auto& book : fictionBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : nonFictionBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : scienceBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copied
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : mysteryBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copied
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : romanceBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : biographyBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : historyBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : technologyBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : childrenBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }

                    for (auto& book : artBooks) {  // Repeat for all categories
                        if (book.id == bookID) {
                            book.copies++;  // Increase the available copies
                            system("CLS");  // Use "clear" for Unix/Linux systems
                            displayMainMenu();
                            return;
                        }
                    }
                    cout << RED << BOLD << "Book not found in the library inventory.\n" << RESET;
                    return;
                }
            }
        }
    }

    // If borrower or book ID not found
    cout << RED << BOLD << "\tBorrower ID or Book ID not found, or the borrower did not borrow any books.\n" << RESET;
    cin.clear();
    cout << BLUE << BOLD << "\n\tPress Enter to return to the main menu..." << RESET;
    cin.get();  // Wait for the user to press Enter
}


int daysSinceStartOfYear(int year, int month, int day) {
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int totalDays = 0;

    // Add days of months in the same year
    for (int i = 0; i < month - 1; ++i) {
        totalDays += daysInMonth[i];
    }

    // Add the current month's days
    totalDays += day;

    // Handle leap years for February
    if (month > 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        totalDays += 1; // Leap day
    }

    return totalDays;
}

int daysFromDate(const string& date) {
    int year, month, day;
    sscanf(date.c_str(), "%d-%d-%d", &year, &month, &day);

    // Calculate total days since 1900-01-01
    int totalDays = (year - 1900) * 365 + (year - 1900) / 4 - (year - 1900) / 100 + (year - 1900) / 400;
    totalDays += daysSinceStartOfYear(year, month, day);
    return totalDays;
}

int calculateOverdueFee(const string& borrowDate, const string& returnDate) {
    int borrowDays = daysFromDate(borrowDate);
    int returnDays = daysFromDate(returnDate);
    int daysBorrowed = returnDays - borrowDays;
    int maxDays = 7;
    int overdueDays = max(0, daysBorrowed - maxDays);
    return overdueDays * 5; // Fee of 5 pesos per day
}
