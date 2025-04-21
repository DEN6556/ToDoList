// ToDOList.cpp : ���� ���� �������� ������� "main". ����� ���������� � ������������� ���������� ���������.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <limits>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ��������� ��� �������� ���������� � ������
struct Task {
    int id;
    string title;         // ��������
    string description;   // ��������
    string category;      // ���������
    string created_at;    // ���� ��������
    bool completed;       // ���������
    int priority;         // ��������� (1-������, 2-�������, 3-�������)
};

// ��������� �������
void showMenu();
void addTask(vector<Task>& tasks);
void showTasks(const vector<Task>& tasks);
void markTaskComplete(vector<Task>& tasks);
void deleteTask(vector<Task>& tasks);
void saveTasks(const vector<Task>& tasks, const string& filename);
void loadTasks(vector<Task>& tasks, const string& filename);
string getCurrentDateTime();
void searchTasks(const vector<Task>& tasks);
void sortTasks(vector<Task>& tasks);
void clearScreen();
void waitForEnter();
void printColor(const string& text, int colorCode);

// ���� ������ ��� ���������
const int COLOR_RED = 31;
const int COLOR_GREEN = 32;
const int COLOR_YELLOW = 33;
const int COLOR_BLUE = 34;
const int COLOR_MAGENTA = 35;
const int COLOR_CYAN = 36;

int main() {

#ifdef _WIN32
    system("chcp 1251 > nul");  // ������������� ������� �������� Windows-1251
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
#endif

    setlocale(LC_ALL, "Russian");  // ���������� ������� ������

    vector<Task> tasks;
    const string filename = "tasks.txt";

    loadTasks(tasks, filename);

    int choice;
    do {
        clearScreen();
        showMenu();
        showTasks(tasks);

        cout << "\n������� ����� �������� (1-8): ";
        cin >> choice;

        // ������� ������ �����
        cin.ignore((numeric_limits<streamsize>::max)(), '\n');

        switch (choice) {
        case 1:
            addTask(tasks);
            break;
        case 2:
            markTaskComplete(tasks);
            break;
        case 3:
            deleteTask(tasks);
            break;
        case 4:
            searchTasks(tasks);
            break;
        case 5:
            sortTasks(tasks);
            break;
        case 6:
            saveTasks(tasks, filename);
            cout << "������ ������� ���������!\n";
            waitForEnter();
            break;
        case 7:
            loadTasks(tasks, filename);
            cout << "������ ��������� �� �����!\n";
            waitForEnter();
            break;
        case 8:
            cout << "�����...\n";
            break;
        default:
            cout << "�������� �����. ���������� �����.\n";
            waitForEnter();
        }
    } while (choice != 8);

    // ���������� ����� �������
    saveTasks(tasks, filename);

    return 0;

}

void showMenu() {
    printColor("\n=== �������� ����� ===\n", COLOR_CYAN);
    cout << "1. �������� ������\n";
    cout << "2. �������� ��� �����������\n";
    cout << "3. ������� ������\n";
    cout << "4. ����� �����\n";
    cout << "5. ���������� �����\n";
    cout << "6. ��������� ������\n";
    cout << "7. ��������� ������\n";
    cout << "8. �����\n";
}

void addTask(vector<Task>& tasks) {
    clearScreen();
    printColor("=== ����� ������ ===\n", COLOR_GREEN);

    Task newTask;
    newTask.id = tasks.empty() ? 1 : tasks.back().id + 1;

    cout << "������� �������� ������: ";
    getline(cin, newTask.title);

    // �������� ������� ��������
    while (newTask.title.empty()) {
        cout << "�������� �� ����� ���� ������. ������� ��������: ";
        getline(cin, newTask.title);
    }

    cout << "������� �������� (�������������): ";
    getline(cin, newTask.description);

    cout << "������� ��������� (������/�����/������/������): ";
    getline(cin, newTask.category);
    if (newTask.category.empty()) {
        newTask.category = "������";
    }

    cout << "������� ��������� (1-������, 2-�������, 3-�������): ";
    string priorityInput;
    getline(cin, priorityInput);
    try {
        newTask.priority = stoi(priorityInput);
        if (newTask.priority < 1 || newTask.priority > 3) {
            newTask.priority = 2; // �� ��������� �������
        }
    }
    catch (...) {
        newTask.priority = 2;
    }

    newTask.created_at = getCurrentDateTime();
    newTask.completed = false;

    tasks.push_back(newTask);

    cout << "������ ������� ���������!\n";
    waitForEnter();
}

void showTasks(const vector<Task>& tasks) {
    if (tasks.empty()) {
        printColor("\n������ ����� ����.\n", COLOR_YELLOW);
        return;
    }

    printColor("\n=== ������ ����� ===\n", COLOR_BLUE);
    for (const auto& task : tasks) {
        cout << task.id << ". ";
        cout << "[" << (task.completed ? "+" : "-") << "] ";

        // ���� � ����������� �� �������
        if (task.completed) {
            printColor(task.title, COLOR_GREEN);
        }
        else {
            // ���� �� ����������
            if (task.priority == 3) {
                printColor(task.title, COLOR_RED);
            }
            else if (task.priority == 2) {
                printColor(task.title, COLOR_YELLOW);
            }
            else {
                cout << task.title;
            }
        }

        cout << " (" << task.category << ") ";
        cout << "[���������: ";
        if (task.priority == 3) {
            printColor("�������", COLOR_RED);
        }
        else if (task.priority == 2) {
            printColor("�������", COLOR_YELLOW);
        }
        else {
            cout << "������";
        }
        cout << "] ";

        cout << " - " << task.created_at;

        if (!task.description.empty()) {
            cout << "\n   " << task.description;
        }
        cout << "\n";
    }
}

void markTaskComplete(vector<Task>& tasks) {
    clearScreen();
    showTasks(tasks);

    if (tasks.empty()) {
        waitForEnter();
        return;
    }

    cout << "\n������� ID ������ ��� ������� (0 ��� ������): ";
    int taskId;
    cin >> taskId;

    if (taskId == 0) {
        cin.ignore();
        return;
    }

    auto it = find_if(tasks.begin(), tasks.end(), [taskId](const Task& t) {
        return t.id == taskId;
        });

    if (it != tasks.end()) {
        it->completed = true;
        cout << "������ �������� ��� �����������!\n";
    }
    else {
        cout << "������ �� �������.\n";
    }

    cin.ignore();
    waitForEnter();
}

void deleteTask(vector<Task>& tasks) {
    clearScreen();
    showTasks(tasks);

    if (tasks.empty()) {
        waitForEnter();
        return;
    }

    cout << "\n������� ID ������ ��� �������� (0 ��� ������): ";
    int taskId;
    cin >> taskId;

    if (taskId == 0) {
        cin.ignore();
        return;
    }

    auto it = find_if(tasks.begin(), tasks.end(), [taskId](const Task& t) {
        return t.id == taskId;
        });

    if (it != tasks.end()) {
        tasks.erase(it);
        cout << "������ ������� �������!\n";
    }
    else {
        cout << "������ �� �������.\n";
    }

    cin.ignore();
    waitForEnter();
}

void saveTasks(const vector<Task>& tasks, const string& filename) {
    ofstream outFile(filename, ios::binary);  // ��������� � �������� ������

    if (!outFile) {
        cerr << "������ �������� ����� ��� ������.\n";
        return;
    }

    // ��������� BOM ��� Windows-1251
    outFile.put(0xEF);
    outFile.put(0xBB);
    outFile.put(0xBF);

    for (const auto& task : tasks) {
        outFile << task.id << "|"
            << task.title << "|"
            << task.description << "|"
            << task.category << "|"
            << task.created_at << "|"
            << task.completed << "|"
            << task.priority << "\n";
    }
    outFile.close();
}

void loadTasks(vector<Task>& tasks, const string& filename) {
    ifstream inFile(filename);
    tasks.clear();

    if (!inFile) {
        // ���� ��� �� ����������
        return;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        Task task;
        stringstream ss(line);
        string field;

        getline(ss, field, '|');
        task.id = stoi(field);

        getline(ss, task.title, '|');
        getline(ss, task.description, '|');
        getline(ss, task.category, '|');
        getline(ss, task.created_at, '|');

        getline(ss, field, '|');
        task.completed = (field == "1");

        getline(ss, field);
        task.priority = stoi(field);

        tasks.push_back(task);
    }

    inFile.close();
}

string getCurrentDateTime() {
    time_t now = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &now);

    stringstream ss;
    ss << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void searchTasks(const vector<Task>& tasks) {
    clearScreen();
    printColor("=== ����� ����� ===\n", COLOR_MAGENTA);

    if (tasks.empty()) {
        cout << "��� ����� ��� ������.\n";
        waitForEnter();
        return;
    }

    cout << "������� ��������� ������ (��������, �������� ��� ���������): ";
    string query;
    getline(cin, query);

    transform(query.begin(), query.end(), query.begin(), ::tolower);

    vector<Task> results;
    for (const auto& task : tasks) {
        string title = task.title;
        string desc = task.description;
        string cat = task.category;

        transform(title.begin(), title.end(), title.begin(), ::tolower);
        transform(desc.begin(), desc.end(), desc.begin(), ::tolower);
        transform(cat.begin(), cat.end(), cat.begin(), ::tolower);

        if (title.find(query) != string::npos ||
            desc.find(query) != string::npos ||
            cat.find(query) != string::npos) {
            results.push_back(task);
        }
    }

    clearScreen();
    printColor("=== ���������� ������ ===\n", COLOR_MAGENTA);
    if (results.empty()) {
        cout << "������ �� �������.\n";
    }
    else {
        for (const auto& task : results) {
            cout << task.id << ". [" << (task.completed ? "+" : "-") << "] "
                << task.title << " (" << task.category << ") - "
                << task.created_at << "\n";
        }
    }

    waitForEnter();
}

void sortTasks(vector<Task>& tasks) {
    clearScreen();
    printColor("=== ���������� ����� ===\n", COLOR_YELLOW);

    if (tasks.empty()) {
        cout << "��� ����� ��� ����������.\n";
        waitForEnter();
        return;
    }

    cout << "1. �� ���� (����� �������)\n";
    cout << "2. �� ���� (������ �������)\n";
    cout << "3. �� ���������� (������� -> ������)\n";
    cout << "4. �� ���������� (������ -> �������)\n";
    cout << "5. �� ������� ����������\n";
    cout << "6. ������\n";

    cout << "�������� ������� ���������� (1-6): ";
    int choice;
    cin >> choice;
    cin.ignore();

    switch (choice) {
    case 1:
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.created_at > b.created_at;
            });
        break;
    case 2:
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.created_at < b.created_at;
            });
        break;
    case 3:
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.priority > b.priority;
            });
        break;
    case 4:
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.priority < b.priority;
            });
        break;
    case 5:
        sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) {
            return a.completed && !b.completed;
            });
        break;
    case 6:
        return;
    default:
        cout << "�������� �����.\n";
        waitForEnter();
        return;
    }

    cout << "������ ������� �������������!\n";
    waitForEnter();
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void waitForEnter() {
    cout << "\n������� Enter ��� �����������...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
}

void printColor(const string& text, int colorCode) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // ����������� ANSI-��� ����� � Windows-���
    WORD winColor;
    switch (colorCode) {
    case 31: winColor = FOREGROUND_RED; break;
    case 32: winColor = FOREGROUND_GREEN; break;
    case 33: winColor = FOREGROUND_RED | FOREGROUND_GREEN; break;
    case 34: winColor = FOREGROUND_BLUE; break;
    case 35: winColor = FOREGROUND_RED | FOREGROUND_BLUE; break;
    case 36: winColor = FOREGROUND_GREEN | FOREGROUND_BLUE; break;
    default: winColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
    SetConsoleTextAttribute(hConsole, winColor);
    cout << text;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    #else
    cout << "\033[1;" << colorCode << "m" << text << "\033[0m";
    #endif
}

// ������ ���������: CTRL+F5 ��� ���� "�������" > "������ ��� �������"
// ������� ���������: F5 ��� ���� "�������" > "��������� �������"

// ������ �� ������ ������ 
//   1. � ���� ������������ ������� ����� ��������� ����� � ��������� ���.
//   2. � ���� Team Explorer ����� ������������ � ������� ���������� ��������.
//   3. � ���� "�������� ������" ����� ������������� �������� ������ ������ � ������ ���������.
//   4. � ���� "������ ������" ����� ������������� ������.
//   5. ��������������� �������� ������ ���� "������" > "�������� ����� �������", ����� ������� ����� ����, ��� "������" > "�������� ������������ �������", ����� �������� � ������ ������������ ����� ����.
//   6. ����� ����� ������� ���� ������ �����, �������� ������ ���� "����" > "�������" > "������" � �������� SLN-����.
