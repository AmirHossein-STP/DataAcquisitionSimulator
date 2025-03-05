#pragma once
struct channelConfig {
    int status = 0; //1 means ok and 0 means faild
    int sensitivity;
    int sensor_type;
};
struct ACQCONFIG {
    int daq_serial_number;
    int acq_interval;
    int acq_duration;
    int sampling_freq;
    channelConfig channels[4];
    int parse_status = 2;
    int start_channel;
    int channel_count = 0;
};