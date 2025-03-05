#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "utils.hpp"

// Function to get the current date and time as a string
std::string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char date[20];
    snprintf(date, sizeof(date), "%04d-%02d-%02d %02d:%02d:%02d",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
        ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return std::string(date);
}

std::string getCurrentDateTimeJustDash() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char date[20];
    snprintf(date, sizeof(date), "%04d-%02d-%02d_%02d-%02d-%02d",
        1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
        ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return std::string(date);
}

