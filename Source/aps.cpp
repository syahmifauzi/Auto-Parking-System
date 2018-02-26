#include <algorithm>     // erase, remove
#include <cctype>        // isdigit
#include <ctime>         // time
#include <fstream>       // fstream
#include <iostream>      // cout
#include <locale>        // toupper
#include <stdlib.h>
#include "aps.h"
using namespace std;

const string CAR_FILENAME = "apscar.dat";
const string MOTO_FILENAME = "apsmoto.dat";
const int TOTAL_FLOOR = 10;
const int TOTAL_LOT_PER_FLOOR = 10;
const int TOTAL_ALL_LOT = TOTAL_FLOOR * TOTAL_LOT_PER_FLOOR;


//  Constructor: Initialize all data members
//==============================================================
AutoParkingSystem::AutoParkingSystem() {
    this->plate_no = "N/A";
    this->pin_no = "N/A";
    this->lot_no = "N/A";
    this->vehicle_type = "N/A";
    this->date_time_in = time(NULL);
    this->date_time_out = time(NULL);
    this->duration = 0.0;
    this->total_charges = 0.0;
    this->total_lines = 0;
    this->new_plate_no = true;
    this->correct_pin = true;
}


// Set data members
//==============================================================
void AutoParkingSystem::setPlateNo(string plateNo) {
    this->plate_no = this->formatString(plateNo);
}
void AutoParkingSystem::setPinNo(string pinNo) {
    this->pin_no = this->formatString(pinNo);
}
void AutoParkingSystem::setLotNo(string lotNo) {
    this->lot_no = this->formatString(lotNo);
}
void AutoParkingSystem::setVehicleType(string vehicleType) {
    this->vehicle_type = this->formatString(vehicleType);
}


// Format string to uppercase with no space
//==============================================================
string AutoParkingSystem::formatString(string s) {
    int i;
    // Remove white spaces
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    // Convert to uppercase
    locale loc;
    for (i = 0; i < s.length(); ++i)
        s[i] = toupper(s[i], loc);
    // Return formatted string s or N/A if s is empty string
    if (i == 0)
        return "N/A";
    else
        return s;
}


bool AutoParkingSystem::validateInput() {
    // Set flags
    bool isPnSet = true,
         isPinSet = true,
         isValidPin1 = true,
         isValidPin2 = true,
         isVtSet = true,
         isValidVT = true;

    // Check if plate_no set or not
    if (this->plate_no.compare("N/A") == 0)
        isPnSet = false;

    // Check if pin_no set or not
    if (this->pin_no.compare("N/A") == 0)
        isPinSet = false;
    
    // Check if pin no not 6 characters
    if (this->pin_no.length() != 6)
        isValidPin1 = false;

    // Check if pin no not digit
    for (int i = 0; i < this->pin_no.length(); ++i) {
        if (!isdigit(this->pin_no[i])) {
            isValidPin2 = false;
            break;
        }
    }

    // Check if vehicle_type set or not
    if (this->vehicle_type.compare("N/A") == 0)
        isVtSet = false;

    // Check vehicle type if other than car/moto        
    if (isVtSet && (this->vehicle_type.compare("CAR") != 0 && 
        this->vehicle_type.compare("MOTORCYCLE") != 0))
            isValidVT = false;

    // Print out for invalid user input(s)
    if (!isPnSet)
        cout << "Set plate number first." << endl;
    if (!isPinSet)
        cout << "Set PIN number first." << endl;
    if (!isValidPin1)
        cout << "Invalid PIN number. Must be 6 digits." << endl;
    if (!isValidPin2)
        cout << "Invalid PIN number. Must be digits only." << endl;
    if (!isVtSet)
        cout << "Set vehicle type first." << endl;
    if (!isValidVT)
        cout << "Invalid vehicle type." << endl;

    // Return true if all user inputs are valid
    if (isPnSet && isPinSet && isValidPin1 &&
        isValidPin2 && isVtSet && isValidVT)
        return true;
    else
        return false;
}


// Read the file before doing anything to it
//==============================================================
void AutoParkingSystem::readFile() {
    // Set flags
    bool isMoto = false,
         isCar = false;
    if (this->vehicle_type.compare("MOTORCYCLE") == 0) isMoto = true;
    else if (this->vehicle_type.compare("CAR") == 0) isCar = true;

    // Read the file based on flags
    ifstream apsRF;
    if (isMoto) apsRF.open(MOTO_FILENAME.c_str());
    else if (isCar) apsRF.open(CAR_FILENAME.c_str());
    if (apsRF.good()) {
        // Get the current total lines of the file
        string tempLine;
        this->total_lines = 0;
        while (getline(apsRF, tempLine))
            ++this->total_lines;
            
        // Clear error flags & set pos. to the beg. of the fstream
        apsRF.clear();
        apsRF.seekg(0, apsRF.beg);
        
        // Allocate memory & store all file data to it
        this->trans = new Transport[this->total_lines];
        for (int i = 0; i < this->total_lines; ++i) {
            apsRF >> this->trans[i].lot_no
                  >> this->trans[i].plate_no
                  >> this->trans[i].date_time_in
                  >> this->trans[i].pin_no;
        }
    } else {
        // Create the file if it does not exist
        ofstream apsCF;
        if (isMoto) apsCF.open(MOTO_FILENAME.c_str());
        else if (isCar) apsCF.open(CAR_FILENAME.c_str());
        apsCF.close();
        // Allocate memory to prevent segementation fault
        // when delete the unallocated memory
        this->trans = new Transport[0];
        // cout << "Failed to read the file\n";
        // cout << "New file has been created.\n";
    }
    apsRF.close();
}


// Write new plate_no or remove old plate_no from the file
//==============================================================
void AutoParkingSystem::writeFile() {
    // Return if exceed max of total parking lot
    if (this->total_lines == TOTAL_ALL_LOT) {
        cout << "Sorry, no more parking lot. All full!" << endl;
        return;
    }
    // Check whether plate_no already exist in file or not
    this->new_plate_no = true;
    int idxMatch = -1;
    for (int i = 0; i < this->total_lines; ++i) {
        if (this->trans[i].plate_no.compare(this->plate_no) == 0) {
            // If it exist, get its index
            this->new_plate_no = false;
            idxMatch = i;
            // Return if incorrect pin_no
            if (this->trans[idxMatch].pin_no.compare(this->pin_no) != 0) {
                cout << "Sorry, invalid pin no!" << endl;
                this->correct_pin = false;
                return;
            }
            // Else continue
            break;
        }
    }
    // Set flags
    bool isMoto = false,
         isCar = false;
    if (this->vehicle_type.compare("MOTORCYCLE") == 0) isMoto = true;
    else if (this->vehicle_type.compare("CAR") == 0) isCar = true;
    // Append plate_no if new_plate_no, else rewrite the whole file
    ofstream apsWF;
    if (isMoto) {
        if (this->new_plate_no) apsWF.open(MOTO_FILENAME.c_str(), ios::app);
        else apsWF.open(MOTO_FILENAME.c_str());
    } else if (isCar) {
        if (this->new_plate_no) apsWF.open(CAR_FILENAME.c_str(), ios::app);
        else apsWF.open(CAR_FILENAME.c_str());
    }
    if (apsWF.good()) {
        if (this->new_plate_no) {
            AutoParkingSystem::genLotNo();
            apsWF << this->lot_no << ' '
                  << this->plate_no << ' ' 
                  << this->date_time_in << ' '
                  << this->pin_no << '\n';
        } else {
            for (int i = 0; i < this->total_lines; ++i) {
                if (i != idxMatch) {
                    apsWF << this->trans[i].lot_no << ' '
                          << this->trans[i].plate_no << ' '
                          << this->trans[i].date_time_in << ' '
                          << this->trans[i].pin_no << '\n';
                }
            }
        }
    } else {
        // cout << "Failed to write the file.\n";
    }
    apsWF.close();
    // Calculate duration & total_charges for old plate_no only
    if (!this->new_plate_no) {
        this->date_time_in = this->trans[idxMatch].date_time_in;
        this->lot_no = this->trans[idxMatch].lot_no;
        AutoParkingSystem::calcDuration();
        AutoParkingSystem::calcCharges();
    }
}


// Generate unique lot_no for new plate_no
//==============================================================
void AutoParkingSystem::genLotNo() {
    srand(time(NULL));
    int randFloor, randLot;
    string newLotNo;
    bool isAvailable;
    do {
        // (randNo between max & min) + '0' + (diff between '0' & 'A')
        randFloor = (rand() % TOTAL_FLOOR + 0) + 48 + 17;
        randLot = rand() % TOTAL_LOT_PER_FLOOR + 1;
        // Generate lot number for floor(alphabet) and lot(number)
        newLotNo = "";
        newLotNo += (char)randFloor;
        if (randLot < 10) newLotNo += '0';    
        newLotNo += to_string(randLot);
        // If newLotNo already exist in the file, regenerate it
        isAvailable = true;
        for (int i = 0; i < this->total_lines; ++i) {
            if (this->trans[i].lot_no.compare(newLotNo) == 0) {
                isAvailable = false;
                break;
            }
        }
        // If not, take the generated newLotNo
        if (isAvailable) break;
    } while (true);
    // Pass here? we got one
    this->lot_no = newLotNo;
}


// Calculate duration for old plate_no to calculate charges
//==============================================================
void AutoParkingSystem::calcDuration() {
    // Get the duration in seconds as doubles
    this->duration = difftime(this->date_time_out, this->date_time_in);
}


// TODO: ADD WEEKEND & WEEKDAYS RATES\
// Calculate charges for old plate_no based on duration
//==============================================================
void AutoParkingSystem::calcCharges() {
    this->total_charges = 0.0;
    double seconds = this->duration;
    double rates;
    // Set flags
    bool isMoto = false,
         isCar = false;
    if (this->vehicle_type.compare("MOTORCYCLE") == 0) isMoto = true;
    else if (this->vehicle_type.compare("CAR") == 0) isCar = true;

    if (isMoto) {
        // Motorcycle RM2 per day
        rates = 2.00;
        while (seconds > 0.0) {
            // Calculate/Increment/Add total_charges
            this->total_charges += rates;
            // Minus seconds by 1 day
            seconds -= 3600 * 24;
        }
    } else if (isCar) {
        // Loop 24 times for each day until no more seconds left
        while (seconds > 0.0) {
            for (int i = 0; i < 24 && seconds > 0.0; ++i) {
                // (1st - 3rd)hr, rates is RM4.50/hr
                if (i < 3) rates = 4.50;
                // (4th & 5th)hr, rates is RM3.50/hr
                else if (i < 5) rates = 3.50;
                // (6th - 9th)hr, rates is RM3.00/hr
                else if (i < 9) rates = 3.00;
                // (10th - 18th)hr, rates is RM2.00/hr
                else if (i < 18) rates = 2.00;
                // (19th - 24th)hr, free
                else rates = 0.00;
                // Calculate/Increment/Add total_charges
                this->total_charges += rates;
                // Minus seconds by 1 hour
                seconds -= 3600;
            }
        }
    }
}


// Swapping Algorithm
//==============================================================
void AutoParkingSystem::swapStr(string *s1, string *s2) {
    string temp = *s1;
    *s1 = *s2;
    *s2 = temp;
}
void AutoParkingSystem::swapTime(time_t *t1, time_t *t2) {
    time_t temp = *t1;
    *t1 = *t2;
    *t2 = temp;
}


// Sorting the file using Insertion Sort Algorithm
//==============================================================
void AutoParkingSystem::sortBy(string sortByWhat) {
    if (this->total_lines == 0) {
        cout << "No data in the file" <<endl;
        return;
    }
    // Set flags for sorting
    bool sortByPN = false,
         sortByLN = false,
         sortByDTI = false;
    if (sortByWhat.compare("PLATE_NO") == 0) sortByPN = true;
    else if (sortByWhat.compare("LOT_NO") == 0) sortByLN = true;
    else if (sortByWhat.compare("DATE_TIME_IN") == 0) sortByDTI = true;

    // Start sorting
    int idxMin, cond;
    for (int i = 0; i < this->total_lines - 1; ++i) {
        idxMin = i;
        for (int j = i + 1; j < this->total_lines; ++j) {
            // Sort by what condition??
            if (sortByPN)
                cond = this->trans[j].plate_no.compare(this->trans[idxMin].plate_no);
            else if (sortByLN)
                cond = this->trans[j].lot_no.compare(this->trans[idxMin].lot_no);
            else if (sortByDTI)
                cond = this->trans[j].date_time_in - this->trans[idxMin].date_time_in;
            // Whatever condition is, sort based on it
            if (cond < 0) {
                // Swap plate_no
                AutoParkingSystem::swapStr(
                    &this->trans[j].plate_no,
                    &this->trans[idxMin].plate_no
                );
                // Swap lot_no
                AutoParkingSystem::swapStr(
                    &this->trans[j].lot_no,
                    &this->trans[idxMin].lot_no
                );
                // Swap date_time_in
                AutoParkingSystem::swapTime(
                    &this->trans[j].date_time_in,
                    &this->trans[idxMin].date_time_in
                );
                // Swap pin_no
                AutoParkingSystem::swapStr(
                    &this->trans[j].pin_no,
                    &this->trans[idxMin].pin_no
                );
            }
        }
    }

    // Set flags for vehicle type
    bool isMoto = false,
         isCar = false;
    if (this->vehicle_type.compare("MOTORCYCLE") == 0) isMoto = true;
    else if (this->vehicle_type.compare("CAR") == 0) isCar = true;

    // Replace the data in the file with the sorted data
    ofstream sortFile;
    if (isMoto) sortFile.open(MOTO_FILENAME.c_str());
    else if (isCar) sortFile.open(CAR_FILENAME.c_str());
    if (sortFile.good()) {
        for (int i = 0; i < this->total_lines; ++i)
            sortFile << this->trans[i].lot_no << ' '
                     << this->trans[i].plate_no << ' '
                     << this->trans[i].date_time_in << ' '
                     << this->trans[i].pin_no << '\n';
        // cout << "Data has been sorted by " << sortByWhat << endl;
    } else {
        // cout << "Data failed to be sorted by " << sortByWhat << endl;
    }
    sortFile.close();
}


// Searching the file using Binary Search Algorithm
//==============================================================
string AutoParkingSystem::searchBy(string searchBy, string key) {
    // Set flags for what to find
    bool findLotNo = false,
         findPlateNo = false;
    if (searchBy.compare("PLATE_NO") == 0) findLotNo = true;
    else if (searchBy.compare("LOT_NO") == 0) findPlateNo = true;

    // Format key to be searched
    key = AutoParkingSystem::formatString(key);
    string foundKey = "N/A";

    // Start searching
    int first = 0, mid,
        last = this->total_lines - 1;
    while (first <= last) {
        mid = (first + last) / 2;
        if (findLotNo && key.compare(this->trans[mid].plate_no) == 0) {
            foundKey = this->trans[mid].lot_no;
            break;
        } else if (findPlateNo && key.compare(this->trans[mid].lot_no) == 0) {
            foundKey = this->trans[mid].plate_no;
            break;
        }
        if (findLotNo && key.compare(this->trans[mid].plate_no) > 0)
            first = mid + 1;
        else if (findLotNo)
            last = mid - 1;
        else if (findPlateNo && key.compare(this->trans[mid].lot_no) > 0)
            first = mid + 1;
        else if (findPlateNo)
            last = mid - 1;
    }
    
    // Return "N/A" if key not found
    return foundKey;
}


// Destructor: Free all allocated memories
//==============================================================
AutoParkingSystem::~AutoParkingSystem() {
    delete[] this->trans;
}


//==============================================================
//   Below this line, are the functions that can be directly  //
//    called after the instance of the object of this class   //
//     has been created. Most of them are appropriate to be   //
//          used after setPlateNo(plate_no, pin_no)           //
//==============================================================


// Get everything that data member store, return only one value
//==============================================================
string AutoParkingSystem::getPlateNo() {
    return this->plate_no;
}
string AutoParkingSystem::getLotNo() {
    return this->lot_no;
}
string AutoParkingSystem::getPinNo() {
    return this->pin_no;
}
string AutoParkingSystem::getVehicleType() {
    return this->vehicle_type;
}
time_t AutoParkingSystem::getDateTimeIn() {
    return this->date_time_in;
}
time_t AutoParkingSystem::getDateTimeOut() {
    return this->date_time_out;
}
double AutoParkingSystem::getDuration() {
    return this->duration;
}
double AutoParkingSystem::getCharges() {
    return this->total_charges;
}
u32 AutoParkingSystem::getTotalLines() {
    return this->total_lines;
}
bool AutoParkingSystem::isNewPlateNo() {
    return this->new_plate_no;
}
bool AutoParkingSystem::isCorrectPinNo() {
    return this->correct_pin;
}


// Get 1D array of all data in the file, return a pointer
// use: dtype *dname = obj.methodname(new dtype[totalLines]);
// and don't forget to: delete[] dname;
//==============================================================
string *AutoParkingSystem::getAllLotNo(string *s) {
    // Fill ALL lot_no to array s and return its pointer
    for (int i = 0; i < this->total_lines; ++i)
        s[i] = this->trans[i].lot_no;
    return s;
}
string *AutoParkingSystem::getAllPlateNo(string *s) {
    // Fill ALL plate_no to array s and return its pointer
    for (int i = 0; i < this->total_lines; ++i)
        s[i] = this->trans[i].plate_no;
    return s;
}
time_t *AutoParkingSystem::getAllDateTimeIn(time_t *t) {
    // Fill ALL date_time_in to array t and return its pointer
    for (int i = 0; i < this->total_lines; ++i)
        t[i] = this->trans[i].date_time_in;
    return t;
}


// These functions will determine sortBy(what) function
//==============================================================
void AutoParkingSystem::sortByPlateNo() {
    AutoParkingSystem::sortBy("PLATE_NO");
}
void AutoParkingSystem::sortByLotNo() {
    AutoParkingSystem::sortBy("LOT_NO");
}
void AutoParkingSystem::sortByDateTimeIn() {
    AutoParkingSystem::sortBy("DATE_TIME_IN");
}


// These functions will determine searchBy(what, key) function
//==============================================================
string AutoParkingSystem::getPlateByLotNo(string lotNo) {
    AutoParkingSystem::sortByLotNo();
    return AutoParkingSystem::searchBy("LOT_NO", lotNo);
}
string AutoParkingSystem::getLotByPlateNo(string plateNo) {
    AutoParkingSystem::sortByPlateNo();
    return AutoParkingSystem::searchBy("PLATE_NO", plateNo);
}