#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Driver {  // базовый водитель
protected:
    string name;
    string category;

    Driver(string name, string category) {
        this->name = name;
        this->category = category;
    }

public:
    virtual ~Driver() {}

    string getLicenseCategory() {
        return category;
    }

    string getName() {
        return name;
    }

    virtual void drive() = 0;
};

class TaxiDriver : public Driver {
private:
    static TaxiDriver* instance;

    TaxiDriver() : Driver("Ann Taxist", "Taxi Category B") {}

    TaxiDriver(const TaxiDriver&) = delete;
    TaxiDriver& operator=(const TaxiDriver&) = delete;

public:
    static TaxiDriver* getInstance() {
        if (!instance) {
            instance = new TaxiDriver();
        }
        return instance;
    }

    void drive() override {
        cout << "Taxi driver " << name << " is driving (Category: " << category << ")" << endl;
    }

    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }
};

TaxiDriver* TaxiDriver::instance = nullptr;

class BusDriver : public Driver {
private:
    static BusDriver* instance;

    BusDriver() : Driver("John Busser", "Bus Category D") {}

    BusDriver(const BusDriver&) = delete;
    BusDriver& operator=(const BusDriver&) = delete;

public:
    static BusDriver* getInstance() {
        if (!instance) {
            instance = new BusDriver();
        }
        return instance;
    }

    void drive() override {
        cout << "Bus driver " << name << " is driving (Category: " << category << ")" << endl;
    }

    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }
};

BusDriver* BusDriver::instance = nullptr;

class Passenger {
private:
    int id;

public:
    Passenger(int id) {
        this->id = id;
    }

    void board() {
        cout << "Passenger " << id << " boarded" << endl;
    }

    int getId() {
        return id;
    }
};

class BoardAnyCar {
protected:
    Driver* driver;
    vector<Passenger*> passengers;
    bool isReady = false;
    int maxPassengers;

public:
    BoardAnyCar() {
        driver = nullptr;
        maxPassengers = 0;
    }

    virtual ~BoardAnyCar() {
        for (Passenger* passenger : passengers) {
            delete passenger;
        }
        passengers.clear();
    }

    virtual bool BoardDriver() = 0;
    virtual bool BoardPassenger() = 0;

    bool isVehicleReady() const {
        return isReady && driver != nullptr && !passengers.empty();
    }

    void startTrip() {
        if (isVehicleReady()) {
            cout << "Vehicle is starting the trip!" << endl;
            driver->drive();
            cout << "Vehicle has " << passengers.size() << " passengers" << endl;
        } else {
            cout << "Vehicle is not ready for trip!" << endl;
            if (!driver)
                cout << " - No driver" << endl;
            if (passengers.empty())
                cout << " - No passengers" << endl;
        }
    }

    int getPassengerCount() {
        return passengers.size();
    }

    bool hasDriver() {
        return driver != nullptr;
    }

    int getMaxCapacity() {
        return maxPassengers;
    }
};

class BoardTaxi : public BoardAnyCar {
public:
    BoardTaxi() {
        maxPassengers = 4;
        cout << "Created taxi with capacity: " << maxPassengers << " passengers" << endl;
    }

    ~BoardTaxi() {}

    bool BoardDriver() override {
        if (driver) {
            cout << "Taxi already has a driver!" << endl;
            return false;
        }

        driver = TaxiDriver::getInstance();
        cout << "Taxi driver boarded (License: " << driver->getLicenseCategory() << ")" << endl;
        checkReadiness();
        return true;
    }

    bool BoardPassenger() override {
        if (passengers.size() >= maxPassengers) {
            cout << "Taxi is full! Max capacity: " << maxPassengers << endl;
            return false;
        }

        Passenger* passenger = new Passenger(passengers.size() + 1);
        passengers.push_back(passenger);
        passenger->board();
        checkReadiness();
        return true;
    }

private:
    void checkReadiness() {
        isReady = (driver != nullptr && passengers.size() > 0 && passengers.size() <= maxPassengers);
        if (isReady) {
            cout << "Taxi is ready for trip! Passengers: " << passengers.size() << "/" << maxPassengers << endl;
        }
    }
};

class BoardBus : public BoardAnyCar {
public:
    BoardBus() {
        maxPassengers = 30;
        cout << "Created bus with capacity: " << maxPassengers << " passengers" << endl;
    }

    ~BoardBus() {}

    bool BoardDriver() override {
        if (driver) {
            cout << "Bus already has a driver!" << endl;
            return false;
        }

        driver = BusDriver::getInstance();
        cout << "Bus driver boarded (License: " << driver->getLicenseCategory() << ")" << endl;
        checkReadiness();
        return true;
    }

    bool BoardPassenger() override {
        if (passengers.size() >= maxPassengers) {
            cout << "Bus is full! Max capacity: " << maxPassengers << endl;
            return false;
        }

        Passenger* passenger = new Passenger(passengers.size() + 1);
        passengers.push_back(passenger);
        passenger->board();
        checkReadiness();
        return true;
    }

private:
    void checkReadiness() {
        isReady = (driver != nullptr && passengers.size() > 0 && passengers.size() <= maxPassengers);
        if (isReady) {
            cout << "Bus is ready for trip! Passengers: " << passengers.size() << "/" << maxPassengers << endl;
        }
    }
};

class TransportFactory {
public:
    virtual ~TransportFactory() {}
    virtual BoardAnyCar* createVehicle() = 0;
    virtual string getFactoryType() const = 0;
};

class TaxiFactory : public TransportFactory {
public:
    BoardAnyCar* createVehicle() override {
        return new BoardTaxi();
    }

    string getFactoryType() const override {
        return "Taxi Factory";
    }
};

class BusFactory : public TransportFactory {
public:
    BoardAnyCar* createVehicle() override {
        return new BoardBus();
    }

    string getFactoryType() const override {
        return "Bus Factory";
    }
};

int main() {
    TaxiFactory taxiFactory;
    BusFactory busFactory;

    cout << "Test 1: Taxi Loading" << endl;
    BoardAnyCar* taxi = taxiFactory.createVehicle();

    cout << endl << "Attempting to start empty taxi:" << endl;
    taxi->startTrip();

    cout << endl << "Boarding taxi driver:" << endl;
    taxi->BoardDriver();

    cout << endl << "Boarding passengers:" << endl;
    for (int i = 0; i < 5; i++) {
        taxi->BoardPassenger();
    }

    cout << endl << "Attempting to start trip:" << endl;
    taxi->startTrip();

    cout << endl << "Test 2: Bus loading" << endl;
    BoardAnyCar* bus = busFactory.createVehicle();

    cout << endl << "Boarding bus driver:" << endl;
    bus->BoardDriver();

    cout << endl << "Boarding passengers:" << endl;
    for (int i = 0; i < 3; i++) {
        bus->BoardPassenger();
    }

    cout << endl << "Attempting to start trip:" << endl;
    bus->startTrip();

    cout << endl << "Test 3: Bus without passengers" << endl;
    BoardAnyCar* emptyBus = busFactory.createVehicle();
    emptyBus->BoardDriver();
    emptyBus->startTrip();

    delete taxi;
    delete bus;
    delete emptyBus;

    TaxiDriver::destroyInstance();
    BusDriver::destroyInstance();

    return 0;
}
