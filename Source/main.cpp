#include <algorithm>     // erase, remove
#include <ctime>         // time
#include <iomanip>       // put_time c++11 (alternative: asctime)
#include <iostream>      // cout
#include <sstream>       // to_string c++11 (alternative: stringstream)
#include <fstream>
#include "aps.h"
using namespace std;

#define DTFORMAT "%d-%m-%Y %H:%M:%S"
const string ADMIN_FILE = "admin.dat";
const string SALES_FILE = "sales.dat";

// Real Time Embedded System
// 1) Admin (must have username & password) can
//     - show all lots - visually (available | taken)
//     - show all data in files - list (lotNo | plateNo | dateTimeIn)
//     - sort data based on (lotNo | plateNo | dateTimeIn)
//     - search plate by lot no
//     - search lot by plate no
//     - view total sales
//     - shutdown/exit program
// 2) User can
//     - Enter Car: put car -> enter plateNo & PIN -> give no receipt (that's all)
//     - Take Car: enter plateNo & PIN -> give receipt -> pay -> get car

struct MyVehicle {
    string plateNo, pinNo, lotNo, vehicleType;
    time_t dateTimeIn, dateTimeOut;
    double duration, charges;
};

void initAdmin();
bool validateAdmin();
double calcTotalSales(double);
void userMenu();
void userFeatures(int);
void showReceipt(MyVehicle);
void adminMenu();
void adminFeatures(int, int);
int inputOption(int, int);
void showAtTop();
void showAllParkingLots(string, string *, int);
void showAllDetails(string, string *, string *, time_t *, int);
void pauseScreen();
void clearScreen();


int main()
{
    // Initialize admin for the program
    initAdmin();
    clearScreen();
    int opt;

    // Need admin credential to start the system
    cout << "This system need admin previlege to start..\n";
    if (!validateAdmin()) return 0;

    while (true) {
        showAtTop();
        cout << "MAIN MENU" << endl
             << "1. User" << endl
             << "2. Admin" << endl
             << "Select option (1/2): ";
        opt = inputOption(1, 2);
        switch (opt) {
            case 1: userMenu(); break;
            case 2: adminMenu(); break;
        }
    }

    return 0;
}


void initAdmin() {
    // Return if admin file exist
    ifstream rAdminFile(ADMIN_FILE.c_str());
    if (rAdminFile.good()) {
        rAdminFile.close();
        return;
    }

    // Else, get the admin credential
    string username, password, cPassword;
    do {
        clearScreen();
        cout << "\tWELCOME TO IBN-BAJJAH AUTO PARKING SYSTEM\n"
            << string(57, '=') << "\n\n"
            << "ADMIN REGISTRATION\n";
        cout << "Username: "; getline(cin, username);
        cout << "Password: "; getline(cin, password);
        cout << "Confirm Password: "; getline(cin, cPassword);
        if (password.compare(cPassword) != 0) {
            cout << "Password do not match. Please try again.\n";
            pauseScreen();
        }
    } while (password.compare(cPassword) != 0);
    
    // And write it to a new admin file
    ofstream cAdminFile(ADMIN_FILE.c_str());
    if (cAdminFile.good()) {
        cAdminFile << username << ' ' << password << '\n';
        cout << username << " has been registered as an administrator.\n";
    }
    cAdminFile.close();

    pauseScreen();
}


bool validateAdmin() {
    string corrUname, corrPword;
    string uName, pWord;

    cout << "\nEnter Admin Username: ";
    getline(cin, uName);
    cout << "Enter Admin Password: ";
    getline(cin, pWord);

    ifstream rAdminFile(ADMIN_FILE.c_str());
    if (rAdminFile.good())
        rAdminFile >> corrUname >> corrPword;
    rAdminFile.close();

    if (uName.compare(corrUname) != 0 || pWord.compare(corrPword) != 0) {
        cout << "Invalid username or password." << endl;
        pauseScreen();
        return false;
    }

    return true;
}


double calcTotalSales(double totalCharges) {
    double totalSales = 0.0;

    // Read sales file & get the current sales
    ifstream rSalesFile(SALES_FILE.c_str());
    if (rSalesFile.good()) {
        rSalesFile >> totalSales;
    } else {
        // If cannot read, create the file
        ofstream cSalesFile(SALES_FILE.c_str());
        cSalesFile << totalSales;
        cSalesFile.close();
    }
    rSalesFile.close();

    // Increment totalSales and re-write the file
    ofstream wSalesFile(SALES_FILE.c_str());
    if (wSalesFile.good()) {
        totalSales += totalCharges;
        wSalesFile << totalSales;
    }
    wSalesFile.close();

    return totalSales;
}


void userMenu() {
    int opt;
    char ans;
    while (true) {
        showAtTop();
        cout << "USER MENU\n"
             << "1. Park/Unpark Car\n"
             << "2. Park/Unpark Motorcycle\n"
             << "3. Back to main menu\n"
             << "Select option (1-3): ";
        opt = inputOption(1, 3);
        if (opt == 3) return;
        userFeatures(opt);
    }
}


void userFeatures(int opt) {
    string plateNo, pinNo;
    AutoParkingSystem userVeh;

    // Get inputs & validate it
    cout << "\nEnter vehicle plate no: ";
    getline(cin, plateNo);
    cout << "Enter 6 digits PIN no: ";
    getline(cin, pinNo);

    userVeh.setPlateNo(plateNo);
    userVeh.setPinNo(pinNo);
    switch (opt) {
        case 1: userVeh.setVehicleType("CAR"); break;
        case 2: userVeh.setVehicleType("MOTORCYCLE"); break;
    }

    userVeh.readFile();
    
    if (!userVeh.validateInput()) {
        pauseScreen();
        return;
    }

    userVeh.writeFile();

    // Get the formatted data
    MyVehicle veh;
    veh.plateNo     = userVeh.getPlateNo();
    veh.pinNo       = userVeh.getPinNo();
    veh.lotNo       = userVeh.getLotNo();
    veh.vehicleType = userVeh.getVehicleType();

    // Show receipt only for unpark the car & correct pin
    if (!userVeh.isNewPlateNo()) {
        if (userVeh.isCorrectPinNo()) {
            veh.dateTimeIn  = userVeh.getDateTimeIn();
            veh.dateTimeOut = userVeh.getDateTimeOut();
            veh.duration    = userVeh.getDuration();
            veh.charges     = userVeh.getCharges();
            showReceipt(veh);
            double tSales = calcTotalSales(veh.charges);
            cout << "\n\tThanks for using IBAPS\n";
        }
    } else {
        cout << "Your vehicle, " << veh.plateNo
             << " will be moved to the empty parking lot.\n"
             << "Please remember your PIN number."
             << endl;
    }
    
    pauseScreen();
}


void adminMenu() {
    // Only admin can pass
    if (!validateAdmin()) return;

    double tSales;
    int opt1, opt2;
    while (true) {
        showAtTop();
        // Admin menu
        cout << "ADMIN MENU\n"
             << "1. See car data\n"
             << "2. See motorcycle data\n"
             << "3. View total sales\n"
             << "4. Back to main menu\n"
             << "5. Exit/Shutdown the system (BE CAREFUL)\n"
             << "Select option (1-5): ";
        opt1 = inputOption(1, 5);
        if (opt1 == 4) return;

        // Exit the program
        if (opt1 == 5) {
            cout << "\nPlease confirm that you really want to exit the\n"
                 << "system by inserting admin username and password.\n";
            if (validateAdmin()) exit(0);
            else continue;
        }
        
        // Show total sales
        if (opt1 == 3) {
            tSales = calcTotalSales(0.0);
            cout << "\nTotal Sales: RM " << tSales << endl;
            pauseScreen();
            continue;
        }

        // Motorcycle/Car menu
        do {
            showAtTop();
            if (opt1 == 1) cout << "CAR MENU\n";
            else if (opt1 == 2) cout << "MOTORCYCLE MENU\n";
            cout << "1. Show all parking lots\n"
                 << "2. Show all details sorted by plate no\n"
                 << "3. Show all details sorted by lot no\n"
                 << "4. Show all details sorted by date time in\n"
                 << "5. Search plate no by lot no\n"
                 << "6. Search lot no by plate no\n"
                 << "7. Back to admin menu\n"
                 << "Select option (1-7): ";
            opt2 = inputOption(1, 7);
            if (opt2 != 7) adminFeatures(opt1, opt2);
        } while (opt2 != 7);
    }
}


void adminFeatures(int vehicle, int opt) {
    u32 totalLines;
    string searchLN, searchPN, getLN, getPN;
    string *ptrLN, *ptrPN, veh;
    time_t *ptrDTI;

    showAtTop();
    AutoParkingSystem adminVeh;

    // Set the type of vehicle first
    switch (vehicle) {
        case 1: veh = "CAR";
                adminVeh.setVehicleType(veh);
                break;
        case 2: veh = "MOTORCYCLE";
                adminVeh.setVehicleType(veh);
                break;
    }

    adminVeh.readFile();

    // Get total number of vehicle in the file
    totalLines = adminVeh.getTotalLines();

    // Sort first before get the sorted data
    switch (opt) {
        case 2: adminVeh.sortByPlateNo(); break;
        case 3: adminVeh.sortByLotNo(); break;
        case 4: adminVeh.sortByDateTimeIn(); break;
    }

    // Allocate memory & point a pointer to it
    if (opt == 1 || opt == 2 || opt == 3 || opt == 4) {
        ptrLN = adminVeh.getAllLotNo(new string[totalLines]);
        ptrPN = adminVeh.getAllPlateNo(new string[totalLines]);
        ptrDTI = adminVeh.getAllDateTimeIn(new time_t[totalLines]);
    }

    switch (opt) {
        case 1: showAllParkingLots(veh, ptrLN, totalLines); break;
        case 2:
        case 3:
        case 4: showAllDetails(veh, ptrLN, ptrPN, ptrDTI, totalLines); break;
        case 5: cout << "Enter plate no that you want to search: ";
                getline(cin, searchPN);
                getLN = adminVeh.getLotByPlateNo(searchPN);
                cout << searchPN;
                if (getLN.compare("N/A") == 0) cout << " is not available.\n";
                else cout << " located at lot no " << getLN << ".\n";
                break;
        case 6: cout << "Enter lot no that you want to search: ";
                getline(cin, searchLN);
                getPN = adminVeh.getPlateByLotNo(searchLN);
                cout << searchLN;
                if (getPN.compare("N/A") == 0) cout << " is not available.\n";
                else cout << " has plate no " << getPN << ".\n";
                break;
    }

    // Free memory that was allocated above
    if (opt == 1 || opt == 2 || opt == 3 || opt == 4) {
        delete[] ptrLN;
        delete[] ptrPN;
        delete[] ptrDTI;
    }

    pauseScreen();
}


int inputOption(int min, int max) {
    int opt;
    do {
        cin >> opt;
        cin.clear();
        cin.ignore(100, '\n');
        if (opt < min || opt > max)
            cout << "Invalid option. Please re-enter: ";
    } while (opt < min || opt > max);
    return opt;
}


void showAtTop() {
    clearScreen();
    u32 totalCars, totalMoto;

    AutoParkingSystem car;
    car.setVehicleType("CAR");
    car.readFile();
    totalCars = car.getTotalLines();

    AutoParkingSystem moto;
    moto.setVehicleType("MOTORCYCLE");
    moto.readFile();
    totalMoto = moto.getTotalLines();

    cout << "\tWELCOME TO IBN-BAJJAH AUTO PARKING SYSTEM\n"
         << string(57, '=') << "\n\n"
         << "Car: " << 100 - totalCars << "/100 parking left\n"
         << "Motorcycle: " << 100 - totalMoto << "/100 parking left\n\n";
}


void showReceipt(MyVehicle currVeh) {
    long int sec = (int)currVeh.duration;
    int min = sec / 60;
    int hr = min / 60;

    cout << endl;
    if (currVeh.vehicleType.compare("MOTORCYCLE") == 0)
        cout << "\t   MOTORCYCLE RECEIPT" << endl;
    else if (currVeh.vehicleType.compare("CAR") == 0)
        cout << "\t     CAR RECEIPT" << endl;
    
    cout << string(40, '=') << endl
         << setw(15) << left << "Plate No." << ": " << currVeh.plateNo << endl
         << setw(15) << left << "Parking Lot No." << ": " << currVeh.lotNo << endl
         << string(40, '-') << endl;

    cout << setw(15) << left << "Date/Time in" << ": "
        //  << asctime(localtime(&currVeh.dateTimeIn)) // put_time alternative
         << put_time(localtime(&currVeh.dateTimeIn), DTFORMAT)
         << endl;

    if (sec != 0) {
        cout << setw(15) << left << "Date/Time out" << ": "
            //  << asctime(localtime(&currVeh.dateTimeIn)) // put_time alternative
             << put_time(localtime(&currVeh.dateTimeOut), DTFORMAT)
             << endl;
        cout << setw(15) << left << "Duration" << ": "
             << hr << "h:"
             << min % 60 << "m:"
             << sec % 60 << "s"
             << endl;
        cout << setw(15) << left << "Total Charges" << ": RM"
             << setprecision(2) << fixed << currVeh.charges
             << endl;
    } else {
        cout << setw(15) << left << "Date/Time out" << ": N/A" << endl;
        cout << setw(15) << left << "Duration" << ": N/A" << endl;
        cout << setw(15) << left << "Total Charges" << ": N/A" << endl;
    }
    cout << string(40, '=') << endl;
}


void showAllParkingLots(string veh, string *ptPL, int size) {
    string lotNo;
    if (veh.compare("MOTORCYCLE") == 0)
        cout << "\t\tALL MOTORCYCLES PARKING LOTS" << endl;
    else if (veh.compare("CAR") == 0)
        cout << "\t\t   ALL CARS PARKING LOTS" << endl;

    cout << "\t\t Floor: A-J\tLot: 01-10\n";

    // stringstream ss; // to_string alternative
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            lotNo = (char)(i + 48 + 17);
            if (j != 9) lotNo += '0';
            lotNo += to_string(j + 1);
            // ss.str("");
            // ss << (j + 1);
            // lotNo += ss.str();
            for (int k = 0; k < size; ++k)
                if (ptPL[k].compare(lotNo) == 0)
                    lotNo = "[***]";
            if (lotNo.compare("[***]") != 0)
                lotNo = '[' + lotNo + ']';
            cout << lotNo << ' ';
        }
        cout << endl << endl;
    }
}


void showAllDetails(string veh, string *ptrLN, string *ptrPN,
                    time_t *ptrDTI, int size) {
    if (veh.compare("MOTORCYCLE") == 0)
        cout << "\tALL MOTORCYCLE DETAILS" << endl;
    else if (veh.compare("CAR") == 0)
        cout << "\t    ALL CARS DETAILS" << endl;

    cout << string(40, '=') << endl
         << setw(10) << left << "Lot-No"
         << setw(10) << left << "Plate-No"
         << setw(20) << left << "Date-Time-In" << endl
         << string(40, '-') << endl;
    for (int i = 0; i < size; ++i) {
        cout << setw(10) << left << ptrLN[i]
             << setw(10) << left << ptrPN[i]
             << setw(20) << left 
            //  << asctime(localtime(&ptrDTI[i])); // put_time alternative
             << put_time(localtime(&ptrDTI[i]), DTFORMAT) << endl;
    }
    cout << string(40, '=') << endl;
}


void pauseScreen() {
    char c;
    cout << "\nPress enter to continue..";
    cin.get(c);
}


void clearScreen() {
    system("clear"); // Unix
    // system("cls"); // Windows
    // cout << string(100, '\n'); // Multi-platform
}