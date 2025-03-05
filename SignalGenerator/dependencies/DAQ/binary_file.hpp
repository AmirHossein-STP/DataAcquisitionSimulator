#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "ACQConfig.hpp"
// Define the structure of the header
struct FileHeader {
    char signature[4];      // Signature, e.g., "PDAT"
    int version;            // Version of the file format
    int serial_num;         // DAQ serial number
    int smpl_freq;          // DAQ serial number
    int sensor_type;        // Sensor type, e.g., 1 for vibration and 0 for tacho
    int sensitivity;        // Sensor sensitivity
    int channel_num;        // Number of the channel
    char date[20];          // Creation date (YYYY-MM-DD HH:MM:SS)
    char reserved[52];      // Reserved space for future use
};
// Trailer to store information at the end of the file
struct FileTrailer {
    unsigned int recordCount;      // Number of records in the file
    //unsigned int checksum;         // Simple checksum for validation (sum of all data)
};

class BinaryFile {
public:
    BinaryFile(const std::string& localDataFolder, ACQCONFIG& config, int channel_num);
    ~BinaryFile();
    int insertData(std::vector<double>& Data);
    int close();
    FileHeader getHeader() const;
    FileTrailer getTrailer() const;
    std::string filename_wihout_extension;
    std::string file_name_location;
    std::string filename_org;
    std::string filename_temp;

protected:
    std::ofstream OutputFile;
    FileHeader header;
    FileTrailer trailer;
    unsigned int dataRecordCount;
};
//std::ofstream initBinaryFile(std::string file_location, int version, std::string dateTime);
//int saveDataBinary(const double* Data, size_t DataSize, std::ofstream &OutputFile);
//int closeBinaryFile(std::ofstream &OutputFile);