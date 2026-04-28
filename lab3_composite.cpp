#include <iostream>
#include <vector>
#include <string>

using namespace std;

class FlightComponent {
public:
    virtual ~FlightComponent() {}
    virtual string getName() = 0;
    virtual int getBaggageWeight() = 0;
    virtual int getOverweight() = 0;
    virtual void printInfo(int level = 0) = 0;
};

class Person : public FlightComponent {
protected:
    string name;

public:
    Person(string name) {
        this->name = name;
    }

    string getName() override {
        return name;
    }
};

class Pilot : public Person {
public:
    Pilot(string name) : Person(name) {}

    int getBaggageWeight() override {
        return 0;
    }

    int getOverweight() override {
        return 0;
    }

    void printInfo(int level = 0) override {
        for (int i = 0; i < level; i++) cout << "  ";
        cout << "Pilot: " << name << endl;
    }
};

class Stewardess : public Person {
public:
    Stewardess(string name) : Person(name) {}

    int getBaggageWeight() override {
        return 0;
    }

    int getOverweight() override {
        return 0;
    }

    void printInfo(int level = 0) override {
        for (int i = 0; i < level; i++) cout << "  ";
        cout << "Stewardess: " << name << endl;
    }
};

class Passenger : public Person {
private:
    int baggageWeight;
    int removedBaggage;

public:
    Passenger(string name, int baggageWeight) : Person(name) {
        if (baggageWeight < 5) {
            cout << "Passenger " << name << " has baggage less than 5 kg. Set to 5 kg" << endl;
            baggageWeight = 5;
        }

        if (baggageWeight > 60) {
            cout << "Passenger " << name << " has baggage more than 60 kg. Set to 60 kg" << endl;
            baggageWeight = 60;
        }

        this->baggageWeight = baggageWeight;
        this->removedBaggage = 0;
    }

    int getBaggageWeight() override {
        return baggageWeight;
    }

    int getRemovedBaggage() {
        return removedBaggage;
    }

    int getOverweight() override {
        return 0;
    }

    int removeBaggage(int needToRemove) {
        int removed = 0;

        if (needToRemove <= 0 || baggageWeight <= 0) {
            return removed;
        }

        if (baggageWeight >= needToRemove) {
            removed = needToRemove;
        }
        else {
            removed = baggageWeight;
        }

        baggageWeight -= removed;
        removedBaggage += removed;
        return removed;
    }

    void printInfo(int level = 0) override {
        for (int i = 0; i < level; i++) cout << "  ";
        cout << "Passenger: " << name << ", baggage: " << baggageWeight << " kg";

        if (removedBaggage > 0) {
            cout << ", removed from flight: " << removedBaggage << " kg";
        }

        cout << endl;
    }
};

class PassengerClass : public FlightComponent {
protected:
    string className;
    int maxPassengers;
    int freeBaggageLimit;
    vector<Passenger*> passengers;

public:
    PassengerClass(string className, int maxPassengers, int freeBaggageLimit) {
        this->className = className;
        this->maxPassengers = maxPassengers;
        this->freeBaggageLimit = freeBaggageLimit;
    }

    virtual ~PassengerClass() {
        for (int i = 0; i < passengers.size(); i++) {
            delete passengers[i];
        }
    }

    string getName() override {
        return className;
    }

    bool addPassenger(Passenger* passenger) {
        if (passengers.size() >= maxPassengers) {
            cout << className << " is full. Passenger " << passenger->getName() << " cannot be added" << endl;
            delete passenger;
            return false;
        }

        passengers.push_back(passenger);
        cout << "Passenger " << passenger->getName() << " added to " << className << endl;
        return true;
    }

    int getPassengerCount() {
        return passengers.size();
    }

    int getBaggageWeight() override {
        int sum = 0;

        for (int i = 0; i < passengers.size(); i++) {
            sum += passengers[i]->getBaggageWeight();
        }

        return sum;
    }

    int getOverweight() override {
        if (freeBaggageLimit < 0) {
            return 0;
        }

        int sum = 0;

        for (int i = 0; i < passengers.size(); i++) {
            int baggage = passengers[i]->getBaggageWeight();

            if (baggage > freeBaggageLimit) {
                sum += baggage - freeBaggageLimit;
            }
        }

        return sum;
    }

    virtual int removeBaggage(int needToRemove) {
        return 0;
    }

    void printInfo(int level = 0) override {
        for (int i = 0; i < level; i++) cout << "  ";
        cout << className << ": " << passengers.size() << "/" << maxPassengers
             << " passengers, baggage: " << getBaggageWeight() << " kg";

        if (freeBaggageLimit < 0) {
            cout << ", free baggage: without limit";
        }
        else {
            cout << ", free baggage: " << freeBaggageLimit << " kg";
        }

        cout << ", overweight: " << getOverweight() << " kg" << endl;

        for (int i = 0; i < passengers.size(); i++) {
            passengers[i]->printInfo(level + 1);
        }
    }
};

class FirstClass : public PassengerClass {
public:
    FirstClass() : PassengerClass("First class", 10, -1) {}
};

class BusinessClass : public PassengerClass {
public:
    BusinessClass() : PassengerClass("Business class", 20, 35) {}
};

class EconomyClass : public PassengerClass {
public:
    EconomyClass() : PassengerClass("Economy class", 150, 20) {}

    int removeBaggage(int needToRemove) override {
        int removedSum = 0;

        for (int i = 0; i < passengers.size(); i++) {
            if (removedSum >= needToRemove) {
                break;
            }

            int removed = passengers[i]->removeBaggage(needToRemove - removedSum);

            if (removed > 0) {
                cout << "Removed " << removed << " kg baggage from economy passenger "
                     << passengers[i]->getName() << endl;
            }

            removedSum += removed;
        }

        return removedSum;
    }
};

class Airplane {
private:
    string number;
    int maxBaggageWeight;
    vector<Pilot*> pilots;
    vector<Stewardess*> stewardesses;
    FirstClass* firstClass;
    BusinessClass* businessClass;
    EconomyClass* economyClass;

public:
    Airplane(string number, int maxBaggageWeight) {
        this->number = number;
        this->maxBaggageWeight = maxBaggageWeight;
        firstClass = new FirstClass();
        businessClass = new BusinessClass();
        economyClass = new EconomyClass();
    }

    ~Airplane() {
        for (int i = 0; i < pilots.size(); i++) {
            delete pilots[i];
        }

        for (int i = 0; i < stewardesses.size(); i++) {
            delete stewardesses[i];
        }

        delete firstClass;
        delete businessClass;
        delete economyClass;
    }

    void addPilot(Pilot* pilot) {
        if (pilots.size() >= 2) {
            cout << "There are already 2 pilots. Pilot " << pilot->getName() << " cannot be added" << endl;
            delete pilot;
            return;
        }

        pilots.push_back(pilot);
        cout << "Pilot " << pilot->getName() << " added" << endl;
    }

    void addStewardess(Stewardess* stewardess) {
        if (stewardesses.size() >= 6) {
            cout << "There are already 6 stewardesses. Stewardess " << stewardess->getName() << " cannot be added" << endl;
            delete stewardess;
            return;
        }

        stewardesses.push_back(stewardess);
        cout << "Stewardess " << stewardess->getName() << " added" << endl;
    }

    void addFirstClassPassenger(Passenger* passenger) {
        firstClass->addPassenger(passenger);
    }

    void addBusinessClassPassenger(Passenger* passenger) {
        businessClass->addPassenger(passenger);
    }

    void addEconomyClassPassenger(Passenger* passenger) {
        economyClass->addPassenger(passenger);
    }

    int getPassengerCount() {
        return firstClass->getPassengerCount() + businessClass->getPassengerCount() + economyClass->getPassengerCount();
    }

    int getTotalBaggageWeight() {
        return firstClass->getBaggageWeight() + businessClass->getBaggageWeight() + economyClass->getBaggageWeight();
    }

    int getTotalOverweight() {
        return firstClass->getOverweight() + businessClass->getOverweight() + economyClass->getOverweight();
    }

    void controlBaggageLoad() {
        cout << endl;
        cout << "Baggage loading control" << endl;
        cout << "Current baggage weight: " << getTotalBaggageWeight() << " kg" << endl;
        cout << "Maximum baggage weight: " << maxBaggageWeight << " kg" << endl;

        if (getTotalBaggageWeight() <= maxBaggageWeight) {
            cout << "Baggage loading is normal" << endl;
            return;
        }

        int overload = getTotalBaggageWeight() - maxBaggageWeight;
        cout << "Baggage overload: " << overload << " kg" << endl;
        cout << "Baggage can be removed only from economy class passengers" << endl;

        int removed = economyClass->removeBaggage(overload);

        cout << "Total removed baggage: " << removed << " kg" << endl;

        if (getTotalBaggageWeight() > maxBaggageWeight) {
            cout << "Baggage is still overloaded by "
                 << getTotalBaggageWeight() - maxBaggageWeight << " kg" << endl;
        }
        else {
            cout << "Baggage overload fixed" << endl;
        }
    }

    bool isReadyToFly() {
        if (pilots.size() != 2) {
            cout << "Airplane is not ready: need exactly 2 pilots" << endl;
            return false;
        }

        if (stewardesses.size() != 6) {
            cout << "Airplane is not ready: need exactly 6 stewardesses" << endl;
            return false;
        }

        if (getPassengerCount() == 0) {
            cout << "Airplane is not ready: no passengers" << endl;
            return false;
        }

        if (getTotalBaggageWeight() > maxBaggageWeight) {
            cout << "Airplane is not ready: baggage overload" << endl;
            return false;
        }

        cout << "Airplane is ready to fly" << endl;
        return true;
    }

    void printLoadingMap() {
        cout << endl;
        cout << "========== Airplane loading map ==========" << endl;
        cout << "Airplane: " << number << endl;
        cout << "Pilots: " << pilots.size() << "/2" << endl;

        for (int i = 0; i < pilots.size(); i++) {
            pilots[i]->printInfo(1);
        }

        cout << "Stewardesses: " << stewardesses.size() << "/6" << endl;

        for (int i = 0; i < stewardesses.size(); i++) {
            stewardesses[i]->printInfo(1);
        }

        firstClass->printInfo(1);
        businessClass->printInfo(1);
        economyClass->printInfo(1);

        cout << "Total passengers: " << getPassengerCount() << endl;
        cout << "Total baggage: " << getTotalBaggageWeight() << "/" << maxBaggageWeight << " kg" << endl;
        cout << "Paid overweight baggage: " << getTotalOverweight() << " kg" << endl;
        cout << "==========================================" << endl;
    }

    void startFlight() {
        cout << endl;
        cout << "Trying to start flight" << endl;

        if (isReadyToFly()) {
            cout << "Flight started" << endl;
        }
        else {
            cout << "Flight was not started" << endl;
        }
    }
};

int main() {
    Airplane airplane("SU-100", 350);

    cout << "Trying to start empty airplane" << endl;
    airplane.startFlight();

    cout << endl;
    cout << "Loading crew" << endl;
    airplane.addPilot(new Pilot("Ivan"));
    airplane.addPilot(new Pilot("Petr"));
    airplane.addPilot(new Pilot("Extra pilot"));

    airplane.addStewardess(new Stewardess("Anna"));
    airplane.addStewardess(new Stewardess("Maria"));
    airplane.addStewardess(new Stewardess("Olga"));
    airplane.addStewardess(new Stewardess("Elena"));
    airplane.addStewardess(new Stewardess("Sofia"));
    airplane.addStewardess(new Stewardess("Daria"));
    airplane.addStewardess(new Stewardess("Extra stewardess"));

    cout << endl;
    cout << "Loading passengers" << endl;
    airplane.addFirstClassPassenger(new Passenger("First passenger 1", 60));
    airplane.addFirstClassPassenger(new Passenger("First passenger 2", 55));

    airplane.addBusinessClassPassenger(new Passenger("Business passenger 1", 40));
    airplane.addBusinessClassPassenger(new Passenger("Business passenger 2", 35));
    airplane.addBusinessClassPassenger(new Passenger("Business passenger 3", 50));

    airplane.addEconomyClassPassenger(new Passenger("Economy passenger 1", 60));
    airplane.addEconomyClassPassenger(new Passenger("Economy passenger 2", 55));
    airplane.addEconomyClassPassenger(new Passenger("Economy passenger 3", 45));
    airplane.addEconomyClassPassenger(new Passenger("Economy passenger 4", 30));
    airplane.addEconomyClassPassenger(new Passenger("Economy passenger 5", 25));

    airplane.printLoadingMap();
    airplane.startFlight();

    airplane.controlBaggageLoad();
    airplane.printLoadingMap();
    airplane.startFlight();

    return 0;
}
