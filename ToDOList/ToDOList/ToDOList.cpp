// ToDOList.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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

// Структура для хранения информации о задаче
struct Task {
    int id;
    string title;         // Название
    string description;   // Описание
    string category;      // Категория
    string created_at;    // Дата создания
    bool completed;       // Выполнено
    int priority;         // Приоритет (1-низкий, 2-средний, 3-высокий)
};

// Прототипы функций
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

// Коды цветов для терминала
const int COLOR_RED = 31;
const int COLOR_GREEN = 32;
const int COLOR_YELLOW = 33;
const int COLOR_BLUE = 34;
const int COLOR_MAGENTA = 35;
const int COLOR_CYAN = 36;

int main() {

    setlocale(LC_ALL, "Russian");
    #ifdef _WIN32
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    #endif

    vector<Task> tasks;
    const string filename = "tasks.txt";

    loadTasks(tasks, filename);

    int choice;
    do {
        clearScreen();
        showMenu();
        showTasks(tasks);

        cout << "\nВведите номер действия (1-8): ";
        cin >> choice;

        // Очистка буфера ввода
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
            cout << "Задачи успешно сохранены!\n";
            waitForEnter();
            break;
        case 7:
            loadTasks(tasks, filename);
            cout << "Задачи загружены из файла!\n";
            waitForEnter();
            break;
        case 8:
            cout << "Выход...\n";
            break;
        default:
            cout << "Неверный выбор. Попробуйте снова.\n";
            waitForEnter();
        }
    } while (choice != 8);

    // Сохранение перед выходом
    saveTasks(tasks, filename);

    return 0;
}

void showMenu() {
    printColor("\n=== Менеджер задач ===\n", COLOR_CYAN);
    cout << "1. Добавить задачу\n";
    cout << "2. Отметить как выполненную\n";
    cout << "3. Удалить задачу\n";
    cout << "4. Поиск задач\n";
    cout << "5. Сортировка задач\n";
    cout << "6. Сохранить задачи\n";
    cout << "7. Загрузить задачи\n";
    cout << "8. Выход\n";
}

void addTask(vector<Task>& tasks) {
    clearScreen();
    printColor("=== Новая задача ===\n", COLOR_GREEN);

    Task newTask;
    newTask.id = tasks.empty() ? 1 : tasks.back().id + 1;

    cout << "Введите название задачи: ";
    getline(cin, newTask.title);

    // Проверка пустого названия
    while (newTask.title.empty()) {
        cout << "Название не может быть пустым. Введите название: ";
        getline(cin, newTask.title);
    }

    cout << "Введите описание (необязательно): ";
    getline(cin, newTask.description);

    cout << "Введите категорию (работа/учеба/личное/другое): ";
    getline(cin, newTask.category);
    if (newTask.category.empty()) {
        newTask.category = "другое";
    }

    cout << "Укажите приоритет (1-низкий, 2-средний, 3-высокий): ";
    string priorityInput;
    getline(cin, priorityInput);
    try {
        newTask.priority = stoi(priorityInput);
        if (newTask.priority < 1 || newTask.priority > 3) {
            newTask.priority = 2; // По умолчанию средний
        }
    }
    catch (...) {
        newTask.priority = 2;
    }

    newTask.created_at = getCurrentDateTime();
    newTask.completed = false;

    tasks.push_back(newTask);

    cout << "Задача успешно добавлена!\n";
    waitForEnter();
}

void showTasks(const vector<Task>& tasks) {
    if (tasks.empty()) {
        printColor("\nСписок задач пуст.\n", COLOR_YELLOW);
        return;
    }

    printColor("\n=== Список задач ===\n", COLOR_BLUE);
    for (const auto& task : tasks) {
        cout << task.id << ". ";
        cout << "[" << (task.completed ? "+" : "-") << "] ";

        // Цвет в зависимости от статуса
        if (task.completed) {
            printColor(task.title, COLOR_GREEN);
        }
        else {
            // Цвет по приоритету
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
        cout << "[Приоритет: ";
        if (task.priority == 3) {
            printColor("Высокий", COLOR_RED);
        }
        else if (task.priority == 2) {
            printColor("Средний", COLOR_YELLOW);
        }
        else {
            cout << "Низкий";
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

    cout << "\nВведите ID задачи для отметки (0 для отмены): ";
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
        cout << "Задача отмечена как выполненная!\n";
    }
    else {
        cout << "Задача не найдена.\n";
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

    cout << "\nВведите ID задачи для удаления (0 для отмены): ";
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
        cout << "Задача успешно удалена!\n";
    }
    else {
        cout << "Задача не найдена.\n";
    }

    cin.ignore();
    waitForEnter();
}

void saveTasks(const vector<Task>& tasks, const string& filename) {
    ofstream outFile(filename);

    if (!outFile) {
        cerr << "Ошибка открытия файла для записи.\n";
        return;
    }

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
        // Файл ещё не существует
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
    printColor("=== Поиск задач ===\n", COLOR_MAGENTA);

    if (tasks.empty()) {
        cout << "Нет задач для поиска.\n";
        waitForEnter();
        return;
    }

    cout << "Введите поисковый запрос (название, описание или категория): ";
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
    printColor("=== Результаты поиска ===\n", COLOR_MAGENTA);
    if (results.empty()) {
        cout << "Задачи не найдены.\n";
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
    printColor("=== Сортировка задач ===\n", COLOR_YELLOW);

    if (tasks.empty()) {
        cout << "Нет задач для сортировки.\n";
        waitForEnter();
        return;
    }

    cout << "1. По дате (новые сначала)\n";
    cout << "2. По дате (старые сначала)\n";
    cout << "3. По приоритету (высокий -> низкий)\n";
    cout << "4. По приоритету (низкий -> высокий)\n";
    cout << "5. По статусу выполнения\n";
    cout << "6. Отмена\n";

    cout << "Выберите вариант сортировки (1-6): ";
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
        cout << "Неверный выбор.\n";
        waitForEnter();
        return;
    }

    cout << "Задачи успешно отсортированы!\n";
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
    cout << "\nНажмите Enter для продолжения...";
    cin.ignore((numeric_limits<streamsize>::max)(), '\n');
}

void printColor(const string& text, int colorCode) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Преобразуем ANSI-код цвета в Windows-код
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

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
