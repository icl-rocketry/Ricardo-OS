#ifndef STORAGECONTROLLER_H
#define STORAGECONTROLLER_H


#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include <string>
#include <vector>
#include <SdFat.h>

#include "freertos/semphr.h"



class stateMachine; //forward declaration to prevent ciruclar dependancy

enum class STORAGE_DEVICE:uint8_t{
    MICROSD,
    FLASH,
    SD1,
    SD2,
    NONE 
};

enum class FILE_TYPE:uint8_t{
    FILE,
    DIRECTORY
};

struct directory_element_t{
    std::string name;
    uint32_t size; //?
    FILE_TYPE type;
};

class StorageController{
    public:
        /**
         * @brief Construct a new Storage Controller object
         * 
         * @param sm statemachine pointer
         */
        StorageController(stateMachine *sm);
        void setup(); //setup storage devices

        
        /**
         * @brief open a file on specified path on specific device
         * 
         * @param path 
         * @param device 
         * @param mode 
         * @return File 
         */
        File open(std::string path,STORAGE_DEVICE device,oflag_t mode);
        /**
         * @brief Generate a unique directory name for a given directory on a given device.
         * 
         * @param input_directory 
         * @param device 
         * @return std::string 
         */
        std::string getUniqueDirectory(std::string input_directory,STORAGE_DEVICE device); //checks and updates directory name so that it is unique
        
        bool mkdir(std::string path, STORAGE_DEVICE device);
        /**
         * @brief List all files and directories in given path
         * 
         * @param path filepath
         * @param directory_structure output vector
         * @param device 
         * @return true 
         * @return false 
         */
        bool ls(std::string path,std::vector<directory_element_t> &directory_structure,STORAGE_DEVICE device);
        /**
         * @brief List all files and directories in root path
         * 
         * @param directory_structure 
         * @param device 
         * @return true 
         * @return false 
         */
        bool ls(std::vector<directory_element_t> &directory_structure,STORAGE_DEVICE device);
        void printDirectory(std::string path,STORAGE_DEVICE device);
        /**
         * @brief Erases only "/Logs" directory on specified device.
         * 
         * @param device 
         * @return true 
         * @return false 
         */
        bool erase(STORAGE_DEVICE device);//erase logging directory
        /**
         * @brief Format given device
         * 
         * @param device 
         * @return true 
         * @return false 
         */
        bool format(STORAGE_DEVICE device);
        /**
         * @brief Generate directory strucutre consisting of /Logs and /Configuration
         * 
         * @param device 
         */
        void generateDirectoryStructure(STORAGE_DEVICE device);

        enum class DEVICE_STATE:uint8_t{
            OK,
            ERR_WRITE,
            ERR_READ,
            ERR_OPEN
        };

        /**
         * @brief Report an error has occured while using a storage device
         * 
         * @param device 
         */
        void reportStatus(STORAGE_DEVICE device,DEVICE_STATE error);

        /**
         * @brief Get the lock for a device - DOESN'T ACQUIRE IT
         * 
         * @param device
         * 
         * @return The actual lock
        */
        SemaphoreHandle_t getDeviceLock(STORAGE_DEVICE device);


    private:
        stateMachine *_sm;//pointer to state machine

        SdFat microsd; // microsd card storage object
        //File file; // file object
        
        Adafruit_FlashTransport_SPI flashTransport;
        Adafruit_SPIFlash flash;// flash storage object

        FatFileSystem flash_fatfs;//flash filesystem

        // Have 1 lock per type of storage
        // Even None for now to make the code look nicer
        // No idea why this needs 2 variables
        StaticSemaphore_t lock_buffer[5]; // Static so we don't worry about freeing it
        SemaphoreHandle_t locks[5];
        
        int getFileNameIndex(const std::string fileName); // Extracts the index of the file from file name (e.g. returns 4 when input is logs_flight_4)
        bool ls(std::string path,std::vector<directory_element_t> &directory_structure,FatFileSystem* fs);
       
        bool rmParent(std::string path, FatFileSystem* fs); // remove all files under directory -> deletes path
};

// https://i.kym-cdn.com/photos/images/newsfeed/001/515/694/3b5.jpg
class DeviceMutex {
public:
    DeviceMutex(STORAGE_DEVICE device, StorageController& sc) {
        lock = sc.getDeviceLock(device);
        while (xSemaphoreTakeRecursive(lock, (TickType_t) 10) != pdTRUE);
    }
    ~DeviceMutex() {
        xSemaphoreGiveRecursive(lock);
    }

private:
    SemaphoreHandle_t lock;
};


#endif