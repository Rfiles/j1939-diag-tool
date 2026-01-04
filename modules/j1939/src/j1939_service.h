/**
 * J1939 Diagnostic Tool - J1939 Service Interface
 * 
 * This service acts as a central coordinator for the J1939 protocol stack.
 */

#ifndef J1939_SERVICE_H
#define J1939_SERVICE_H

#include <Arduino.h>
#include "j1939_handler.h"
#include "dm_handler.h"
#include "tp_handler.h"
#include "pdu_processor.h"

class J1939Service {
public:
    /**
     * @brief Gets the singleton instance of the J1939Service.
     */
    static J1939Service& getInstance() {
        static J1939Service instance;
        return instance;
    }

    /**
     * @brief Initializes the J1939 service and all related handlers.
     */
    void init();

    /**
     * @brief Starts the J1939 service.
     */
    void start();

    /**
     * @brief Stops the J1939 service.
     */
    void stop();

    // Add methods for sending/receiving messages, etc.

private:
    // Private constructor for singleton pattern
    J1939Service() {}

    // Delete copy constructor and assignment operator
    J1939Service(J1939Service const&) = delete;
    void operator=(J1939Service const&) = delete;
};

#endif // J1939_SERVICE_H
