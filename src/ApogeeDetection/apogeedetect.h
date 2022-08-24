#pragma once

#include <vector> //preamble needed for class
#include <array>
#include <Eigen/Dense>

struct ApogeeInfo{
    bool reached;
    float altitude;
    uint32_t time; // time in ms
};


class ApogeeDetect
{
public:
    ApogeeDetect();

    const ApogeeInfo& checkApogee(float altitude, float velocity, uint32_t time);       //create function in the memory address of the structure to estimate the apogee

private:
    // int len_time; // The length of the time
    static constexpr int arrayLen = 5;                  //polyval takes elements 3:5 to approximate the apogee, so 5 elements are required

    std::array<uint32_t,arrayLen> time_array;           //create arrays to store recent flight history for apogee approximation
    std::array<float,arrayLen> altitude_array;
    std::array<float,arrayLen> velocity_array;

    //std::array<float,3> quad;                            // Stores the polyfit results (3 coefficients)

    //function for 2nd degree polynomial approximation:
    std::array<float,2> Polyval(std::array<uint32_t,arrayLen> time_array, std::array<float,arrayLen> altitude_array);
    //linear system for polyval (Ax=b)
    Eigen::Matrix3f A;
    Eigen::Vector3f b;
    Eigen::Vector3f coeffs;


    uint32_t prevMachLockTime;
    bool mlock;         // Mach lockout activated by default on launch
    float mlock_speed; // 100ft/s in m/s

    float alt_min;   // Minimum altitude (m) before apogee detection algorithm works
      
    ApogeeInfo _apogeeinfo;                             //create the structure for ApogeeInfo

};