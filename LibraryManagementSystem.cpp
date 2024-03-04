#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <conio.h>
#include <Windows.h>
#include <ctime>
#include <chrono>
using namespace std;

int generateBookID(string s);
struct Book
{
    string name, author, genre;
    int id, amount;
    struct Book *next;

public:
    Book()
    {
        name = "";
        author = "";
        genre = "";
        id = 0;
        next = nullptr;
        amount = 0;
    }
    Book(string new_name, string new_author, string new_genre, int quantity)
    {
        name = new_name;
        author = new_author;
        genre = new_genre;
        id = generateBookID(new_name + new_author + new_genre);
        next = nullptr;
        amount = quantity;
    }
};

struct User
{
    string name, password;
    vector<Book *> books;
    struct User *next;
    User()
    {
        name = "";
        password = "";
        books = vector<Book *>();
        next = nullptr;
    }
    User(string n, string pwd)
    {
        name = n;
        password = pwd;
        books = vector<Book *>();
        next = nullptr;
    }
};

class HashTable
{
private:
    // HashTables for Books, Shelf and Users.
    unordered_map<int, Book *> books;
    unordered_map<string, vector<Book *>> shelf;
    unordered_map<int, User *> users;

public:
    // Hash function to generate index for a book
    int hashFunction(string book_name)
    {
        int id = generateBookID(book_name);
        return id % 997;
    }

    // Insert a book into the hash table
    void insertBook(Book *book)
    {

        // Create unique key for book using title, author, and genre
        int key = hashFunction(book->name);
        // Check if book already exists in the map
        if (books.find(key) != books.end())
        {
            // Book already exists in the map incrementing its quantity by 1.
            books[key]->amount += 1;
        }
        else
        {
            // Insert the book into the map
            books[key] = book;
            addBookToShelf(book);
        }
    }

    // Search for a book by name
    Book *searchBook()
    {
        string book_name, book_genre, author_name;
        cout << "\nBook Name: ";
        cin.ignore();
        getline(cin, book_name);
        cout << "\nAuthor Name: ";
        getline(cin, author_name);
        cout << "\nBook Genre: ";
        getline(cin, book_genre);

        int index = hashFunction(book_name);
        if (books.count(index) > 0)
        {
            if (books[index]->author == author_name && books[index]->genre == book_genre)
            {
                return books[index];
            }
        }
        cout << "[ERROR] Book not found in library." << endl;
        return nullptr;
    }

    // Delete a book by name
    bool deleteBook()
    {
        string book_name, book_genre, author_name;
        cout << "\nBook Name: ";
        cin.ignore();
        getline(cin, book_name);
        cout << "\nAuthor Name: ";
        getline(cin, author_name);
        cout << "\nBook Genre: ";
        getline(cin, book_genre);

        int index = hashFunction(book_name);
        if (books.count(index) > 0)
        {
            books.erase(index);
            return true;
        }
        return false;
    }

    // Adding books to shelf based upon there Genre.
    void addBookToShelf(Book *book)
    {
        string genre = book->genre;
        bool alreadyExists = false;

        // Iterate through the vector of the specific genre to check if a book is already on shelf.
        for (auto existingBook : shelf[genre])
        {
            if (existingBook->name == book->name && existingBook->author == book->author)
            {
                cout << "\n[ERROR] Book is already on shelf!\n";
                alreadyExists = true;
                break;
                return;
            }
        }

        if (!alreadyExists)
        {
            shelf[genre].push_back(book); // vector- modifier--push back is used to add element at the end.
        }
    }

    // Printing books by Genre.
    void printBooksByGenre(string genre)
    {
        auto it = shelf.find(genre);
        if (it == shelf.end())
        {
            cout << "[ERROR] Genre not found." << endl;
            return;
        }
        vector<Book *> books = it->second; // second is pre-defined member of <unordered_map>..used in iterations
        for (auto book : books)
        {
            cout << book->name << " by " << book->author << endl;
        }
    }

    // Printing all books in library.
    void printAllBooks()
    {
        unordered_map<int, Book *>::iterator it;
        int counter = 0;
        for (it = books.begin(); it != books.end(); it++)
        {
            cout << "ID: " << it->first << endl;               // first--int--BOOK ID in unordered map
            cout << "Book Name: " << it->second->name << endl; // second book pointer values
            cout << "Author: " << it->second->author << endl;
            cout << "Genre: " << it->second->genre << endl;
            cout << "Quantity Available: " << it->second->amount << endl;
            cout << endl;
            counter++;
        }
        // cout << counter;
    }

    // Adds user to hashtable
    void addUser(User *user)
    {
        // Create unique key for book using title, author, and genre
        int key = hashFunction(user->name);
        // Check if book already exists in the map
        if (users.find(key) != users.end())
        {
            // IF user already exists in the map gives error
            cout << "[ERROR] User already exists" << endl;
            return;
        }
        else
        {
            // Insert the user into the hashtable
            users[key] = user;
            cout << "[USER ADDED]" << endl;
        }
    }

    // Search for a user by their name.
    User *searchUser(string name)
    {
        int key = hashFunction(name);
        if (users.count(key) > 0)
        {
            return users[key];
        }
        return nullptr;
    }

    // Issues book to user, Returns True if issued successfully and decrements book quantity
    bool issueBook(User *user, Book *book)
    {
        try // exceptional handling
        {
            time_t now = time(nullptr);
            char *date = ctime(&now);

            if (book->amount == 0)
            {
                cout << "[NO COPIES AVAILABLE]" << endl;
                return false;
            }

            book->amount--;

            // adding issued book to user's issued books array.
            user->books.emplace_back(book);

            cout << "\nBook Issued by : " << user->name << " On " << date << endl;

            return true;
        }
        catch (exception e)
        {
            cout << "[EXCEPTION OCCURED]" << endl;
            return false;
        }
    }

    // Returns book issued by user, Returns True if returned successfully and increments book quantity
    bool returnBook(User *user, Book *book)
    {
        // To get current date and time
        time_t now = time(nullptr);
        char *date = ctime(&now);

        // checking if book exists in user's issued books array.
        for (int i = 0; i < user->books.size(); i++)
        {
            if (user->books[i]->name == book->name)
            {
                book->amount++;
                user->books.erase(user->books.begin() + i);
                cout << "\nBook Returned by : " << user->name << " On " << date << endl;
                return true;
            }
        }
        return false;
    }

    // Printing Books Issued by User.
    void printUserBooks(User *user)
    {
        if (user->books.empty())
        {
            cout << "[NO BOOKS ISSUED]\n";
        }
        else
        {
            cout << "[BOOKS ISSUED BY: " << user->name << " ]" << endl;
            cout << "\n\nBook Title \t\t\t Book Author \t\t\t Book Genre\n";
            for (auto book : user->books)
            {
                cout << book->name << "\t\t\t" << book->author << "\t\t\t" << book->genre << " " << endl;
            }
        }
    }
    // making a global object of HashTable to access all over the program.
} library;

// Generates Unique ID for book by its title
int generateBookID(string book_name)
{
    int g = 1009;
    int hashvalue = 0;
    for (int i = 0; i < book_name.length(); i++)
    {
        hashvalue += g * hashvalue + book_name.at(i);
    }
    if (hashvalue < 0)
    {
        // Making hash value positive.
        hashvalue = hashvalue & 0x7FFFFFFF;
        return hashvalue;
    }
    return hashvalue;
}

// Method that takes input to insert Book, Returns a pointer to Struct Book object to insert into hashtable
Book *bookToAdd()
{
    int book_quantity;
    string book_name, book_genre, author_name;
    cout << "\nBook Name: ";
    cin.ignore();
    getline(cin, book_name);
    cout << "\nAuthor Name: ";
    getline(cin, author_name);
    cout << "\nBook Genre: ";
    getline(cin, book_genre);
    cout << "Book Quantity: ";
    cin >> book_quantity;

    Book *temp = new Book(book_name, author_name, book_genre, book_quantity);
    return temp;
    delete temp;
}

void menu();

// Menu for librarian to access all methods
void librarian(User *user)
{
    int choice = 0;
    Book *searchedBook;
    User stdnt;
    string std_name, pwd, genre;
    bool deleted = false;
    system("cls");
    while (1)
    {
        cout << "\n\n\n++++++++++     M E N U     ++++++++++\n";
        cout << "Welcome " << user->name << endl
             << endl;
        cout << "0. Exit\n";
        cout << "1. Display All Books\n";
        cout << "2. Insert a Book\n";
        cout << "3. Delete a Book\n";
        cout << "4. Search Book\n";
        cout << "5. Show Books by Genre\n";
        cout << "6. Register a Student\n";
        cout << "7. LogOut\n";
        cout << "\nSelect an option : ";
        cin >> choice;

        switch (choice)
        {
        case 0:
            system("exit");
            return;
        case 1:
            library.printAllBooks();
            break;
        case 2:
            library.insertBook(bookToAdd());
            break;
        case 3:
            deleted = library.deleteBook();
            if (deleted)
            {
                cout << "[BOOK DELETED]";
            }
            else
            {
                cout << "[BOOK NOT PRESENT]";
            }
            break;
        case 4:
            searchedBook = library.searchBook();
            if (searchedBook != nullptr)
            {
                cout << "\n[BOOK FOUND]\n";
                cout << "Book Title \t\t\t Book Author \t\t\t Book Genre \t\t\t Book Quantity\n";
                cout << searchedBook->name << "\t\t\t " << searchedBook->author << "\t\t\t " << searchedBook->genre << "\t\t\t\t" << searchedBook->amount << endl;
            }
            else
            {
                cout << "[BOOK NOT PRESENT]";
            }
            break;
        case 5:
            cout << "Enter Genre : ";
            cin >> genre;
            library.printBooksByGenre(genre);
            break;
        case 6:
            cout << "\nStudent Name: ";
            cin.ignore();
            getline(cin, std_name);
            cout << "\nPassword: ";
            getline(cin, pwd);
            stdnt.name = std_name;
            stdnt.password = pwd;
            library.addUser(&stdnt);
            break;
        case 7:
            menu();
            break;
        }
    }
}

// Menu for Student to Issue or Return books.
void student(User *user)
{
    int choice = 0;
    bool issued = false;
    Book *book;
    Book *searchedBook;
    string genre;
    system("cls");
    while (1)
    {
        cout << "\n\n\n++++++++++     M E N U     ++++++++++\n";
        cout << "Welcome " << user->name << endl
             << endl;
        cout << "0. Exit\n";
        cout << "1. Display All Books\n";
        cout << "2. Issue a Book\n";
        cout << "3. Return a Book\n";
        cout << "4. Show all Issued Books\n";
        cout << "5. Search Book\n";
        cout << "6. Show Books by Genre\n";
        cout << "7. LogOut\n";

        cin >> choice;
        switch (choice)
        {
        case 0:
            system("exit");
            return;
        case 1:
            library.printAllBooks();
            break;

        case 2:
            book = library.searchBook();
            if (book != nullptr)
            {
                issued = library.issueBook(user, book);
                if (issued)
                {
                    cout << "\n[ISSUED]";
                }
                else
                    cout << "[NOT ISSUED]";
            }
            else
            {
                cout << "[NOT ISSUED]";
            }
            break;
        case 3:
            book = library.searchBook();
            if (book != nullptr)
            {
                issued = library.returnBook(user, book);
                if (issued)
                {
                    cout << "[RETURNED]";
                }
                else
                    cout << "[NOT RETURNED]";
            }
            break;

        case 4:
            library.printUserBooks(user);
            break;
        case 5:
            searchedBook = library.searchBook();
            if (searchedBook != nullptr)
            {
                cout << "\n[BOOK FOUND]\n";
                cout << "Book Title \t\t\t Book Author \t\t\t Book Genre \t\t\t Book Quantity\n";
                cout << searchedBook->name << "\t\t\t " << searchedBook->author << "\t\t\t " << searchedBook->genre << "\t\t\t\t" << searchedBook->amount << endl;
            }
            else
            {
                cout << "[BOOK NOT PRESENT]";
            }
            break;
        case 6:
            cout << "Enter Genre : ";
            cin >> genre;
            library.printBooksByGenre(genre);
            break;
        case 7:
            menu();
            break;
        }
    }
}

// Prompting User to login to the system. As Student or as librarian.
// Using _getch() function to hide the password as it is typed.
User *login(int choice)
{
    string id, pass;
    char ch;
    if (choice == 1)
    {
        for (int i = 1; i < 4; i++)
        {
            system("cls");
            cout << "\t\t\t-----------------------------------------------------" << endl;
            cout << "\t\t\t|                                                   |" << endl;
            cout << "\t\t\t|                    LogIn as Student               |" << endl;
            cout << "\t\t\t|                                                   |" << endl;
            cout << "\t\t\t-----------------------------------------------------" << endl;
            cout << "\t\t\t            Username : ";
            // taking input
            cin >> id;
            cout << "\t\t\t            Password : ";
            ch = _getch();
            // checking for password size//It should not be greater than 13 characrters
            while (ch != 13)
            {
                // inputing entered password into pass string
                pass.push_back(ch);
                // Printing * to hide what user types
                cout << '*';
                ch = _getch();
            }

            bool found = false;
            User *user = library.searchUser(id);
            if (user != nullptr && user->name == id && user->password == pass)
            {
                found = true;
            }

            if (found)
            {
                system("CLS");
                cout << "\n\t\t\t            Access granted." << endl;
                _getch();
                return user;
            }
            else
            {
                cout << "\n\t\t\t            Error : Invalid username or password." << endl;
                cout << "\t\t\t            You have " << 3 - i << " tries left." << endl;
                // clearing id and password to take input again
                id.clear();
                pass.clear();
                _getch();
            }
        }
    }
    else
    {
        for (int i = 1; i < 4; i++)
        {
            system("cls");
            cout << "\t\t\t-----------------------------------------------------" << endl;
            cout << "\t\t\t|                                                   |" << endl;
            cout << "\t\t\t|                    LogIn as Librarian             |" << endl;
            cout << "\t\t\t|                                                   |" << endl;
            cout << "\t\t\t-----------------------------------------------------" << endl;
            cout << "\t\t\t            Username : ";
            // taking input
            cin >> id;
            cout << "\t\t\t            Password : ";
            ch = _getch();
            // checking for password size//It should not be greater than 13 characrters
            while (ch != 13)
            {
                pass.push_back(ch);
                // Printing * to hide what user types
                cout << '*';
                ch = _getch();
            }

            bool found = false;
            User *user = library.searchUser(id);
            if (user != nullptr && user->name == id && user->password == pass)
            {
                found = true;
            }

            if (found)
            {
                system("CLS");
                cout << "\n\t\t\t            Access granted." << endl;
                _getch();

                return user;
            }
            else
            {
                cout << "\n\t\t\t            Error : Invalid username or password." << endl;
                cout << "\t\t\t            You have " << 3 - i << " tries left." << endl;
                _getch();
                // clearing id and password to take input again
                id.clear();
                pass.clear();
            }
        }
    }
    return nullptr;
}

// Main Menu that appears when program boots.
void menu()
{
    int choose;
    User *user;
    while (1)
    {
        system("cls");
        cout << "-------------------------------------------------------------------------------------------------------------" << endl;
        cout << "|                                                                                                           |" << endl;
        cout << "|                   - W E L C O M E   T O   T H E   LIBRARY -                                               |" << endl;
        cout << "|                                                                                                           |" << endl;
        cout << "-------------------------------------------------------------------------------------------------------------" << endl
             << endl;

        cout << "\n\nPlease Choose an appropriate option : ";
        cout << "\n=======================================";

        cout << "\n\n0. Exit";
        cout << "\n\n1. LogIn as Librarian";
        cout << "\n\n2. LogIn as Student";
        cout << "\n\nPlease Pick any one option: ";
        cin >> choose;

        switch (choose)
        {
        case 0:
            system("cls");
            system("exit");
            return;

        case 1:
            system("cls");
            user = login(2);
            if (user != nullptr)
            {
                librarian(user);
                return;
            }
            break;

        case 2:
            system("cls");
            user = login(1);
            if (user != nullptr)
            {
                student(user);
                return;
            }
            break;
        }
    }
}

// Generates books
// Only for testing purposes
void generateData(int numBooks)
{
    string title, author, genre;
    int j = 1;
    for (int i = 0; i < numBooks / 2; i++)
    {
        title = "Harry Potter" + to_string(i + 1);
        author = "JK Rowling";

        if (i % 2 == 0)
            genre = "Fiction";
        else
            genre = "Horror";

        Book *newBook = new Book(title, author, genre, 5);
        library.insertBook(newBook);
    }

    for (int i = numBooks / 2; i < numBooks; i++)
    {
        title = "The Stand" + to_string(j);
        author = "Stephen King";

        if (i % 2 == 0)
            genre = "Novel";
        else
            genre = "Drama";

        Book *Book2 = new Book(title, author, genre, 5);
        library.insertBook(Book2);

        j++;
    }

    Book *book = new Book("The Lord Of Rings", "JRR Tolkien", "Fiction", 5);
    library.insertBook(book);
}

int main()
{
    generateData(200);
    // Adding users to work initially
    User user1("Isha", "user");
    // Librarian
    User librarian("admin", "admin");
    library.addUser(&user1);
    library.addUser(&librarian);
    menu();
}