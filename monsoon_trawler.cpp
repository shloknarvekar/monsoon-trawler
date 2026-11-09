// Monsoon Trawler - fishing boat game set in Goa
// OOP mini project (C++)
// you are a fishing captain, pick a boat, go fishing, sell at panjim market
// goal is to make Rs 1,00,000 before december
// no fishing in june and july (monsoon ban)

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;

const int GOAL = 100000;
const int FUEL_PRICE = 90;

string MONTHS[14] = {"", "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December", "Season Over"};
string WEATHER[2] = {"Calm seas", "STORM"};
string ZONES[4] = {"", "Aguada Coast", "Deep Sea", "Grande Island"};
int DISTANCE[4] = {0, 10, 30, 50};

// error messages, index = error number thrown
string ERRORS[8] = {
    "Invalid choice!",
    "MONSOON FISHING BAN (June-July)! Boats must stay in harbour.",
    "Not enough fuel for this trip! Sell your catch to refuel.",
    "STORM! Too dangerous for a sail boat. You waited at the jetty all week.",
    "Fish hold is FULL!",
    "Not enough money!",
    "Your fish hold is empty - nothing to sell.",
    "Could not open the logbook file."
};

void showError(int code) {
    cout << "  >> Error no. " << code << " : " << ERRORS[code] << endl;
}

inline int randomNumber(int low, int high) {
    return low + rand() % (high - low + 1);
}

void line() {
    cout << "--------------------------------------------------" << endl;
}

// ---------- Fish ----------
// Fish is abstract, every fish has its own price

class Fish {
protected:
    string name;
    string localName; // konkani name
    double weight;
    static int totalCaught;
public:
    Fish() {
        weight = 0;
        totalCaught++;
    }
    virtual ~Fish() {}
    virtual int pricePerKg() = 0;

    int value() {
        return weight * pricePerKg() + 0.5; // +0.5 to round off
    }

    void describe() {
        cout << name << " (" << localName << ")  " << weight << " kg  ->  Rs " << value() << endl;
    }

    static int getTotalCaught() { return totalCaught; }
};

int Fish::totalCaught = 0;

class Mackerel : public Fish {
public:
    Mackerel(double w) {
        name = "Mackerel";
        localName = "Bangdo";
        weight = w;
    }
    int pricePerKg() { return 180; }
};

class Pomfret : public Fish {
public:
    Pomfret() {
        name = "Silver Pomfret";
        localName = "Paplet";
    }
    Pomfret(double w) {
        name = "Silver Pomfret";
        localName = "Paplet";
        weight = w;
    }
    int pricePerKg() { return 850; }
};

// multilevel - Fish -> Pomfret -> BlackPomfret
class BlackPomfret : public Pomfret {
public:
    BlackPomfret(double w) {
        name = "Black Pomfret";
        localName = "Halwo";
        weight = w;
    }
    // black pomfret is cheaper than silver
    int pricePerKg() { return Pomfret::pricePerKg() - 350; }
};

// ---------- Boats ----------
// diamond problem: HybridBoat is a SailBoat and a MotorBoat
// virtual inheritance so only one Boat object is made inside HybridBoat

class Boat {
protected:
    double fuel;
    double tankSize; // 0 for sail boat
    int holdSize;
public:
    Boat() {
        cout << "  Boat constructor -> hull is ready" << endl;
        fuel = 0;
        tankSize = 0;
        holdSize = 0;
    }
    // virtual so child destructors also run when we delete a Boat pointer
    virtual ~Boat() { cout << "  Boat destructor -> hull taken apart" << endl; }

    virtual double fuelNeeded(int km) = 0;
    virtual bool canSailIn(int weather) = 0;
    virtual int netsPerTrip() = 0;

    void useFuel(double f) { fuel = fuel - f; }
    void fillTank() { fuel = tankSize; }
    double getFuel() { return fuel; }
    double getTankSize() { return tankSize; }
    int getHoldSize() { return holdSize; }
};

class SailBoat : virtual public Boat {
public:
    SailBoat() {
        cout << "  SailBoat constructor -> mast and sails fitted" << endl;
        holdSize = 6;
    }
    ~SailBoat() { cout << "  SailBoat destructor -> sails removed" << endl; }

    double fuelNeeded(int km) { return 0; }
    bool canSailIn(int weather) { return weather != 1; } // cant go in storm
    int netsPerTrip() { return 3; }
};

class MotorBoat : virtual public Boat {
public:
    MotorBoat() {
        cout << "  MotorBoat constructor -> diesel engine fitted" << endl;
        tankSize = 60;
        fuel = 60;
        holdSize = 10;
    }
    ~MotorBoat() { cout << "  MotorBoat destructor -> engine removed" << endl; }

    double fuelNeeded(int km) { return km * 0.5; }
    bool canSailIn(int weather) { return true; }
    int netsPerTrip() { return 4; }
};

class HybridBoat : public SailBoat, public MotorBoat {
public:
    HybridBoat() {
        cout << "  HybridBoat constructor -> sails + engine combined!" << endl;
        tankSize = 40;
        fuel = 40;
        holdSize = 12;
    }
    ~HybridBoat() { cout << "  HybridBoat destructor -> hybrid system removed" << endl; }

    // have to override these again otherwise its ambiguous
    // (both SailBoat and MotorBoat have their own version)
    double fuelNeeded(int km) { return km * 0.25; }
    bool canSailIn(int weather) { return true; }
    int netsPerTrip() { return 5; }
};

// ---------- Fish hold ----------

class FishHold {
private:
    vector<Fish*> crates;
    int capacity;
public:
    FishHold(int cap) { capacity = cap; }
    ~FishHold() { emptyHold(); }

    int count() { return crates.size(); }
    bool isFull() { return count() >= capacity; }
    void add(Fish *f) {
        if (isFull()) {
            delete f;
            throw 4;
        }
        crates.push_back(f);
    }

    int totalValue() {
        int sum = 0;
        for (Fish *f : crates)
            sum = sum + f->value();
        return sum;
    }

    void show() {
        for (int i = 0; i < count(); i++) {
            cout << "  " << i + 1 << ". ";
            crates.at(i)->describe();
        }
        cout << "  Total value : Rs " << totalValue() << endl;
    }

    void emptyHold() {
        for (Fish *f : crates)
            delete f;
        crates.clear();
    }
};

// ---------- Captain ----------

class Captain {
private:
    string name;
    int money;
    int week; // 1-4
    int month;
public:
    Captain(string name, int money) {
        this->name = name;
        this->money = money;
        this->week = 1;
        this->month = 4; // season starts in april
    }
    Captain(const Captain &c) {
        name = c.name;
        money = c.money;
        week = c.week;
        month = c.month;
    }

    int getMoney() { return money; }
    int getWeek() { return week; }
    int getMonth() { return month; }
    bool monsoonBan() { return month == 6 || month == 7; }
    bool seasonOver() { return month > 12; }
    void earn(int amount) { money = money + amount; }
    void spend(int amount) {
        if (amount > money)
            throw 5;
        money = money - amount;
    }

    // ++captain moves to next week
    void operator++() {
        week++;
        if (week > 4) {
            week = 1;
            month++;
        }
    }

    friend void printLicense(Captain c);
};

void printLicense(Captain c) {
    cout << "  Captain : " << c.name << endl;
    cout << "  Money : Rs " << c.money << " (goal Rs " << GOAL << ")" << endl;
    cout << "  Date : Week " << c.week << " of " << MONTHS[c.month] << endl;
}

// ---------- logbook file ----------

void writeLog(Captain &c, string event, int number) {
    ofstream my_file("logbook.txt", ios::app);
    if (!my_file)
        throw 7;
    my_file << "Week " << c.getWeek() << " of " << MONTHS[c.getMonth()] << " : " << event << number << endl;
    my_file.close();
}

// reads menu choice, throws 0 if wrong input
int readChoice(int low, int high) {
    int choice;
    cin >> choice;
    if (cin.fail()) { // user typed letters
        cin.clear();
        cin.ignore(1000, '\n');
        throw 0;
    }
    cin.ignore(1000, '\n');
    if (choice < low || choice > high)
        throw 0;
    return choice;
}

// ---------- fishing ----------

// returns a fish, or throws 'E' for empty net and 'S' for shark
Fish* catchFish(int zone) {
    int r = randomNumber(0, 99);
    double w = randomNumber(10, 50) / 10.0; // 1 to 5 kg

    if (zone == 1) {
        if (r < 60) return new Mackerel(w);
        if (r < 85) return new BlackPomfret(w);
        throw 'E';
    }
    if (zone == 2) {
        if (r < 45) return new Pomfret(w);
        if (r < 85) return new BlackPomfret(w);
        throw 'E';
    }
    if (r < 80) return new Pomfret(w);
    throw 'S';
}

void goFishing(Captain &c, Boat *boat, FishHold &hold, int zone) {
    int caught = 0;
    bool weekUsed = false;

    try {
        if (c.monsoonBan())
            throw 1;
        if (hold.isFull())
            throw 4;

        double fuel = boat->fuelNeeded(DISTANCE[zone] * 2); // going and coming back
        if (fuel > boat->getFuel())
            throw 2;

        weekUsed = true;
        int weather = 0;
        if (randomNumber(0, 99) >= 85) // 15% chance of storm
            weather = 1;

        cout << "  Weather report: " << WEATHER[weather] << endl;
        if (!boat->canSailIn(weather))
            throw 3;

        boat->useFuel(fuel);
        cout << "  Sailing " << DISTANCE[zone] << " km to " << ZONES[zone] << "..." << endl;

        for (int i = 1; i <= boat->netsPerTrip(); i++) {
            cout << "  Net " << i << ": ";
            try {
                Fish *f = catchFish(zone);
                hold.add(f);
                f->describe();
                caught++;
            }
            catch (char ch) {
                if (ch == 'S')
                    cout << "A SHARK got tangled! Sharks are protected - released back." << endl;
                else
                    cout << "Empty net..." << endl;
            }
            catch (int code) {
                cout << "no space left, heading back early!" << endl;
                throw; // rethrow, outer catch will handle it
            }
        }
        cout << "  Back at Panjim jetty with " << caught << " new crate(s)." << endl;
    }
    catch (int code) {
        showError(code);
    }

    if (weekUsed) {
        writeLog(c, "Fishing trip to " + ZONES[zone] + ", crates caught = ", caught);
        ++c;
    }
}

void sellCatch(Captain &c, Boat *boat, FishHold &hold) {
    if (hold.count() == 0)
        throw 6;

    int rate = randomNumber(90, 120); // market price changes every time
    cout << "  Panjim market is paying " << rate << "% of normal price today." << endl;
    hold.show();

    int earned = hold.totalValue() * rate / 100;
    c.earn(earned);
    cout << "  You earned Rs " << earned << endl;
    writeLog(c, "Sold catch at Panjim market for Rs ", earned);
    hold.emptyHold();

    // refuel at the jetty (sail boat has tankSize 0 so this is skipped)
    int litres = boat->getTankSize() - boat->getFuel();
    if (litres > 0) {
        c.spend(litres * FUEL_PRICE); // throws 5 if not enough money
        boat->fillTank();
        cout << "  Refuelled " << litres << " litres for Rs " << litres * FUEL_PRICE << endl;
    }
}

int main() {
    srand(time(0));

    cout << "=========== M O N S O O N   T R A W L E R ===========" << endl;
    cout << "  Earn Rs " << GOAL << " before December." << endl;
    cout << "  No fishing in June and July (monsoon ban)!" << endl << endl;

    string name;
    cout << "  Enter your name, Captain: ";
    getline(cin, name);

    cout << "\n  1. Sail Boat - no fuel, 6 crates, cant sail in storms\n";
    cout << "  2. Motor Boat - 60 L tank, 10 crates\n";
    cout << "  3. Hybrid Boat - 40 L tank, 12 crates, uses less fuel\n";
    cout << "  Choose your boat (anything else = hybrid): ";
    int type;
    cin >> type;
    cin.ignore(1000, '\n');

    cout << "\n  Building your boat at the Vasco shipyard..." << endl;
    Boat *boat;
    if (type == 1)
        boat = new SailBoat();
    else if (type == 2)
        boat = new MotorBoat();
    else
        boat = new HybridBoat();

    Captain captain(name, 5000);
    Captain seasonStart(captain); // copy, used for profit at the end
    FishHold hold(boat->getHoldSize());

    // new game so start a fresh logbook
    ofstream my_file("logbook.txt");
    my_file << "===== Logbook of Captain " << name << " =====" << endl;
    my_file.close();

    bool running = true;
    while (running && !captain.seasonOver()) {
        cout << endl;
        line();
        cout << "  Week " << captain.getWeek() << " of " << MONTHS[captain.getMonth()] << "  |  Rs "
             << captain.getMoney() << "  |  Hold " << hold.count() << "/" << boat->getHoldSize() << endl;
        line();
        cout << "  1. Go fishing\n  2. Sell catch (and refuel)\n  3. Rest for a week\n  4. Captain's license\n  0. Quit\n";
        cout << "  Choice: ";

        try {
            int choice = readChoice(0, 4);

            if (choice == 1) {
                for (int z = 1; z <= 3; z++)
                    cout << "  " << z << ". " << ZONES[z] << " (" << DISTANCE[z] << " km, fuel "
                         << boat->fuelNeeded(DISTANCE[z] * 2) << " L)" << endl;
                cout << "  Where to? ";
                int zone = readChoice(1, 3);
                goFishing(captain, boat, hold, zone);
            }
            else if (choice == 2)
                sellCatch(captain, boat, hold);
            else if (choice == 3) {
                cout << "  You rest at home in Panjim for a week." << endl;
                writeLog(captain, "Rested. Money = Rs ", captain.getMoney());
                ++captain;
            }
            else if (choice == 4) {
                printLicense(captain);
                cout << "  Total crates caught : " << Fish::getTotalCaught() << endl;
            }
            else
                running = false;
        }
        catch (int code) {
            showError(code);
        }
        catch (...) {
            cout << "  >> Something unexpected went wrong." << endl;
        }
    }

    cout << endl << "  END OF VOYAGE" << endl;
    cout << "  Started with Rs " << seasonStart.getMoney() << ", finished with Rs " << captain.getMoney() << endl;
    if (captain.getMoney() >= GOAL)
        cout << "  GOAL REACHED - Legend of the Goa coast!" << endl;
    else
        cout << "  Goal not reached. Better luck next season!" << endl;

    delete boat; // destructors get called in reverse order
    return 0;
}
