

#include "apogeedetect.h"
#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Arduino.h>
// #include "millis.h"

ApogeeDetect::ApogeeDetect(uint16_t sampleTime) : _sampleTime(sampleTime),
                                                  time_array{0},
                                                  altitude_array{0},
                                                  mlock(true), // intialise the mach lockout stuff
                                                  _apogeeinfo({false, 0, 0})
{}

// Function to update flight data values and return data for apogee prediction
const ApogeeInfo &ApogeeDetect::checkApogee(float altitude, float velocity, uint32_t time)
{
    if (millis() - prevCheckApogeeTime > _sampleTime)
    {
        uint32_t prevTime = time_array.pop_push_back(time);
        uint32_t prevAltitude = altitude_array.pop_push_back(altitude);

        // Mach lock check:
        if (abs(velocity) >= mlock_speed)
        {
            mlock = true;
            prevMachLockTime = millis();

            // log time
        }
        else if ((millis() - prevMachLockTime) > mlock_time) // if more than a second has passed and vel is less than mlock_speed
        {
            mlock = false;
            // log the time
        }

        // Apogee detection:

        if (!(_apogeeinfo.reached) && (altitude > alt_min) && !mlock)
        {
            // coeffs = poly2fit(time_array, altitude_array); // POLYFIT -> x(t), time in ms
            quadraticFit(prevTime,time,prevAltitude,altitude);
            
            _apogeeinfo.time = -coeffs(1) / (2 * coeffs(2)); // maximum from polyinomial using derivative

            if ((millis() >= _apogeeinfo.time) && (coeffs(2) < 0) && (millis() > 0))
            {
                _apogeeinfo.altitude = coeffs(0) - (std::pow(coeffs(1),2)/(4*coeffs(2)));
                
                // coeffs(2) * std::pow(_apogeeinfo.time,2) + (coeffs(1) * _apogeeinfo.time) + coeffs(0); // evalute 2nd order polynomial
                if ((altitude - _apogeeinfo.altitude) < alt_threshold) // if we have passed apogee and now decending, could put a bound on here too
                {
                    _apogeeinfo.reached = true;
                    // log apogee time and altitude
                }
            }
        }
    }
    prevCheckApogeeTime = millis();
    return _apogeeinfo;
};

void ApogeeDetect::updateSigmas(uint32_t oldTime, uint32_t newTime, float oldAlt, float newAlt){
    sigmaTime += (newTime-oldTime);
    sigmaTime_2 += (std::pow(newTime,2) - std::pow(oldTime,2));
    sigmaTime_3 += (std::pow(newTime,3) - std::pow(oldTime,3));
    sigmaTime_4 += (std::pow(newTime,4) - std::pow(oldTime,4));
    sigmaAlt += (newAlt-oldAlt);
    sigmaAltTime += ((newAlt*float(newTime)) - (oldAlt*float(oldAlt)));
    sigmaAltTime_2 += ((newAlt*float(std::pow(newTime,2))) - (oldAlt*float(std::pow(oldAlt,2)))); 
};

/*Create a matrix, three simulatneos equations */
void ApogeeDetect::quadraticFit(uint32_t oldTime, uint32_t newTime, float oldAlt, float newAlt)
{
    updateSigmas(oldTime, newTime, oldAlt, newAlt); // update sigmas with new values and remove old values
    //re populate the arrays
    A << arrayLen, sigmaTime, sigmaTime_2,
        sigmaTime, sigmaTime_2, sigmaTime_3,
        sigmaTime_2, sigmaTime_3, sigmaTime_4;

    b << sigmaAlt, sigmaAltTime, sigmaAltTime_2;
    //solve the system for the coefficents -> if this is too slow, look into using LLT cholesky decomp
    coeffs = A.householderQr().solve(b); 

}
