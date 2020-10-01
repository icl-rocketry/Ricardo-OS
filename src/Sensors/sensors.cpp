#include "sensors.h"
#include "stateMachine.h"

//config
#include "ricardo_pins.h"
#include "config.h"

//indivudal sensor classes
#include "gps.h"
#include "imu.h"
#include "baro.h"
#include "battery.h"

Sensors::Sensors(stateMachine* sm) :
    gps(&(sm->I2C)),
    baro(&(sm->vspi)),
    imu(&(sm->vspi)),
    batt(BattVolt)
{
    _sm = sm;
}

void Sensors::setup_sensors(){
    //calls setup for each indiviual sensor
    gps.setup();
    baro.setup();
    imu.setup();
    batt.setup();

    
};

void Sensors::update(){
//updates the raw_measuremets data structure with new measurements
    gps.update();
    baro.update();
    imu.update();
    batt.update();

    sensors_raw.batt_percent = batt.batt_data.percent;
    sensors_raw.batt_volt = batt.batt_data.voltage;
    

};