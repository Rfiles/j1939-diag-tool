/**
 * J1939 Diagnostic Tool - Filesystem Handler Interface
 * 
 * Versão: 2.1.0
 */

#ifndef FILESYSTEM_HANDLER_H
#define FILESYSTEM_HANDLER_H

// Initializes LittleFS and loads the configuration from config.json
// Returns true on success, false on failure.
bool filesystem_init();

#endif // FILESYSTEM_HANDLER_H