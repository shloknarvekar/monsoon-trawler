// Monsoon Trawler - fishing boat game set in Goa
// OOP mini project (C++)
//
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
const int START_MONEY = 5000;

string MONTHS[14] = {"", "January", "February", "March", "April", "May", "June", "July",
    "August", "September", "October", "November", "December", "Season Over"};
string WEATHER[3] = {"Calm seas", "Windy", "STORM"};
string ZONES[4] = {"", "Aguada Coast (near shore)", "Deep Sea", "Grande Island"};
int DISTANCE[4] = {0, 10, 30, 50};

// error messages, index = error number thrown
string ERRORS[10] = {
    "Invalid choice! Please enter a number from the menu.",
    "MONSOON FISHING BAN (June-July)! Fish are breeding, boats must stay in harbour.",
    "Not enough fuel for this trip! Buy fuel first.",
    "STORM! Too dangerous for a sail boat. You waited at the jetty all week.",
    "Fish hold is FULL!",
    "Not enough money!",
    "Sail boats run on wind - they do not need fuel!",
    "Your fish hold is empty - nothing to sell.",
    "Could not open the file.",
    "Save file is corrupted."
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

void line(int n) {
    for (int i = 0; i < n; i++)
        cout << "=";
    cout << endl;
}

template <class newtype>
newtype getMax(newtype a, newtype b) {
    if (a > b)
        return a;
    return b;
}

// best fish sold so far
struct Trophy {
    string name;
    double weight;
    int value;
};


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
        name = "Unknown";
        localName = "?";
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

    string getName() { return name; }
    double getWeight() { return weight; }

    // compare two fish by value
    bool operator>(Fish &f2) {
        return this->value() > f2.value();
    }

    static int getTotalCaught() { return totalCaught; }
    static void setTotalCaught(int n) { totalCaught = n; }
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

class Prawns : public Fish {
public:
    Prawns(double w) {
        name = "Prawns";
        localName = "Sungta";
        weight = w;
    }
    int pricePerKg() { return 450; }
};

class Kingfish : public Fish {
public:
    Kingfish(double w) {
        name = "Kingfish";
        localName = "Visvan";
        weight = w;
    }
    int pricePerKg() { return 700; }
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
    string name;
    string type;
    double fuel;
    double tankSize; // 0 for sail boat
    int holdSize;

public:
    Boat() {
        cout << "  Boat constructor -> hull is ready" << endl;
        name = "Unnamed";
        type = "Boat";
        fuel = 0;
        tankSize = 0;
        holdSize = 0;
    }

    // virtual so child destructors also run when we delete a Boat pointer
    virtual ~Boat() {
        cout << "  Boat destructor -> hull taken apart" << endl;
    }

    virtual double fuelNeeded(int km) = 0;
    virtual bool canSailIn(int weather) = 0;
    virtual int netsPerTrip(int weather) = 0;
    virtual int typeCode() = 0;

    void setName(string n) { name = n; }
    void setFuel(double f) { fuel = f; }
    void useFuel(double f) { fuel = fuel - f; }
    void addFuel(double f) { fuel = fuel + f; }
    string getName() { return name; }
    double getFuel() { return fuel; }
    double getTankSize() { return tankSize; }
    int getHoldSize() { return holdSize; }

    void showInfo() {
        cout << "  Boat : " << name << " [" << type << "]" << endl;
        if (tankSize == 0)
            cout << "  Fuel : not needed (runs on wind)" << endl;
        else
            cout << "  Fuel : " << fuel << " / " << tankSize << " litres" << endl;
        cout << "  Hold : " << holdSize << " crates" << endl;
    }
};

class SailBoat : virtual public Boat {
public:
    SailBoat() {
        cout << "  SailBoat constructor -> mast and sails fitted" << endl;
        type = "Sail Boat";
        tankSize = 0;
        fuel = 0;
        holdSize = 6;
    }
    ~SailBoat() {
        cout << "  SailBoat destructor -> sails removed" << endl;
    }
    double fuelNeeded(int km) { return 0; }
    bool canSailIn(int weather) { return weather != 2; } // cant go in storm
    int netsPerTrip(int weather) {
        if (weather == 1)
            return 4; // extra net when windy
        return 3;
    }
    int typeCode() { return 1; }
};

class MotorBoat : virtual public Boat {
public:
    MotorBoat() {
        cout << "  MotorBoat constructor -> diesel engine fitted" << endl;
        type = "Motor Boat";
        tankSize = 60;
        fuel = 60;
        holdSize = 10;
    }
    ~MotorBoat() {
        cout << "  MotorBoat destructor -> engine removed" << endl;
    }
    double fuelNeeded(int km) { return km * 0.5; }
    bool canSailIn(int weather) { return true; }
    int netsPerTrip(int weather) { return 4; }
    int typeCode() { return 2; }
};

class HybridBoat : public SailBoat, public MotorBoat {
public:
    HybridBoat() {
        cout << "  HybridBoat constructor -> sails + engine combined!" << endl;
        type = "Hybrid Boat";
        tankSize = 40;
        fuel = 40;
        holdSize = 12;
    }
    ~HybridBoat() {
        cout << "  HybridBoat destructor -> hybrid system removed" << endl;
    }
    // have to override these again otherwise its ambiguous
    // (both SailBoat and MotorBoat have their own version)
    double fuelNeeded(int km) { return km * 0.25; }
    bool canSailIn(int weather) { return true; }
    int netsPerTrip(int weather) { return 5; }
    int typeCode() { return 3; }
};

Boat* createBoat(int type) {
    if (type == 1)
        return new SailBoat();
    if (type == 2)
        return new MotorBoat();
    return new HybridBoat();
}


// ---------- Fish hold ----------

class FishHold {
private:
    vector<Fish*> crates;
    int capacity;

public:
    FishHold() { capacity = 0; }
    ~FishHold() { emptyHold(); }

    void setCapacity(int c) { capacity = c; }
    int getCapacity() { return capacity; }
    int count() { return crates.size(); }

    void add(Fish *f) {
        if (count() >= capacity) {
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

    Fish* best() {
        Fish *b = crates.at(0);
        for (Fish *f : crates)
            if (*f > *b)
                b = f;
        return b;
    }

    void show() {
        if (count() == 0) {
            cout << "  (hold is empty)" << endl;
            return;
        }
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

class Market;

class Captain {
private:
    string name;
    int money;
    int week; // 1-4
    int month; // 1-12

public:
    Captain();
    Captain(string name, int money);
    Captain(const Captain &c);

    string getName();
    int getMoney();
    int getWeek();
    int getMonth();
    bool monsoonBan();
    bool seasonOver();
    void showDate();
    void operator++();
    void save(ofstream &my_file);
    void load(ifstream &my_file);

    friend void printLicense(Captain c);
    friend class Market;
};

Captain::Captain() {
    name = "Unnamed";
    money = START_MONEY;
    week = 1;
    month = 4; // starts in april
}

Captain::Captain(string name, int money) {
    this->name = name;
    this->money = money;
    this->week = 1;
    this->month = 4;
}

Captain::Captain(const Captain &c) {
    name = c.name;
    money = c.money;
    week = c.week;
    month = c.month;
}

string Captain::getName() { return name; }
int Captain::getMoney() { return money; }
int Captain::getWeek() { return week; }
int Captain::getMonth() { return month; }

bool Captain::monsoonBan() {
    return month == 6 || month == 7;
}

bool Captain::seasonOver() {
    return month > 12;
}

void Captain::showDate() {
    cout << "Week " << week << " of " << MONTHS[month];
}

// next week
void Captain::operator++() {
    week++;
    if (week > 4) {
        week = 1;
        month++;
    }
}

void Captain::save(ofstream &my_file) {
    my_file << name << endl;
    my_file << money << " " << week << " " << month << endl;
}

void Captain::load(ifstream &my_file) {
    getline(my_file, name);
    my_file >> money >> week >> month;
}

void printLicense(Captain c) {
    line(50);
    cout << "     GOA FISHERIES DEPARTMENT - CAPTAIN LICENSE" << endl;
    line(50);
    cout << "  Captain : " << c.name << endl;
    cout << "  Money : Rs " << c.money << "   (goal: Rs " << GOAL << ")" << endl;
    cout << "  Date : Week " << c.week << " of " << MONTHS[c.month] << endl;
}


// ---------- logbook file ----------

void writeLog(Captain &c, string event, int number) {
    ofstream my_file("logbook.txt", ios::app);
    if (!my_file)
        throw 8;
    my_file << "Week " << c.getWeek() << " of " << MONTHS[c.getMonth()] << " : " << event << number << endl;
    my_file.close();
}

void showLogbook() {
    ifstream my_file("logbook.txt");
    if (!my_file)
        throw 8;
    string entry;
    while (!my_file.eof()) {
        getline(my_file, entry);
        cout << "  " << entry << endl;
    }
    my_file.close();
}


// ---------- Market ----------
// friend class of Captain so it can change money directly

class Market {
public:
    void sell(Captain &c, FishHold &hold, Trophy &t);
    void buyFuel(Captain &c, Boat *boat);
};

void Market::sell(Captain &c, FishHold &hold, Trophy &t) {
    if (hold.count() == 0)
        throw 7;

    int rate = randomNumber(90, 120); // price changes every time
    cout << "  Panjim fish market is paying " << rate << "% of normal price today." << endl;
    line();
    hold.show();
    line();

    int earned = hold.totalValue() * rate / 100;

    Fish *b = hold.best();
    if (b->value() > t.value) {
        cout << "  NEW TROPHY CATCH: " << b->getName() << " worth Rs " << b->value() << "!" << endl;
        t.name = b->getName();
        t.weight = b->getWeight();
        t.value = b->value();
    }

    c.money = c.money + earned;
    cout << "  You sold " << hold.count() << " crate(s) and earned Rs " << earned << endl;
    writeLog(c, "Sold catch at Panjim market for Rs ", earned);
    hold.emptyHold();
}

void Market::buyFuel(Captain &c, Boat *boat) {
    if (boat->getTankSize() == 0)
        throw 6;

    double space = boat->getTankSize() - boat->getFuel();
    cout << "  Fuel price: Rs " << FUEL_PRICE << " per litre. Tank has space for " << space << " litres." << endl;
    cout << "  How many litres? ";
    int litres;
    cin >> litres;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
        throw 0;
    }
    cin.ignore(1000, '\n');
    if (litres <= 0)
        throw 0;
    if (litres > space)
        litres = space;

    int cost = litres * FUEL_PRICE;
    if (cost > c.money)
        throw 5;

    c.money = c.money - cost;
    boat->addFuel(litres);
    cout << "  Bought " << litres << " litres for Rs " << cost << endl;
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
        if (r < 45) return new Mackerel(w);
        if (r < 75) return new Prawns(w);
        if (r < 90) return new BlackPomfret(w);
        throw 'E';
    }
    else if (zone == 2) {
        if (r < 25) return new Mackerel(w);
        if (r < 50) return new Pomfret(w);
        if (r < 70) return new BlackPomfret(w);
        if (r < 90) return new Kingfish(w);
        throw 'E';
    }
    else {
        if (r < 45) return new Kingfish(w);
        if (r < 75) return new Pomfret(w);
        if (r < 88) return new BlackPomfret(w);
        throw 'S';
    }
}

int goFishing(Captain &c, Boat *boat, FishHold &hold, int zone) {
    static int tripNumber = 0;
    int caught = 0;
    bool weekUsed = false;

    try {
        if (c.monsoonBan())
            throw 1;

        if (hold.count() >= hold.getCapacity())
            throw 4;

        double fuel = boat->fuelNeeded(DISTANCE[zone] * 2); // going and coming back
        if (fuel > boat->getFuel())
            throw 2;

        weekUsed = true;
        tripNumber++;
        int weather = randomNumber(0, 99);
        if (weather < 50)
            weather = 0;
        else if (weather < 85)
            weather = 1;
        else
            weather = 2;

        cout << "  Trip #" << tripNumber << "  |  Weather report: " << WEATHER[weather] << endl;

        if (!boat->canSailIn(weather))
            throw 3;

        boat->useFuel(fuel);
        cout << "  Sailing " << DISTANCE[zone] << " km to " << ZONES[zone] << "..." << endl;

        int nets = boat->netsPerTrip(weather);
        if (weather == 2) {
            nets = nets - 2;
            cout << "  Rough seas! You can only throw " << nets << " nets." << endl;
        }
        line();

        for (int i = 1; i <= nets; i++) {
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
                    cout << "Empty net... nothing this time." << endl;
            }
            catch (int code) {
                cout << "Hold is full, heading back early!" << endl;
                throw; // rethrow, outer catch will handle it
            }
        }
        line();
        cout << "  Back at Panjim jetty with " << caught << " new crate(s)." << endl;
    }
    catch (int code) {
        showError(code);
    }

    if (weekUsed) {
        writeLog(c, "Fishing trip to " + ZONES[zone] + ", crates caught = ", caught);
        ++c;
    }
    return caught;
}


// ---------- save / load ----------

void saveGame(Captain &c, Boat *boat, Trophy &t) {
    ofstream my_file("trawler_save.txt");
    if (!my_file)
        throw 8;
    c.save(my_file);
    my_file << boat->typeCode() << " " << boat->getFuel() << " " << Fish::getTotalCaught() << endl;
    my_file << boat->getName() << endl;
    my_file << t.value << " " << t.weight << endl;
    my_file << t.name << endl;
    my_file.close();
}

Boat* loadGame(Captain &c, Trophy &t) {
    ifstream my_file("trawler_save.txt");
    if (!my_file)
        throw 8;

    int type, total;
    double fuel;
    string boatName;

    c.load(my_file);
    my_file >> type >> fuel >> total;
    my_file.ignore();
    getline(my_file, boatName);
    my_file >> t.value >> t.weight;
    my_file.ignore();
    getline(my_file, t.name);
    my_file.close();

    if (type < 1 || type > 3)
        throw 9;

    Boat *boat = createBoat(type);
    boat->setName(boatName);
    boat->setFuel(fuel);
    Fish::setTotalCaught(total);
    return boat;
}


int main() {
    srand(time(0));

    Captain captain;
    Boat *boat;
    FishHold hold;
    Market market;
    Trophy trophy;
    trophy.name = "none";
    trophy.weight = 0;
    trophy.value = 0;

    int bestTrip = 0;
    int bestSale = 0;

    line(50);
    cout << "            M O N S O O N   T R A W L E R" << endl;
    cout << "          ~ a Goa fishing boat simulator ~" << endl;
    line(50);
    cout << "  Earn Rs " << GOAL << " before the season ends in December." << endl;
    cout << "  Remember: NO fishing in June and July (monsoon ban)!" << endl << endl;

    bool started = false;
    while (!started) {
        cout << "  1. New game" << endl;
        cout << "  2. Load saved game" << endl;
        cout << "  0. Exit" << endl;
        cout << "  Choice: ";
        try {
            int choice = readChoice(0, 2);
            if (choice == 0)
                return 0;

            if (choice == 1) {
                string name, boatName;
                cout << "\n  Enter your name, Captain: ";
                getline(cin, name);
                cout << "  Name your boat: ";
                getline(cin, boatName);

                cout << "\n  Choose your boat:" << endl;
                cout << "  1. Sail Boat - no fuel needed, 6 crates, cannot sail in storms" << endl;
                cout << "  2. Motor Boat - 60 L tank, 10 crates, sails in any weather" << endl;
                cout << "  3. Hybrid Boat - 40 L tank, 12 crates, sails + engine" << endl;
                cout << "  Choice: ";
                int type = readChoice(1, 3);

                captain = Captain(name, START_MONEY);
                cout << "\n  Building your boat at the Vasco shipyard..." << endl;
                boat = createBoat(type);
                boat->setName(boatName);

                // new game so start a fresh logbook
                ofstream my_file("logbook.txt");
                my_file << "===== Logbook of Captain " << name << " =====" << endl;
                my_file.close();
            }
            else {
                boat = loadGame(captain, trophy);
                cout << "  Game loaded! Welcome back, Captain " << captain.getName() << "." << endl;
            }
            started = true;
        }
        catch (int code) {
            showError(code);
            cout << endl;
        }
    }

    hold.setCapacity(boat->getHoldSize());
    Captain seasonStart(captain); // copy, used for profit at the end
    bool goalShown = false;

    bool running = true;
    while (running) {
        if (captain.seasonOver()) {
            cout << "\n  The fishing season is over!" << endl;
            break;
        }
        if (!goalShown && captain.getMoney() >= GOAL) {
            line(50);
            cout << "  CONGRATULATIONS! You reached the Rs " << GOAL << " goal!" << endl;
            cout << "  You can keep fishing until December." << endl;
            line(50);
            goalShown = true;
        }

        cout << endl;
        line(50);
        cout << "  ";
        captain.showDate();
        cout << "   |   Rs " << captain.getMoney() << "   |   Hold " << hold.count() << "/" << hold.getCapacity() << endl;
        if (captain.monsoonBan())
            cout << "  *** MONSOON SEASON - fishing ban in effect ***" << endl;
        line(50);
        cout << "  1. Set sail (go fishing)" << endl;
        cout << "  2. View fish hold" << endl;
        cout << "  3. Sell catch at Panjim market" << endl;
        cout << "  4. Buy fuel" << endl;
        cout << "  5. Rest for a week" << endl;
        cout << "  6. Captain's license and records" << endl;
        cout << "  7. Read logbook" << endl;
        cout << "  8. Save game" << endl;
        cout << "  0. Quit" << endl;
        cout << "  Choice: ";

        try {
            int choice = readChoice(0, 8);

            if (choice == 1) {
                cout << "\n  Where do you want to fish?" << endl;
                for (int z = 1; z <= 3; z++) {
                    cout << "  " << z << ". " << ZONES[z] << "  (" << DISTANCE[z] << " km, fuel needed: "
                         << boat->fuelNeeded(DISTANCE[z] * 2) << " L)" << endl;
                }
                cout << "  0. Back" << endl;
                cout << "  Choice: ";
                int zone = readChoice(0, 3);
                if (zone != 0) {
                    int caught = goFishing(captain, boat, hold, zone);
                    bestTrip = getMax(bestTrip, caught);
                }
            }
            else if (choice == 2) {
                cout << "\n  YOUR FISH HOLD" << endl;
                line();
                hold.show();
            }
            else if (choice == 3) {
                int before = captain.getMoney();
                market.sell(captain, hold, trophy);
                bestSale = getMax(bestSale, captain.getMoney() - before);
            }
            else if (choice == 4) {
                market.buyFuel(captain, boat);
            }
            else if (choice == 5) {
                if (captain.monsoonBan())
                    cout << "  You repair your nets while the monsoon rain pours..." << endl;
                else
                    cout << "  You rest at home in Panjim for a week." << endl;
                writeLog(captain, "Rested in harbour. Money = Rs ", captain.getMoney());
                ++captain;
            }
            else if (choice == 6) {
                printLicense(captain);
                boat->showInfo();
                line();
                cout << "  Total fish crates caught : " << Fish::getTotalCaught() << endl;
                cout << "  Most crates in one trip : " << bestTrip << endl;
                cout << "  Biggest single sale : Rs " << bestSale << endl;
                cout << "  Trophy catch : " << trophy.name;
                if (trophy.value > 0)
                    cout << " (" << trophy.weight << " kg, Rs " << trophy.value << ")";
                cout << endl;
            }
            else if (choice == 7) {
                cout << "\n  CAPTAIN'S LOGBOOK" << endl;
                line();
                showLogbook();
            }
            else if (choice == 8) {
                saveGame(captain, boat, trophy);
                cout << "  Game saved to trawler_save.txt" << endl;
                if (hold.count() > 0)
                    cout << "  (Note: unsold fish in the hold are not saved.)" << endl;
            }
            else {
                running = false;
            }
        }
        catch (int code) {
            showError(code);
        }
        catch (...) {
            cout << "  >> Something unexpected went wrong." << endl;
        }
    }

    cout << endl;
    line(50);
    cout << "                 END OF VOYAGE" << endl;
    line(50);
    cout << "  Captain " << captain.getName() << endl;
    cout << "  Started with : Rs " << seasonStart.getMoney() << endl;
    cout << "  Finished with : Rs " << captain.getMoney() << endl;
    cout << "  Profit : Rs " << captain.getMoney() - seasonStart.getMoney() << endl;
    cout << "  Trophy catch : " << trophy.name << endl;
    if (captain.getMoney() >= GOAL)
        cout << "  Result : GOAL REACHED - Legend of the Goa coast!" << endl;
    else
        cout << "  Result : Goal not reached. Better luck next season!" << endl;
    line(50);

    cout << "\n  Docking the boat for the last time..." << endl;
    delete boat; // destructors get called in reverse order
    return 0;
}
