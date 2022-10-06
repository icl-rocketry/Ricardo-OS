#pragma once

#include <vector> 
#include <algorithm>
#include <array>
#include <Eigen/Dense>
#include <Storage/logController.h>

/**
 * @brief Simple ringbuf implementation based on std::array
 * 
 * @tparam T 
 * @tparam LEN 
 */
template<typename T,size_t LEN>
class RingBuf : public std::array<T,LEN> {
    public:
        /**
         * @brief rotates the array, inserts a new value at the front and returns the removed value
         * 
         * @param val 
         * @return T 
         */
        T pop_push_back(T val){
            T prevVal = this->back();
            std::rotate(this->rbegin(),this->rbegin()+1,this->rend()); // shift elements to the right by 1
            this->at(0) = val;
            return prevVal;
        };
};

struct ApogeeInfo{
    bool reached;
    float altitude;
    uint32_t time; // time in ms
};


class ApogeeDetect
{
public:
/**
 * @brief Construct a new Apogee Detect object
 * 
 * @param sampleTime  in millis
 */
    ApogeeDetect(uint16_t sampleTime,LogController& logcontroller);
    /**
     * @brief 
     * 
     * @param altitude expects up + be careful!
     * @param velocity 
     * @param time 
     * @return const ApogeeInfo& 
     */
    const ApogeeInfo& checkApogee(float altitude, float velocity, uint32_t time);       //create function in the memory address of the structure to estimate the apogee

private:
    LogController& _logcontroller;
    // int len_time; // The length of the time
    static constexpr int arrayLen = 5;                  //polyval takes elements 3:5 to approximate the apogee, so 5 elements are required
    
    const uint16_t _sampleTime;
    uint32_t prevCheckApogeeTime{0};
    uint32_t initialEntryTime{0};


    RingBuf<uint32_t,arrayLen> time_array;           //create arrays to store recent flight history for apogee approximation
    RingBuf<float,arrayLen> altitude_array;


    /**
     * @brief Returns the coefficents of a 2nd order fitted polynomial
     * 
     * @param time_array 
     * @param altitude_array 
     * @return std::array<float,2> 
     */
    void quadraticFit(float oldTime, float newTime, float oldAlt, float newAlt);
    //sums for fitting polynomial
    float sigmaTime;
    float sigmaTime_2;
    float sigmaTime_3;
    float sigmaTime_4;
    float sigmaAlt;
    float sigmaAltTime;
    float sigmaAltTime_2;

    void updateSigmas(float oldTime, float newTime, float oldAlt, float newAlt);
    
    // Eigen::Matrix3d A;
    // Eigen::Vector3d b;
    // Eigen::Vector3d coeffs;

    Eigen::Matrix3f A;
    Eigen::Vector3f b;
    Eigen::Vector3f coeffs;


    uint32_t prevMachLockTime;
    bool mlock;         // Mach lockout activated by default on launch
    static constexpr float mlock_speed = 30; //value chosen thru tuning -> depends on filter performance
    static constexpr int mlock_time = 1000; // lockout time in milliseconds

    static constexpr float alt_threshold = 0; //threshold to detect altitude descent
    static constexpr float alt_min = 100;   // Minimum altitude (m) before apogee detection algorithm works
      
    ApogeeInfo _apogeeinfo;                             //create the structure for ApogeeInfo

};

