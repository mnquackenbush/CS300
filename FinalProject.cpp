//============================================================================
// Name        : FinalProject.cpp
// Author      : Mark Quackenbush
// Version     : 1.0
// Copyright   : Copyright © 2023 SNHU COCE
// Description : Project 7-1 Project 2
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold course information
struct Course {
    string courseNumber; // unique identifier
    string courseName;
    vector<string> prerequisites;
};

// Define a Node structure to hold courses
struct Node {
    Course course;
    unsigned int key;
    Node* next;

    // default constructor
    Node() {
        key = UINT_MAX;
        next = nullptr;
    }

    // initialize with a course
    Node(Course aCourse) : Node() {
        course = aCourse;
    }

    // initialize with a course and a key
    Node(Course aCourse, unsigned int aKey) : Node(aCourse) {
        key = aKey;
    }
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:

    vector<Node> nodes;

    unsigned int tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAlphanumerically();
    Course Search(string courseNumber);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    // Initalize node structure by resizing tableSize
    nodes.resize(tableSize);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->
    // resize nodes size
    this->tableSize = size;
    nodes.resize(size);
}


/**
 * Destructor
 */
HashTable::~HashTable() {
    // erase nodes beginning
    nodes.erase(nodes.begin());
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    // return key tableSize
    return key % tableSize;
}

/**
 * Insert a course
 *
 * @param course The course to insert
 */
void HashTable::Insert(Course course) {
    // create the key for the given course
    unsigned courseKey = hash(atoi(course.courseNumber.c_str()));
    // retrieve node using key
    Node* oldNode = &(nodes.at(courseKey));
    // if no entry found for the key
    if (oldNode == nullptr) {
        // assign this node to the key position
        Node* newNode = new Node(course, courseKey);
        nodes.insert(nodes.begin() + courseKey, (*newNode));
    }
    // else if node is not used
    else if (oldNode->key == UINT_MAX) {
        // passing old node key to UNIT_MAX, set to key, set old node to course and old node next to null pointer
        oldNode->key = courseKey;
        oldNode->course = course;
        oldNode->next = nullptr;
    }
    // else find the next open node
    else {
        while (oldNode->next != nullptr) {
            // add new newNode to end
            oldNode = oldNode->next;
        }
        oldNode->next = new Node(course, courseKey);
    }

}

/**
 * Print all courses alphanumerically
 */
void HashTable::PrintAlphanumerically() {
    
    // New vector to hold sorted courses in.
    vector<Course> courseList;

    // Gather all courses from hash table (including chained nodes)
    for (auto& node : nodes) {
        if (node.key != UINT_MAX) {
            courseList.push_back(node.course);
            Node* current = node.next;
            while (current != nullptr) {
                courseList.push_back(current->course);
                current = current->next;
            }
        }
    }

    // Sort courses by courseNumber
    sort(courseList.begin(), courseList.end(), [](const Course& a, const Course& b) {
        return a.courseNumber < b.courseNumber;
        });

    // Print sorted courses
    for (const auto& course : courseList) {
        cout << course.courseNumber << ": " << course.courseName << endl;
        cout << "Prerequisites: ";
        if (course.prerequisites.empty()) {
            cout << "None" << endl;
        }
        else {
            for (const auto& pre : course.prerequisites) {
                cout << pre << " ";
            }
            cout << endl;
        }
    }
}


/**
 * Search for the specified courseNumber
 *
 * @param courseNumber The course number to search for
 */
Course HashTable::Search(string courseNumber) {
    Course course;

    // create the key for the given course
    unsigned int matchKey = hash(atoi(courseNumber.c_str()));
    // if entry found for the key
    Node* searchNode = &nodes.at(matchKey);
    if ((searchNode != nullptr) && (searchNode->key != UINT_MAX) && (searchNode->course.courseNumber.compare(courseNumber) == 0)) {
        //return node course
        return searchNode->course;
    }


    // if no entry found for the key
    if ((searchNode == nullptr) || (searchNode->key == UINT_MAX)) {
        // return course
        return course;
    }

    // while node not equal to nullptr
    while (searchNode != nullptr) {
        // if the current node matches, return it
        if ((searchNode->key != UINT_MAX) && (searchNode->course.courseNumber.compare(courseNumber) == 0)) {
            return searchNode->course;
        }
        //node is equal to next node
        searchNode = searchNode->next;
    }



    return course;
}


//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the course information to the console (std::out)
 *
 * @param course struct containing the course info
 */
void displayCourse(Course course) {
    cout << course.courseNumber << ": " << course.courseName << endl;
    cout << "Prerequisites: ";
    for (int i = 0; i < course.prerequisites.size(); i++) {
        cout << course.prerequisites.at(i) << " ";
    }
    cout << endl;
    return;
}

/**
 * Load a CSV file containing courses into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the courses read
 */
void loadCourses(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    ifstream file(csvPath);
    if (file.is_open()) {
        cout << "File loaded." << endl;

        int num;
        string line;
        string word;

        while (getline(file, line)) {

            num = 0;
            Node* node = new Node();
            stringstream str(line);

            while (num < 2) {
                getline(str, word, ',');
                if (num == 0) {
                    node->course.courseNumber = word;
                }
                else {
                    node->course.courseName = word;
                }
                num++;
            }
            while (getline(str, word, ',')) {
                node->course.prerequisites.push_back(word);
            }

            //inserts node into hash table
            hashTable->Insert(node->course);
        }
    }
    //file error if file not found
    else {
        cout << "File error, please try again. " << endl;
        return;
    }

}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // csv file path definition
    string csvPath = "CS 300 ABCU_Advising_Program_Input.csv";

    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the courses
    HashTable* courseTable;

    Course course;
    courseTable = new HashTable();

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Courses" << endl;
        cout << "  2. Display All Courses Alphanumerically" << endl;
        cout << "  3. Find Course and Prerequisites" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:

            // Initialize a timer variable before loading courses
            ticks = clock();

            // Complete the method call to load the courses
            loadCourses(csvPath, courseTable);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            courseTable->PrintAlphanumerically();
            break;

        case 3:
            string courseKey;

            cout << "Please enter Course Number: " << endl;
            cin >> courseKey;

            course = courseTable->Search(courseKey);

            if (!course.courseNumber.empty()) {
                displayCourse(course);
            }
            else {
                cout << "Course Number " << courseKey << " not found." << endl;
            }
            break;
  
        }
        if ((choice <= 0) || (choice >= 4)) {
            if (choice != 9) {
                cout << "Invalid Choice. Please try again." << endl;
            }
        }
        
    }

        cout << "Good bye." << endl;

        return 0;
    
}
