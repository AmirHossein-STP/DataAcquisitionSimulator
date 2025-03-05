#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
//#include <sstream>
//#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "binary_file.hpp"
#include "utils.hpp"
#include "ACQConfig.hpp"

const int version = 3;
const std::string extention_org = ".bin";
const std::string extention_temp = ".temp";

BinaryFile::BinaryFile(const std::string& localDataFolder, ACQCONFIG& config, int channel_num) {
    filename_wihout_extension = getCurrentDateTimeJustDash() + "_" + std::to_string(channel_num);
    filename_org = filename_wihout_extension + extention_org;
    filename_temp = filename_wihout_extension + extention_temp;
    file_name_location = localDataFolder + ("/" + filename_org);

    OutputFile.open(file_name_location, std::ios::binary | std::ios::trunc | std::ios::out);
    if (!OutputFile.is_open()) {
        std::cout << "Error initializing binary file! file cannot be opened!" << std::endl;
    }
    dataRecordCount = 0;

    // Set the file signature
    strncpy(header.signature, "PDAT", 4);
    // Set the version number
    header.version = version;
    header.serial_num = config.daq_serial_number;
    header.smpl_freq = config.sampling_freq;
    header.sensor_type = config.channels[channel_num].sensor_type;
    header.sensitivity = config.channels[channel_num].sensitivity;
    header.channel_num = channel_num;
    // Set the current date and time
    strncpy(header.date, getCurrentDateTime().c_str(), 20);
    // Reserve future space with zeros
    memset(header.reserved, 0, sizeof(header.reserved));
    // Write the header to the binary file
    OutputFile.write(reinterpret_cast<const char*>(&header), sizeof(header));

    if (OutputFile.fail()) {
        std::cout << "Error writing header to the file: " << file_name_location << std::endl;
    }
    else {
        std::cout << "Open binary file: " << file_name_location << " successfully" << std::endl;
    }
}
int BinaryFile::insertData(std::vector<double>& Data){
    if (!OutputFile.is_open()) {
        std::cout << "Error insert to binary file! file cannot be opened!" << std::endl;
        return 1;
    }
    OutputFile.write(reinterpret_cast<const char*>(Data.data()), Data.size() * sizeof(double));

    if (OutputFile.fail()) {
        std::cout << "Error writing to the file. " << std::endl;
        return 1;
    }
    else {
        dataRecordCount += Data.size();
    }
    return 0;
}
int BinaryFile::close() {
    if (!OutputFile.is_open()) {
        std::cout << "Error close the binary file! file cannot be opened!" << std::endl;
        return 1;
    }
    FileTrailer trailer;
    trailer.recordCount = dataRecordCount;
    OutputFile.write(reinterpret_cast<const char*>(&trailer), sizeof(trailer));

    OutputFile.close();

    if (OutputFile.fail()) {
        std::cout << "Error binary file saving trailer." << std::endl;
        return 1;
    }
    else {
        std::cout << "saved " << dataRecordCount << " data records." << std::endl;
    }
    return 0;
}

BinaryFile::~BinaryFile() {
    if (OutputFile.is_open()) {
        close();
    }
}

FileHeader BinaryFile::getHeader() const {
    return header;
}
FileTrailer BinaryFile::getTrailer() const {
    return trailer;
}



//std::ofstream initBinaryFile(std::string file_location, int version, std::string dateTime) {
//    std::ofstream OutputFile(file_location, std::ios::binary | std::ios::trunc);
//    if (!OutputFile.is_open()) {
//        std::cout << "Error initializing binary file! file cannot be opened!" << std::endl;
//        return OutputFile;
//    }
//    
//
//    FileHeader header;
//    // Set the file signature
//    //strncpy(header.signature, "PDAT", 4);
//    // Set the version number
//    header.version = version;
//    header.serial_num = 0;
//    header.smpl_freq = 10000;
//    header.sensor_type = 1;
//    header.channel_num = 1;
//    // Set the current date and time
//    //strncpy(header.date, dateTime.c_str(), 20);
//    // Reserve future space with zeros
//    memset(header.reserved, 0, sizeof(header.reserved));
//    // Write the header to the binary file
//    OutputFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
//
//    if (OutputFile.fail()) {
//        std::cout << "Error writing header to the file: " << file_location << std::endl;
//        return OutputFile;
//    }
//    return OutputFile;
//}
//int saveDataBinary(const double* Data, size_t DataSize, std::ofstream &OutputFile) {
//    if (!OutputFile.is_open()) {
//        std::cout << "Error save to binary file! file cannot be opened!" << std::endl;
//        return 1;
//    }
//    OutputFile.write(reinterpret_cast<const char*>(Data), DataSize * sizeof(double));
//
//    if (OutputFile.fail()) {
//        std::cout << "Error writing to the file. " << std::endl;
//        return 1;
//    }
//    return 0;
//}
//int closeBinaryFile(std::ofstream &OutputFile) {
//    if (!OutputFile.is_open()) {
//        std::cout << "Error save to binary file! file cannot be opened!" << std::endl;
//        return 1;
//    }
//    FileTrailer trailer;
//    trailer.recordCount = dataRecordCount;
//    OutputFile.write(reinterpret_cast<const char*>(&trailer), sizeof(trailer));
//
//    OutputFile.close();
//
//    if (OutputFile.fail()) {
//        std::cout << "Error binary file saving trailer." << std::endl;
//        return 1;
//    }
//    else {
//        std::cout << "saved " << dataRecordCount << " data records." << std::endl;
//    }
//    return 0;
//}