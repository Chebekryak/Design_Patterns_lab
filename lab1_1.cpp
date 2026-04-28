#include <iostream>
#include <string>

using namespace std;

class Driver {
private:
    static Driver* p_instance;   // статический указатель на единственного водителя
    string name;
    string category;

    Driver() {   // конструктор по умолчанию
        name = "John Driver";
        category = "A";
    }

    Driver(string name, string category) {   // конструктор с параметрами
        this->name = name;
        this->category = category;
    }

    Driver(const Driver&) = delete;              // запрет на копирование
    Driver& operator=(const Driver&) = delete;

public:
    static Driver* getInstance() {
        if (!p_instance)
            p_instance = new Driver();
        return p_instance;
    }

    static Driver* getInstance(string name, string category) {
        if (!p_instance)
            p_instance = new Driver(name, category);
        return p_instance;
    }

    string getInfo() {
        return name + " " + category;
    }

    static void destroyInstance() {
        delete p_instance;
        p_instance = nullptr;
    }
};

Driver* Driver::p_instance = nullptr;

int main() {
    Driver* driver1 = Driver::getInstance();
    Driver* driver2 = Driver::getInstance();

    cout << "driver1: " << driver1->getInfo() << endl;
    cout << "driver2: " << driver2->getInfo() << endl;
    cout << "Singleton: " << (driver1 == driver2) << endl;

    Driver::destroyInstance();

    driver1 = Driver::getInstance("Ann Driver", "C");
    driver2 = Driver::getInstance();

    cout << "driver3: " << driver1->getInfo() << endl;
    cout << "driver4: " << driver2->getInfo() << endl;
    cout << "Singleton: " << (driver1 == driver2) << endl;

    Driver::destroyInstance();
    return 0;
}
