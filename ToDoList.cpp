#include <bits/stdc++.h>
#include <unordered_set>
#include <limits>
using namespace std;

#define RESET   "\033[0m"    // Reset color
#define BLACK   "\033[30m"   // Black
#define RED     "\033[31m"   // Red
#define GREEN   "\033[32m"   // Green
#define YELLOW  "\033[33m"   // Yellow
#define BLUE    "\033[34m"   // Blue
#define MAGENTA "\033[35m"   // Magenta
#define CYAN    "\033[36m"   // Cyan
#define WHITE   "\033[37m"   // White
#define GRAY    "\033[90m"   // Gray

struct Date {
    int year, month, day, hour;
};

struct Task {
    Date deadline;
    bool done;
    string taskname;
};

class ToDoList {
private:
    vector<Task> tasks;
    std::unordered_set<string> added_tasks;

public:
    Date getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm* localTime = std::localtime(&currentTime);

        return {1900 + localTime->tm_year, 1 + localTime->tm_mon, localTime->tm_mday, localTime->tm_hour};
    }

    bool isValidDate(const Date& date) {
        Date currentDate = getCurrentDate();
        if (date.year < currentDate.year ||
            (date.year == currentDate.year && date.month < currentDate.month) ||
            (date.year == currentDate.year && date.month == currentDate.month && date.day < currentDate.day) ||
            (date.year == currentDate.year && date.month == currentDate.month && date.day == currentDate.day && date.hour < currentDate.hour) ||
            date.month > 12 || date.month < 1 || date.day > 31 || date.day < 1 || date.hour > 23 || date.hour < 0) {
            cout << "ENTER VALID DATE" << endl;
            return false;
        }
        return true;
    }

    static bool sortTasks(const Task& first, const Task& second) {
        if (first.deadline.year != second.deadline.year) return first.deadline.year < second.deadline.year;
        if (first.deadline.month != second.deadline.month) return first.deadline.month < second.deadline.month;
        if (first.deadline.day != second.deadline.day) return first.deadline.day < second.deadline.day;
        return first.deadline.hour < second.deadline.hour;
    }

    void addTask() {
        cin.ignore();
        Task current;

        cout << "ENTER THE NAME OF THE TASK: ";
        getline(cin, current.taskname);
        current.taskname = trim(current.taskname);
        if (added_tasks.find(current.taskname) != added_tasks.end()) {
            cout << "Task already added" << endl;
            return;
        }
        cout << "ENTER THE DEADLINE OF THE TASK (Hour Day Month Year). ";
        cin >> current.deadline.hour >> current.deadline.day >> current.deadline.month >> current.deadline.year;

        if (isValidDate(current.deadline)) {
            current.done = false;
            tasks.push_back(current);
            added_tasks.insert(current.taskname);
        }
    }

    void printTasks() {
        Date currentDate = getCurrentDate();
        for (auto& this_task : tasks) {
            printTaskDetails(this_task, currentDate);
        }
    }

    void printUncompletedTasks() {
        sort(tasks.begin(), tasks.end(), sortTasks);
        Date currentDate = getCurrentDate();
        bool flag = true;
        for (auto& this_task : tasks) {
            if (!this_task.done) {
                printTaskDetails(this_task, currentDate);
                flag = false;
            }
        }
        if(flag){
            cout << "No uncompleted Tasks" <<endl;
        }
    }

    void markTaskAsDone(const string& taskname) {
        string trimmedTaskName = trim(taskname);
        for (auto& this_task : tasks) {
            if (trimmedTaskName == trim(this_task.taskname)) {
                this_task.done = true;
                cout << "Task \"" << trimmedTaskName << "\" marked as done." << endl;
                return;
            }
        }
        cout << "Task \"" << trimmedTaskName << "\" does not exist." << endl;
    }

    pair<int, int> calculateTimeLeft(const Date& deadline, const Date& currentDate) {
        std::tm deadline_tm = {0, 0, deadline.hour, deadline.day, deadline.month - 1, deadline.year - 1900};
        std::tm current_tm = {0, 0, currentDate.hour, currentDate.day, currentDate.month - 1, currentDate.year - 1900};

        std::time_t deadline_time = std::mktime(&deadline_tm);
        std::time_t current_time = std::mktime(&current_tm);

        if (deadline_time == -1 || current_time == -1) {
            return std::make_pair(0, 0); // Return a default value in case of an error
        }

        auto diff = std::difftime(deadline_time, current_time);
        int daysLeft = static_cast<int>(diff / (60 * 60 * 24));
        int hoursLeft = static_cast<int>((diff - (daysLeft * 24 * 60 * 60)) / 3600);

        return std::make_pair(hoursLeft, daysLeft);
    }

    void print_menu() {
        cout << RED << "                To-Do List" << RESET << endl;
        cout << "Enter your choice:" << endl;
        cout << "1. Add Task" << endl;
        cout << "2. Print Tasks" << endl;
        cout << "3. Mark Task as Done" << endl;
        cout << "4. Print Uncompleted Tasks" << endl;
        cout << "5. Save List" << endl;
        cout << "6. Load List" << endl;
        cout << "7. Exit" << endl;
    }

    void save_list() {
        string playerName;
        cout << "Enter the player's name to save the list: ";
        cin.ignore(); // Ignore any leftover newline character
        getline(cin, playerName);

        // Replace spaces in playerName with underscores to avoid issues with file names
        replace(playerName.begin(), playerName.end(), ' ', '_');
        string filename = playerName + "_todolist.txt";

        // Check if the file name already exists
        ifstream namesFile("saved_filenames.txt");
        string existingFilename;
        while (getline(namesFile, existingFilename)) {
            if (existingFilename == filename) {
                cout << "A file for this player already exists. Do you want to overwrite it? (yes/no): ";
                string response;
                getline(cin, response);
                if (response != "yes") {
                    return;
                }
                break;
            }
        }
        namesFile.close();

        // Save the task list to the new file
        ofstream outFile(filename);
        if (!outFile) {
            cout << "Error opening file for writing." << endl;
            return;
        }

        // Save tasks
        for (const auto& task : tasks) {
            outFile << task.deadline.year << " "
                    << task.deadline.month << " "
                    << task.deadline.day << " "
                    << task.deadline.hour << " "
                    << task.done << " "
                    << task.taskname << endl;
        }

        // Save added tasks
        outFile << "---" << endl; // Delimiter to separate tasks and added_tasks
        for (const auto& taskName : added_tasks) {
            outFile << taskName << endl;
        }
        outFile.close();

        // Save the new file name to the list of saved file names
        ofstream namesFileOut("saved_filenames.txt", ios::app);
        namesFileOut << filename << endl;
        namesFileOut.close();

        cout << "List saved to " << filename << endl;
    }

  void load_list() {
    // Read the list of saved file names
    ifstream namesFile("saved_filenames.txt");
    if (!namesFile) {
        cout << "No saved lists found." << endl;
        return;
    }

    vector<string> filenames;
    string filename;
    while (getline(namesFile, filename)) {
        filenames.push_back(filename);
    }
    namesFile.close();

    if (filenames.empty()) {
        cout << "No saved lists found." << endl;
        return;
    }

    // Display the list of saved file names
    cout << "Saved lists:" << endl;
    for (size_t i = 0; i < filenames.size(); ++i) {
        cout << i + 1 << ". " << filenames[i] << endl;
    }

    // Prompt the user to select a file
    cout << "Enter the number or the name of the list to load: ";
    string input;
    cin.ignore(); // Ignore any leftover newline character
    getline(cin, input);

    // Check if the input is a number
    bool isNumber = all_of(input.begin(), input.end(), ::isdigit);
    if (isNumber) {
        int choice = 0;
        try {
            choice = stoi(input);
        } catch (const std::invalid_argument& e) {
            cout << "Invalid input. Please enter a valid number or name." << endl;
            return;
        } catch (const std::out_of_range& e) {
            cout << "Number out of range. Please enter a valid number." << endl;
            return;
        }

        if (choice < 1 || choice > filenames.size()) {
            cout << "Invalid choice. Please enter a valid number." << endl;
            return;
        }
        filename = filenames[choice - 1];
    } else {
        // Check if the input matches any filename
        auto it = find(filenames.begin(), filenames.end(), input);
        if (it == filenames.end()) {
            cout << "Invalid choice. Please enter a valid name." << endl;
            return;
        }
        filename = *it;
    }

    ifstream inFile(filename);
    if (!inFile) {
        cout << "Error opening file for reading." << endl;
        return;
    }

    tasks.clear();
    added_tasks.clear();

    Task task;
    string line;
    while (getline(inFile, line) && line != "---") {
        istringstream iss(line);
        if (!(iss >> task.deadline.year
                  >> task.deadline.month
                  >> task.deadline.day
                  >> task.deadline.hour
                  >> task.done)) {
            continue;
        }
        getline(iss, task.taskname);
        task.taskname = trim(task.taskname); // Ensure trimming
        tasks.push_back(task);
    }

    while (getline(inFile, line)) {
        added_tasks.insert(trim(line)); // Ensure trimming
    }

    inFile.close();
    cout << "List loaded from " << filename << endl;
}



    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

private:
    void printTaskDetails(const Task& task, const Date& currentDate) {
        cout << "Task Name: " << task.taskname << endl;
        cout << "Deadline: " << (task.deadline.hour > 12 ? task.deadline.hour - 12 : task.deadline.hour)
             << (task.deadline.hour >= 12 ? " PM" : " AM") << " AT "
             << task.deadline.day << "/"
             << task.deadline.month << "/"
             << task.deadline.year << endl;
        cout << "Done: " << (task.done ? "Yes" : "No") << endl;

        auto TimeLeft = calculateTimeLeft(task.deadline, currentDate);
        if (TimeLeft.second > 0) {
            cout << "Days left: " << TimeLeft.second << " at "
                 << (task.deadline.hour > 12 ? task.deadline.hour - 12 : task.deadline.hour)
                 << (task.deadline.hour >= 12 ? " PM" : " AM") << endl;
        } else if (TimeLeft.second == 0 && TimeLeft.first > 0) {
            cout << TimeLeft.first << " Hours Left" << endl;
        } else {
            cout << "TIME OUT FOR THIS TASK!!" << endl;
        }
        cout << "----------------------" << endl;
    }

    string trim(const string& str) {
        size_t first = str.find_first_not_of(' ');
        if (first == string::npos) return "";
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, last - first + 1);
    }
};

int main() {
    ToDoList myList;
    int choice;

    while (true) {
        cin.clear();
        myList.print_menu();

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a valid number." << endl;
            continue;
        }

        myList.clearScreen();

        switch (choice) {
            case 1:
                myList.addTask();
                break;
            case 2:
                myList.printTasks();
                break;
            case 3: {
                cin.ignore();
                string taskname;
                cout << "Finished Task name: ";
                getline(cin, taskname);
                myList.markTaskAsDone(taskname);
                break;
            }
            case 4:
                myList.printUncompletedTasks();
                break;
            case 5:
                myList.save_list();
                break;
            case 6:
                myList.load_list();
                break;
            case 7:
                return 0;
            default:
                cout << "Invalid choice. Please enter a valid choice." << endl;
                break;
        }
    }
}
