#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class Observer {
public:
    virtual void update(const string& teacherName, const string& departmentName,
                        const string& groupName, const string& disciplineName) = 0;
    virtual ~Observer() {}
};

class Subject {
public:
    virtual void attach(Observer* observer) = 0;
    virtual void detach(Observer* observer) = 0;
    virtual void notify(const string& teacherName, const string& departmentName,
                        const string& groupName, const string& disciplineName) = 0;
    virtual ~Subject() {}
};

class Department : public Observer {
private:
    string name;

public:
    Department(const string& name) {
        this->name = name;
    }

    string getName() const {
        return name;
    }

    void update(const string& teacherName, const string& departmentName,
                const string& groupName, const string& disciplineName) override {
        if (departmentName == name) {
            cout << "Кафедра " << name << " получила уведомление" << endl;
            cout << "Преподаватель: " << teacherName << endl;
            cout << "Группа: " << groupName << endl;
            cout << "Дисциплина: " << disciplineName << endl;
            cout << "Причина: текущая успеваемость не была создана вовремя" << endl;
            cout << endl;
        }
    }
};

class ProgressReport {
private:
    string teacherName;
    string departmentName;
    string groupName;
    string disciplineName;
    bool created;

public:
    ProgressReport(const string& teacherName, const string& departmentName,
                   const string& groupName, const string& disciplineName) {
        this->teacherName = teacherName;
        this->departmentName = departmentName;
        this->groupName = groupName;
        this->disciplineName = disciplineName;
        this->created = false;
    }

    string getTeacherName() const {
        return teacherName;
    }

    string getDepartmentName() const {
        return departmentName;
    }

    string getGroupName() const {
        return groupName;
    }

    string getDisciplineName() const {
        return disciplineName;
    }

    bool isCreated() const {
        return created;
    }

    void create() {
        created = true;
    }
};

class Database {
private:
    vector<ProgressReport*> reports;

public:
    void saveReport(ProgressReport* report) {
        if (report == nullptr) {
            return;
        }

        report->create();
        reports.push_back(report);

        cout << "Успеваемость создана и размещена в базе данных" << endl;
        cout << "Преподаватель: " << report->getTeacherName() << endl;
        cout << "Кафедра: " << report->getDepartmentName() << endl;
        cout << "Группа: " << report->getGroupName() << endl;
        cout << "Дисциплина: " << report->getDisciplineName() << endl;
        cout << endl;
    }

    bool hasReport(const string& teacherName, const string& groupName,
                   const string& disciplineName) const {
        for (ProgressReport* report : reports) {
            if (report->getTeacherName() == teacherName &&
                report->getGroupName() == groupName &&
                report->getDisciplineName() == disciplineName &&
                report->isCreated()) {
                return true;
            }
        }
        return false;
    }

    void printReports() const {
        cout << "----- База данных текущей успеваемости -----" << endl;

        if (reports.empty()) {
            cout << "Записей нет" << endl;
            cout << endl;
            return;
        }

        for (ProgressReport* report : reports) {
            cout << "Преподаватель: " << report->getTeacherName()
                 << ", кафедра: " << report->getDepartmentName()
                 << ", группа: " << report->getGroupName()
                 << ", дисциплина: " << report->getDisciplineName() << endl;
        }
        cout << endl;
    }
};

class Teacher {
private:
    string name;
    string departmentName;

public:
    Teacher(const string& name, const string& departmentName) {
        this->name = name;
        this->departmentName = departmentName;
    }

    string getName() const {
        return name;
    }

    string getDepartmentName() const {
        return departmentName;
    }

    void createProgress(Database& database, const string& groupName,
                        const string& disciplineName) {
        ProgressReport* report = new ProgressReport(name, departmentName, groupName, disciplineName);
        database.saveReport(report);
    }
};

class Deanery : public Subject {
private:
    vector<Observer*> observers;
    Database* database;

public:
    Deanery(Database* database) {
        this->database = database;
    }

    void attach(Observer* observer) override {
        observers.push_back(observer);
    }

    void detach(Observer* observer) override {
        observers.erase(remove(observers.begin(), observers.end(), observer), observers.end());
    }

    void notify(const string& teacherName, const string& departmentName,
                const string& groupName, const string& disciplineName) override {
        for (Observer* observer : observers) {
            observer->update(teacherName, departmentName, groupName, disciplineName);
        }
    }

    void checkWeeklyProgress(const vector<Teacher*>& teachers,
                             const vector<string>& groups,
                             const string& disciplineName) {
        cout << "----- Деканат проверяет текущую успеваемость -----" << endl;
        cout << "Дисциплина: " << disciplineName << endl;
        cout << endl;

        for (Teacher* teacher : teachers) {
            for (const string& group : groups) {
                bool exists = database->hasReport(teacher->getName(), group, disciplineName);

                if (exists) {
                    cout << "Отчет найден: " << teacher->getName()
                         << ", группа " << group << endl;
                } else {
                    cout << "Отчет не найден: " << teacher->getName()
                         << ", группа " << group << endl;
                    notify(teacher->getName(), teacher->getDepartmentName(), group, disciplineName);
                }
            }
        }

        cout << endl;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");

    Database database;

    Department programmingDepartment("Программной инженерии");
    Department mathDepartment("Высшей математики");

    Deanery deanery(&database);
    deanery.attach(&programmingDepartment);
    deanery.attach(&mathDepartment);

    Teacher teacher1("Иванов И.И.", "Программной инженерии");
    Teacher teacher2("Петров П.П.", "Высшей математики");
    Teacher teacher3("Сидоров С.С.", "Программной инженерии");

    vector<Teacher*> teachers;
    teachers.push_back(&teacher1);
    teachers.push_back(&teacher2);
    teachers.push_back(&teacher3);

    vector<string> groups;
    groups.push_back("ПИН-11");
    groups.push_back("ПИН-12");

    string discipline = "Объектно-ориентированное программирование";

    cout << "----- Преподаватели создают текущую успеваемость -----" << endl;
    teacher1.createProgress(database, "ПИН-11", discipline);
    teacher2.createProgress(database, "ПИН-11", discipline);
    teacher3.createProgress(database, "ПИН-12", discipline);

    database.printReports();

    deanery.checkWeeklyProgress(teachers, groups, discipline);

    return 0;
}
