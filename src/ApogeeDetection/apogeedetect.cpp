

#include "apogeedetect.h"
#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Arduino.h>
// #include "millis.h"

ApogeeDetect::ApogeeDetect():
mlock(true),        //intialise the mach lockout stuff
mlock_speed(30.48),
alt_min(100),
_apogeeinfo({false,0,0})
{
 

    //Apogee detect stuff
    // apogee_reached = false;
    // apogee_index = -1;
    // apo_predict = 0;
    // apo_index = {3,4,5};                 //WHAT IS THIS-----------------------
    // alt_initial = millis();                 //take the initial altitude to calculate the change in alt later
    //Reading flight data

    // len_time = sizeof(time)/time[1];    //length of the time vector
}

//Function to update flight data values and return data for apogee prediction
const ApogeeInfo& ApogeeDetect::checkApogee(float altitude,float velocity,uint32_t time)
{
    // if (!(mlock_index = -1))
    // {
    //[0 1 2 3 4]
    //[-1 0 1 2 3]

    //Initialising and creating the ring buffers using memory copy (maintains newest updated values in the arrays, deletes oldest value)
    std::memcpy(time_array.data()+1,time_array.data(),(arrayLen-1)*sizeof(uint32_t));       //new value assigned is at 0 index, oldest value is at 4th index
    time_array.at(0) = time;

    std::memcpy(altitude_array.data()+1,altitude_array.data(),(arrayLen-1)*sizeof(float));
    altitude_array.at(0) = altitude;

    std::memcpy(velocity_array.data()+1,velocity_array.data(),(arrayLen-1)*sizeof(float));
    velocity_array.at(0) = velocity;
    
    //Mach lock check:
    if (abs(velocity) >= mlock_speed)
    {
        mlock = true;
        prevMachLockTime = millis();
        // log time
    }
    else if ((millis() - prevMachLockTime) > 1000)      //if more than a second has passed and vel is less than mlock_speed
    {
        mlock = false;
        // log the time
    }


    // Apogee detection:

    if (!(_apogeeinfo.reached) && (altitude > alt_min))
    {
        Polyval(time_array, altitude_array); // POLYFIT -> x(t), time in ms
        _apogeeinfo.time = -coeffs(1) / (2 * coeffs(2));  // maximum from polyinomial using derivative
        if ((millis() >= _apogeeinfo.time) && (coeffs(2) < 0) && (millis() > 0))
        {
            _apogeeinfo.altitude = coeffs(2)*(_apogeeinfo.time*_apogeeinfo.time) + (coeffs(1)*_apogeeinfo.time) + coeffs(0); //evalute 2nd order polynomial
            if ((altitude - _apogeeinfo.altitude) < 0){
                _apogeeinfo.reached = true;
                //log apogee time and altitude
                return _apogeeinfo;
            }
    
        }
    }

    _apogeeinfo.reached = false;
    return _apogeeinfo;

};

/*Create a matrix, three simulatneos equations */
std::array<float,2> ApogeeDetect::Polyval(std::array<uint32_t,arrayLen> time_array, std::array<float,arrayLen> altitude_array)
{
    //std::array<float,3> arrayConstants = {0,0,0};
    //int matrixConstants[3][3];
    //simultaneos equation instantiation, in form Ax=b
    for(int i = 0;i<arrayLen;i++)
    {
        //create the b array by summing the results
        //arrayConstants = {arrayConstants.at(0) + altitude_array.at(i), arrayConstants.at(1) + altitude_array.at(i)*time_array.at(i), arrayConstants.at(2) + altitude_array.at(i)*(time_array.at(i)^2)};
        //create the matrix of coefficients(A):
        //matrixConstants = {{3,matrixConstants[0][1]+time_array.at(i),matrixConstants[0][2]+(time_array.at(i)^2)},{matrixConstants[1][0]+time_array.at(i),matrixConstants[1][1]+(time_array.at(i)^2),matrixConstants[1][2]+(time_array.at(i)^3)},{matrixConstants[2][0]+(time_array.at(i)^2),matrixConstants[2][1]+(time_array.at(i)^3),matrixConstants[2][2]+(time_array.at(i)^4)}};
        A << {
            {3,x(0,1)+time_array.at(i),x(0,2)+(time_array.at(i)^2)},
            {x(1,0)+time_array.at(i),x(1,1)+(time_array.at(i)^2),x(1,2)+(time_array.at(i)^3)},
            {x(2,0)+(time_array.at(i)^2),x(2,1)+(time_array.at(i)^3),x(2,2)+(time_array.at(i)^4)}
        };
    
        b << {{arrayConstants.at(0) + altitude_array.at(i), arrayConstants.at(1) + altitude_array.at(i)*time_array.at(i), arrayConstants.at(2) + altitude_array.at(i)*(time_array.at(i)^2)}};
    }
    //solve the system:
    coeffs = A.colPivHouseholderQr().solve(b);
}
