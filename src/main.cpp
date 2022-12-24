#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#define ARDUINO_LOOP_STACK_SIZE 8192

#include <Arduino.h>

#include "stateMachine.h"
#include "States/setup.h"

#include "Storage/logController.h"
#include "Storage/storageController.h"
#include "Storage/configController.h"

#include <exception>

// #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
// #include "esp_log.h"

// #include <i2cpyro.h"

// stateMachine statemachine;
static constexpr bool exceptionsEnabled = true; //for debugging -> will integrate this into the sd configuration options later

TaskHandle_t loopTaskHandle = NULL;

void setup_task()
{
    try

    {
        Serial.begin(9600);
        Serial.println("Hello world");
        stateMachine statemachine;
        statemachine.storagecontroller.setup();
        statemachine.logcontroller.setup();

        ConfigController configcontroller(&statemachine.storagecontroller,&statemachine.logcontroller); 
        configcontroller.load(); 
        statemachine.initialise(new Setup(&statemachine));
    }
    catch (const std::exception &e)
    {
        Serial.println("Error");
        Serial.println(e.what());
        Serial.flush();
        throw e;
    }
}

void inner_loop_task() {
    Serial.println("loop");
    const TickType_t xDelay = 500;
    vTaskDelay(xDelay);
    // if constexpr (exceptionsEnabled)
    // {
    //     try
    //     {
    //         statemachine.update();
    //     }
    //     catch (const std::exception &e)
    //     {
    //         statemachine.logcontroller.log(e.what());
    //     }
    // }
    // else
    // {
    //     statemachine.update();
    // }
}

void loopTask(void *pvParameters)
{
    // esp_log_level_set("*", ESP_LOG_INFO); 
    // statemachine.initialise(new Setup(&statemachine)); //intialize statemachine with setup state to run all necessary setup tasks
    setup_task();
    for(;;) {
        inner_loop_task();
        vTaskDelay(1);
 
    }
}

extern "C" void app_main()
{
    initArduino(); //probably dont even need this
    xTaskCreateUniversal(loopTask, "loopTask", ARDUINO_LOOP_STACK_SIZE, NULL, 1, &loopTaskHandle, 1);
}
