#include <string>
typedef unsigned int u32;

class AutoParkingSystem
{
    public:
        AutoParkingSystem();
        // Set data members
        void setPlateNo(std::string);
        void setPinNo(std::string);
        void setLotNo(std::string);
        void setVehicleType(std::string);
        // Must call these 3 methods in sequence
        bool validateInput();
        void readFile();
        void writeFile();
        // Get everything
        std::string getPlateNo();
        std::string getPinNo();
        std::string getLotNo();
        std::string getVehicleType();
        time_t getDateTimeIn();
        time_t getDateTimeOut();
        double getDuration();
        double getCharges();
        bool isNewPlateNo();
        bool isCorrectPinNo();
        u32 getTotalLines();
        std::string *getAllLotNo(std::string *);
        std::string *getAllPlateNo(std::string *);
        time_t *getAllDateTimeIn(time_t *);
        // Sort all data in the file
        void sortByPlateNo();
        void sortByLotNo();
        void sortByDateTimeIn();
        // Search data from the file
        std::string getLotByPlateNo(std::string);
        std::string getPlateByLotNo(std::string);
        ~AutoParkingSystem();
    protected:
        std::string formatString(std::string);
        void genLotNo();
        void calcDuration();
        void calcCharges();
        void swapStr(std::string *, std::string *);
        void swapTime(time_t *, time_t *);
        void sortBy(std::string);
        std::string searchBy(std::string, std::string);
    private:
        struct Transport {
            std::string lot_no;
            std::string plate_no;
            time_t date_time_in;
            std::string pin_no;
        };
        Transport *trans;
        std::string plate_no;
        std::string pin_no;
        std::string lot_no;
        std::string vehicle_type;
        time_t date_time_in;
        time_t date_time_out;
        double duration;
        double total_charges;
        u32 total_lines;
        bool new_plate_no;
        bool correct_pin;
};