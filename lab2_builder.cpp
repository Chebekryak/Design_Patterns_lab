#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Driver {
protected:
    string name;
    string category;

public:
    Driver(string name, string category) {
        this->name = name;
        this->category = category;
    }

    virtual ~Driver() {}

    string getName() {
        return name;
    }

    string getLicenseCategory() {
        return category;
    }

    virtual string getDriverType() = 0;
};

class TaxiDriver : public Driver {
public:
    TaxiDriver(string name) : Driver(name, "B") {}

    string getDriverType() override {
        return "Taxi driver";
    }
};

class BusDriver : public Driver {
public:
    BusDriver(string name) : Driver(name, "D") {}

    string getDriverType() override {
        return "Bus driver";
    }
};

class Passenger {
protected:
    string name;

public:
    Passenger(string name) {
        this->name = name;
    }

    virtual ~Passenger() {}

    string getName() {
        return name;
    }

    virtual string getPassengerType() = 0;
    virtual int getTicketPrice() = 0;
};

class AdultBusPassenger : public Passenger {
public:
    AdultBusPassenger(string name) : Passenger(name) {}

    string getPassengerType() override {
        return "Adult bus passenger";
    }

    int getTicketPrice() override {
        return 100;
    }
};

class PrivilegedBusPassenger : public Passenger {
public:
    PrivilegedBusPassenger(string name) : Passenger(name) {}

    string getPassengerType() override {
        return "Privileged bus passenger";
    }

    int getTicketPrice() override {
        return 50;
    }
};

class ChildBusPassenger : public Passenger {
public:
    ChildBusPassenger(string name) : Passenger(name) {}

    string getPassengerType() override {
        return "Child bus passenger";
    }

    int getTicketPrice() override {
        return 30;
    }
};

class AdultTaxiPassenger : public Passenger {
public:
    AdultTaxiPassenger(string name) : Passenger(name) {}

    string getPassengerType() override {
        return "Adult taxi passenger";
    }

    int getTicketPrice() override {
        return 250;
    }
};

class ChildTaxiPassenger : public Passenger {
private:
    bool hasChildSeat;

public:
    ChildTaxiPassenger(string name, bool hasChildSeat) : Passenger(name) {
        this->hasChildSeat = hasChildSeat;
    }

    string getPassengerType() override {
        if (hasChildSeat) {
            return "Child taxi passenger with child seat";
        }
        return "Child taxi passenger without child seat";
    }

    int getTicketPrice() override {
        return 150;
    }

    bool childSeatReady() {
        return hasChildSeat;
    }
};

class Transport {
protected:
    string transportType;
    int maxPassengers;
    Driver* driver;
    vector<Passenger*> passengers;

public:
    Transport(string transportType, int maxPassengers) {
        this->transportType = transportType;
        this->maxPassengers = maxPassengers;
        this->driver = nullptr;
    }

    virtual ~Transport() {
        delete driver;
        for (Passenger* passenger : passengers) {
            delete passenger;
        }
        passengers.clear();
    }

    bool setDriver(Driver* newDriver) {
        if (driver != nullptr) {
            cout << transportType << " already has a driver!" << endl;
            delete newDriver;
            return false;
        }

        if (!isCorrectDriver(newDriver)) {
            cout << newDriver->getDriverType() << " cannot drive " << transportType << endl;
            delete newDriver;
            return false;
        }

        driver = newDriver;
        cout << driver->getDriverType() << " " << driver->getName()
             << " boarded " << transportType
             << " (Category: " << driver->getLicenseCategory() << ")" << endl;
        return true;
    }

    bool addPassenger(Passenger* passenger) {
        if ((int)passengers.size() >= maxPassengers) {
            cout << transportType << " is full! Max capacity: " << maxPassengers << endl;
            delete passenger;
            return false;
        }

        if (!isCorrectPassenger(passenger)) {
            cout << passenger->getPassengerType() << " cannot board " << transportType << endl;
            delete passenger;
            return false;
        }

        passengers.push_back(passenger);
        cout << passenger->getPassengerType() << " " << passenger->getName()
             << " boarded. Ticket price: " << passenger->getTicketPrice() << endl;
        return true;
    }

    bool isReady() {
        return driver != nullptr && !passengers.empty() && (int)passengers.size() <= maxPassengers;
    }

    void startTrip() {
        cout << endl << "Trying to start " << transportType << endl;

        if (!isReady()) {
            cout << transportType << " is not ready!" << endl;
            if (driver == nullptr) {
                cout << " - No driver" << endl;
            }
            if (passengers.empty()) {
                cout << " - No passengers" << endl;
            }
            return;
        }

        cout << transportType << " started the trip" << endl;
        cout << "Driver: " << driver->getName() << endl;
        cout << "Passengers: " << passengers.size() << "/" << maxPassengers << endl;
        cout << "Total tickets cost: " << getTotalTicketsCost() << endl;
    }

    int getTotalTicketsCost() {
        int totalCost = 0;
        for (Passenger* passenger : passengers) {
            totalCost += passenger->getTicketPrice();
        }
        return totalCost;
    }

    string getTransportType() {
        return transportType;
    }

    int getMaxPassengers() {
        return maxPassengers;
    }

    virtual bool isCorrectDriver(Driver* driver) = 0;
    virtual bool isCorrectPassenger(Passenger* passenger) = 0;
};

class Taxi : public Transport {
public:
    Taxi() : Transport("Taxi", 4) {}

    bool isCorrectDriver(Driver* driver) override {
        return driver->getLicenseCategory() == "B";
    }

    bool isCorrectPassenger(Passenger* passenger) override {
        ChildTaxiPassenger* child = dynamic_cast<ChildTaxiPassenger*>(passenger);
        if (child != nullptr && !child->childSeatReady()) {
            cout << "Child taxi passenger needs a child seat!" << endl;
            return false;
        }

        return dynamic_cast<AdultTaxiPassenger*>(passenger) != nullptr ||
               dynamic_cast<ChildTaxiPassenger*>(passenger) != nullptr;
    }
};

class Bus : public Transport {
public:
    Bus() : Transport("Bus", 30) {}

    bool isCorrectDriver(Driver* driver) override {
        return driver->getLicenseCategory() == "D";
    }

    bool isCorrectPassenger(Passenger* passenger) override {
        return dynamic_cast<AdultBusPassenger*>(passenger) != nullptr ||
               dynamic_cast<PrivilegedBusPassenger*>(passenger) != nullptr ||
               dynamic_cast<ChildBusPassenger*>(passenger) != nullptr;
    }
};

class TransportBuilder {
protected:
    Transport* transport;

public:
    TransportBuilder() {
        transport = nullptr;
    }

    virtual ~TransportBuilder() {}

    virtual void createTransport() = 0;
    virtual void buildDriver(string name) = 0;
    virtual void buildAdultPassenger(string name) = 0;
    virtual void buildChildPassenger(string name) = 0;

    Transport* getTransport() {
        Transport* result = transport;
        transport = nullptr;
        return result;
    }
};

class TaxiBuilder : public TransportBuilder {
public:
    void createTransport() override {
        transport = new Taxi();
        cout << "Created taxi with capacity: " << transport->getMaxPassengers() << endl;
    }

    void buildDriver(string name) override {
        transport->setDriver(new TaxiDriver(name));
    }

    void buildAdultPassenger(string name) override {
        transport->addPassenger(new AdultTaxiPassenger(name));
    }

    void buildChildPassenger(string name) override {
        transport->addPassenger(new ChildTaxiPassenger(name, true));
    }

    void buildChildPassengerWithoutSeat(string name) {
        transport->addPassenger(new ChildTaxiPassenger(name, false));
    }
};

class BusBuilder : public TransportBuilder {
public:
    void createTransport() override {
        transport = new Bus();
        cout << "Created bus with capacity: " << transport->getMaxPassengers() << endl;
    }

    void buildDriver(string name) override {
        transport->setDriver(new BusDriver(name));
    }

    void buildAdultPassenger(string name) override {
        transport->addPassenger(new AdultBusPassenger(name));
    }

    void buildChildPassenger(string name) override {
        transport->addPassenger(new ChildBusPassenger(name));
    }

    void buildPrivilegedPassenger(string name) {
        transport->addPassenger(new PrivilegedBusPassenger(name));
    }
};

class TransportDirector {
public:
    Transport* makeReadyTaxi(TaxiBuilder* builder) {
        builder->createTransport();
        builder->buildDriver("Ann Taxist");
        builder->buildAdultPassenger("Ivan");
        builder->buildChildPassenger("Masha");
        return builder->getTransport();
    }

    Transport* makeReadyBus(BusBuilder* builder) {
        builder->createTransport();
        builder->buildDriver("John Busser");
        builder->buildAdultPassenger("Alex");
        builder->buildPrivilegedPassenger("Petr");
        builder->buildChildPassenger("Sasha");
        return builder->getTransport();
    }
};

int main() {
    TransportDirector director;
    TaxiBuilder taxiBuilder;
    BusBuilder busBuilder;

    cout << "Test 1: Ready taxi" << endl;
    Transport* taxi = director.makeReadyTaxi(&taxiBuilder);
    taxi->startTrip();

    cout << endl << "Test 2: Ready bus" << endl;
    Transport* bus = director.makeReadyBus(&busBuilder);
    bus->startTrip();

    cout << endl << "Test 3: Taxi without passengers" << endl;
    taxiBuilder.createTransport();
    taxiBuilder.buildDriver("Kate Taxist");
    Transport* emptyTaxi = taxiBuilder.getTransport();
    emptyTaxi->startTrip();

    cout << endl << "Test 4: Taxi passenger limit" << endl;
    taxiBuilder.createTransport();
    taxiBuilder.buildDriver("Bob Taxist");
    taxiBuilder.buildAdultPassenger("Passenger 1");
    taxiBuilder.buildAdultPassenger("Passenger 2");
    taxiBuilder.buildAdultPassenger("Passenger 3");
    taxiBuilder.buildAdultPassenger("Passenger 4");
    taxiBuilder.buildAdultPassenger("Passenger 5");
    Transport* fullTaxi = taxiBuilder.getTransport();
    fullTaxi->startTrip();

    cout << endl << "Test 5: Child taxi passenger without child seat" << endl;
    taxiBuilder.createTransport();
    taxiBuilder.buildDriver("Sam Taxist");
    taxiBuilder.buildChildPassengerWithoutSeat("Little Tom");
    Transport* taxiWithoutSeat = taxiBuilder.getTransport();
    taxiWithoutSeat->startTrip();

    cout << endl << "Test 6: Two drivers in one bus" << endl;
    busBuilder.createTransport();
    busBuilder.buildDriver("First Busser");
    busBuilder.buildDriver("Second Busser");
    busBuilder.buildAdultPassenger("Bus passenger");
    Transport* busWithTwoDrivers = busBuilder.getTransport();
    busWithTwoDrivers->startTrip();

    delete taxi;
    delete bus;
    delete emptyTaxi;
    delete fullTaxi;
    delete taxiWithoutSeat;
    delete busWithTwoDrivers;

    return 0;
}
