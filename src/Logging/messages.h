/*
handles all messages bitwise so multiple messages can be contained in a single variable



*/

#include "Arduino.h"

#ifndef MESSAGES_H
#define MESSAGES_H


void new_message(uint32_t msg,String reason);
void delete_message(uint32_t msg);
bool flag_triggered(uint32_t msg);

#endif