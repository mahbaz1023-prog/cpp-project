#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <ctime>

using namespace std;


class Task {
protected:
    string title;
    string desc;
    bool isDone;
    string deadline;
    int priority; 

public:
   
    Task(string t, string d, string dl, int p) {
        title = t;
        desc = d;
        deadline = dl;
        priority = p;
        isDone = false;
    }

    // Sazande baraye load kardan az file
    Task(string t, string d, string dl, int p, bool done) {
        title = t;
        desc = d;
        deadline = dl;
        priority = p;
        isDone = done;
    }

    virtual ~Task() {} 

 
    virtual void print() {
        cout << "--------------------------------" << endl;
        cout << "Title: " << title;
        if (isDone) cout << " [DONE]";
        else cout << " [NOT DONE]";
        cout << endl;
        
        cout << "Desc: " << desc << endl;
        
        string pStr = "Low";
        if (priority == 2) pStr = "Medium";
        if (priority == 3) pStr = "High";
        
        cout << "Priority: " << pStr << " | Deadline: " << deadline << endl;
    }

   
    bool getStatus() { return isDone; }
    void setStatus(bool s) { isDone = s; }
    string getDeadline() { return deadline; }
    int getPriority() { return priority; }
    
    // Amade sazi string baraye save dar file
    virtual string getDataForFile() {
   
        return title + "|" + desc + "|" + deadline + "|" + to_string(priority) + "|" + (isDone ? "1" : "0");
    }
};


class Todo : public Task {
public:
    Todo(string t, string d, string dl, int p) : Task(t, d, dl, p) {}
    
    Todo(string t, string d, string dl, int p, bool done) : Task(t, d, dl, p, done) {}

    void print() override {
        Task::print();
    }
};

// Tabe baraye moghayese 2 ta task (baraye sort kardan)
bool compareTasks(Task* a, Task* b) {
    // Moratab sazi bar asas Priority (High be Low)
    return a->getPriority() > b->getPriority();
}


class TodoList {
    vector<Task*> tasks; 

public:
  
    ~TodoList() {
        for (int i = 0; i < tasks.size(); i++) {
            delete tasks[i];
        }
        tasks.clear();
    }

    void addTask() {
        string t, d, dl;
        int p;
        
        cout << "\n--- Add New Task ---\n";
        cout << "Enter Title: ";
        cin.ignore();
        getline(cin, t);
        
        cout << "Enter Description: ";
        getline(cin, d);
        
        cout << "Enter Deadline (YYYY-MM-DD): ";
        cin >> dl;
        
        cout << "Enter Priority (1:Low, 2:Medium, 3:High): ";
        cin >> p;
        if (p < 1 || p > 3) p = 1; 
        
        tasks.push_back(new Todo(t, d, dl, p));
        cout << "Task added successfully.\n";
    }

    void showAll() {
        if (tasks.empty()) {
            cout << "\nList is empty.\n";
            return;
        }
        for (int i = 0; i < tasks.size(); i++) {
            cout << "\nTask ID: " << i + 1;
            tasks[i]->print();
        }
    }

    void showDone(bool status) {
        bool found = false;
        for (int i = 0; i < tasks.size(); i++) {
            if (tasks[i]->getStatus() == status) {
                cout << "\nTask ID: " << i + 1;
                tasks[i]->print();
                found = true;
            }
        }
        if (!found) cout << "No tasks found.\n";
    }
    
    void showOverdue() {
        // Gereftan tarikhe emroz az system
        time_t now = time(0);
        tm *ltm = localtime(&now);
        char dateBuffer[20];
        // Format YYYY-MM-DD
        sprintf(dateBuffer, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
        string today = string(dateBuffer);

        cout << "\nToday is: " << today << endl;
        bool found = false;
        
        for (int i = 0; i < tasks.size(); i++) {
            // Check: Anjam nashode va deadline gozashte bashad
            if (!tasks[i]->getStatus() && tasks[i]->getDeadline() < today) {
                tasks[i]->print();
                found = true;
            }
        }
        if (!found) cout << "No overdue tasks (Good job!).\n";
    }

    void changeStatus() {
        int id;
        cout << "Enter Task ID to toggle status: ";
        cin >> id;
        if (id > 0 && id <= tasks.size()) {
            bool s = tasks[id-1]->getStatus();
            tasks[id-1]->setStatus(!s);
            cout << "Status changed.\n";
        } else {
            cout << "Invalid ID.\n";
        }
    }

    void deleteTask() {
        int id;
        cout << "Enter Task ID to delete: ";
        cin >> id;
        if (id > 0 && id <= tasks.size()) {
            delete tasks[id-1]; // Avval az hafeze pak mikonim
            tasks.erase(tasks.begin() + (id - 1)); // Bad az vector
            cout << "Deleted.\n";
        } else {
            cout << "Invalid ID.\n";
        }
    }

    void sortTasks() {
        sort(tasks.begin(), tasks.end(), compareTasks);
        cout << "List sorted by Priority (High -> Low).\n";
        showAll();
    }

  
    void saveToFile() {
        ofstream f("tasks_data.txt");
        if (f.is_open()) {
            for (int i = 0; i < tasks.size(); i++) {
                f << tasks[i]->getDataForFile() << endl;
            }
            f.close();
            cout << "Data saved to tasks_data.txt\n";
        } else {
            cout << "Error: Could not save file.\n";
        }
    }

    void loadFromFile() {
        ifstream f("tasks_data.txt");
        if (!f.is_open()) return;

        string line;
        while (getline(f, line)) {
            // Parse kardane dasti string (bedone ketabkhone ezafe)
            string t, d, dl, pStr, doneStr;
            int p;
            bool done;
            
            // Format: Title|Desc|Deadline|Priority|Done
            int pos;
            
            pos = line.find("|");
            t = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find("|");
            d = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find("|");
            dl = line.substr(0, pos);
            line.erase(0, pos + 1);

            pos = line.find("|");
            pStr = line.substr(0, pos);
            line.erase(0, pos + 1);
            
            doneStr = line; 

            p = stoi(pStr);
            done = (doneStr == "1");

            tasks.push_back(new Todo(t, d, dl, p, done));
        }
        f.close();
        cout << "Data loaded successfully.\n";
    }
};

int main() {
    TodoList myList;
    
    myList.loadFromFile();
    
    int op = -1;
    while (op != 0) {
        cout << "\n=== TODO LIST PROJECT ===\n";
        cout << "1. Add New Task\n";
        cout << "2. Show All Tasks\n";
        cout << "3. Show Completed Tasks\n";
        cout << "4. Show Pending Tasks\n";
        cout << "5. Show Overdue Tasks\n";
        cout << "6. Change Status (Done/Not Done)\n";
        cout << "7. Delete Task\n";
        cout << "8. Sort by Priority\n";
        cout << "0. Save & Exit\n";
        cout << "Select Option: ";
        cin >> op;

        switch (op) {
            case 1: myList.addTask(); break;
            case 2: myList.showAll(); break;
            case 3: myList.showDone(true); break;
            case 4: myList.showDone(false); break;
            case 5: myList.showOverdue(); break;
            case 6: myList.changeStatus(); break;
            case 7: myList.deleteTask(); break;
            case 8: myList.sortTasks(); break;
            case 0: 
                myList.saveToFile();
                cout << "Goodbye!\n";
                break;
            default: cout << "Invalid Option!\n";
        }
    }
    return 0;
}