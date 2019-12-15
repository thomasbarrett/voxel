#ifndef BROWSER_H
#define BROWSER_H

/**
 * \file browser.hpp
 * \brief Declarations for functions implemented in javascript.
 * \author Thomas Barrett <tbarrett@caltech.edu>s
 * \date Dec 12, 2019
 */

/**
 * Returns the player_id.
 */
extern "C" int get_pid();

/**
 * Send `size` bytes starting from `data` to the server.\
 * \param data: the address of the data to send
 * \param size: the number of bytes to send
 */
extern "C" void send(void *data, size_t size);

/**
 * Returns a random float in the range [0, 1).
 */
extern "C" float random();
extern "C" void update_health(int);
#endif /* BROWSER_H */