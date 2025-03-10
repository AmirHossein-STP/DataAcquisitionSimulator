
#include <vector>
#include <iostream>
#include <string>
#include <random>
#include "SignalGeneratorImgui.h"
#include "binary_file.hpp"
#include <chrono>
#include <thread>


# define M_PI           3.14159265358979323846  /* pi */

class signal {
public:
    virtual ~signal() = default;  // Virtual destructor for polymorphism
    virtual double out(double x) { return 0; }
};

class sin_signal : public signal {
public:
    sin_signal(float frequency, float phase, float amplitude)
        : signal() {  // Initialize base class first
        this->frequency = frequency;
        this->phase = phase;
        this->amplitude = amplitude;

        this->start = std::chrono::steady_clock::now();
    }
    double frequency;
    float phase;
    double amplitude;
    float increase_over_time_ratio = 0;
    std::chrono::time_point<std::chrono::steady_clock> start;
    double out(double x) override {
        double y;
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        y = (amplitude+ double(increase_over_time_ratio*float(elapsed_seconds.count()))/double(60.0)) * sin(2 * M_PI * frequency * x + phase);
        //y = double(increase_over_time_ratio * float(elapsed_seconds.count())) / double(60.0);
        return y;
    }
};

class pulse_train : public signal {
public:
    pulse_train(float frequency, float duty_cycle, float amplitude)
        : signal() {
        this->frequency = frequency;
        this->duty_cycle = duty_cycle;
        this->amplitude = amplitude;
    }

    double frequency;
    float duty_cycle;  // Percentage of the period where the pulse is high (0 to 1)
    double amplitude;

    double out(double x) override {
        double period = 1.0 / frequency;
        double time_in_period = fmod(x, period);
        return (time_in_period < duty_cycle* period) ? amplitude : 0.0;
    }
};

class white_signal : public signal {
private:
    std::mt19937 gen;  // Mersenne Twister RNG
    std::uniform_real_distribution<double> dist; // White noise distribution
    
public:
    double amplitude;

    // Constructor that accepts a seed for reproducibility
    white_signal(float amplitude, unsigned int seed = std::random_device{}())
        : gen(seed), dist(-1.0, 1.0), amplitude(amplitude) {}

    double out(double x) override {
        double y;
        y = amplitude*dist(gen);
        return y;

    }
    void reset() {
        std::mt19937 newgen(1);
        gen = newgen;
    }
};

void GenerateSignal(const std::vector<double>& x, std::vector<double>& y, const std::unique_ptr<signal>& sin_sig) {
    for (int i = 0; i < x.size(); i++) {
        y[i] = sin_sig->out(x[i]);
    }
}
void GenerateAddedSignal(const std::vector<double>& x, std::vector<double>& y, std::vector<std::unique_ptr<signal>>& signals) {
    for (int i = 0; i < x.size(); i++) {
        y[i] = 0;
        for (size_t j = 0; j < signals.size(); j++)
        {
            y[i] = y[i] + signals[j]->out(x[i]);
        }
    }
}
void save_signal(std::vector<double> y, int sampling_freq, int acq_duration, int acq_interval, int channel_num, int sensor_type, int daq_serial_number, std::string address) {
    //std::string address = "../../Data";
    ACQCONFIG config;
    config.daq_serial_number = daq_serial_number;
    config.acq_interval = acq_interval;
    config.acq_duration = acq_duration;
    config.sampling_freq = sampling_freq;

    config.parse_status  = 0;
    config.start_channel = channel_num;
    config.channel_count = 1;

    config.channels[channel_num].status = 1;
    config.channels[channel_num].sensitivity = 1;
    config.channels[channel_num].sensor_type = sensor_type;
    BinaryFile binaryFile(address, config, channel_num);
    binaryFile.insertData(y);
    binaryFile.close();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    bool isDarkMode = false; // Default: Dark Mode
    bool is_periodicaly = true;

    int samplingFreq = 1000;
    float sampleDuration = 10;
    int sensor_type = 1;
    int sampling_interval = 10;
    int channel_num = 0;
    int daq_serial_num = 1;

    std::string data_folder_address;

    int samples;
    std::vector<double> x, y;
    std::vector<std::unique_ptr<signal>> signals;

    #include "imgui_init.h"

    auto start = std::chrono::steady_clock::now();

    // Main loop
    bool done = false;
    while (!done)
    {
        //if (GetAsyncKeyState(VK_ESCAPE)) {
        //    PostQuitMessage(0);
        //}

        #include "imgui_while_init.h"

        // Get window size using Win32 API
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        // Set ImGui window properties
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::Begin("Signal Generator", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

        ImGui::BeginChild("settingPanel", ImVec2(0, 150), ImGuiChildFlags_Borders, window_flags);
        
        ImGui::Text("Settings");
        //ImGui::NewLine();

        //ImVec2 avail = ImGui::GetContentRegionAvail();
        int columns = 3, rows = 2;
        //float spacing = ImGui::GetStyle().ItemSpacing.x;
        //float itemWidth = (avail.x - (columns - 1) * spacing) / columns;

        ImGui::BeginTable("GridTable", columns,  ImGuiTableFlags_SizingStretchSame);
        ImGui::TableNextRow(); // Move to the next row
        ImGui::TableSetColumnIndex(0); // Move to the correct column
        //ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputInt("Sampling Frequency (Hz)", &samplingFreq, 1, 200000);
        //ImGui::Spacing();
        //ImGui::SameLine();
        //ImGui::Spacing();
        //ImGui::SameLine();
        //ImGui::SameLine();
        //ImGui::Spacing();
        //ImGui::SameLine();
        //ImGui::SameLine();
        //ImGui::Spacing();
        //ImGui::SameLine();
        ImGui::TableSetColumnIndex(1);
        ImGui::InputFloat("Sample Duration (Second)", &sampleDuration, 0.1f, 10.0f, "%.3f");
        //ImGui::SameLine();
        ImGui::TableSetColumnIndex(2);
        ImGui::InputInt("Interval (Second)", &sampling_interval, 1, 10);
        //ImGui::PopItemWidth();
        //ImGui::PushItemWidth(100);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::InputInt("Sensor Type", &sensor_type, 1, 10);
        //ImGui::SameLine();
        
        ImGui::TableSetColumnIndex(1);
        ImGui::InputInt("Channel Number", &channel_num, 1, 10);
        //ImGui::SameLine();
        
        ImGui::TableSetColumnIndex(2);
        ImGui::InputInt("DAQ Serial Number", &daq_serial_num, 1, 10);
        //ImGui::SameLine();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        static char data_folder_address_char[128] = "";
        ImGui::InputTextWithHint("Data Address", "enter Data folder address here", data_folder_address_char, IM_ARRAYSIZE(data_folder_address_char));
        data_folder_address = data_folder_address_char;
        //ImGui::PopItemWidth();

        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button("Reset Transition")) {
            for (size_t i = 0; i < signals.size(); i++) {
                if (dynamic_cast<const sin_signal*>(signals[i].get())) {
                    sin_signal* sin_sig = dynamic_cast<sin_signal*>(signals[i].get());
                    sin_sig->start = std::chrono::steady_clock::now();
                }
            }
        }
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        //ImGui::SameLine();
        //if (ImGui::Button("Close Application")) {
        //    PostQuitMessage(0); // Sends WM_QUIT message to exit the app
        //}
        //ImGui::SameLine();
        //ImGui::TableSetColumnIndex(7);
        if (ImGui::Button("Save Immediately")) {
            save_signal(y, samplingFreq, sampleDuration, sampling_interval, channel_num, sensor_type, daq_serial_num, data_folder_address);
        }
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Button(is_periodicaly ? "Stop Saving Periodically" : "Start Saving Periodically")) {
            is_periodicaly = !is_periodicaly; // Toggle the mode
        }
        ImGui::TableSetColumnIndex(2);
        if (ImGui::Button(isDarkMode ? "Switch to Light Mode" : "Switch to Dark Mode")) {
            isDarkMode = !isDarkMode; // Toggle the mode
            if (isDarkMode) {
                ImGui::StyleColorsDark();  // Apply Dark Mode
            }
            else {
                ImGui::StyleColorsLight(); // Apply Light Mode
            }
        }
        ImGui::EndTable();


        ImGui::EndChild();
        ImGui::NewLine();

        samples = int(samplingFreq * sampleDuration);
        x.resize(samples);
        y.resize(samples);
        for (int i = 0; i < samples; i++) {
            x[i] = i / double(samplingFreq);
        }

        if (ImGui::Button("+ Add Sine Wave")) {
            signals.push_back(std::make_unique<sin_signal>(1.0, 0.0, 0.5));
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Add White noise")) {
            signals.push_back(std::make_unique<white_signal>(0.5));
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Add Tachometer wave")) {
            signals.push_back(std::make_unique<pulse_train>(1.0, 0.05, 0.5));
        }
        ImGui::BeginChild("sigPanelContainer", ImVec2(0, 320), ImGuiChildFlags_Borders, window_flags);
        for (size_t i = 0; i < signals.size(); i++) {
            ImGui::PushID(i);  // Ensures uniqueness
            ImGui::BeginChild("sigPanel", ImVec2(0, 260), ImGuiChildFlags_Borders, window_flags);

            
            /* sdadasdsdasdsadsadasdsadasdsa*/
            if (dynamic_cast<const sin_signal*>(signals[i].get())) {
                sin_signal* sin_sig = dynamic_cast<sin_signal*>(signals[i].get());

                ImGui::BeginChild("sigSetting", ImVec2(300, 0), ImGuiChildFlags_None, window_flags);
                ImGui::PushItemWidth(200);
                ImGui::Text("Sine Wave");
                ImGui::NewLine();
                ImGui::InputDouble("Frequency", &sin_sig->frequency, 0.1f, 10.0f, "%.3f");
                //ImGui::Spacing();
                ImGui::InputDouble("Amplitude", &sin_sig->amplitude, 0.1f, 10.0f, "%.3f");
                ImGui::PopItemWidth();

                if (ImGuiKnobs::Knob("Phase", &sin_sig->phase, -2.0f, 2.0f, 0.1f, "%.1fpi", ImGuiKnobVariant_Tick)) {
                    // value was changed
                }
                ImGui::SameLine();
                if (ImGuiKnobs::Knob("Increase", &sin_sig->increase_over_time_ratio, -10.0f, 10.0f, 0.2f, "%.1f", ImGuiKnobVariant_Tick)) {
                    // value was changed
                }
                ImGui::SameLine();

                ImGui::BeginChild("DeleteParentParentContainer", ImVec2(0, 0), ImGuiChildFlags_None, window_flags);
                ImGui::BeginChild("DeleteParentContainer", ImVec2(10, 100), ImGuiChildFlags_None, window_flags);
                ImGui::EndChild();
                ImGui::SameLine();
                ImGui::BeginChild("DeleteContainer", ImVec2(120, 100), ImGuiChildFlags_None, window_flags);
                ImGui::NewLine();
                if (ImGui::Button("Delete")) {

                    signals.erase(signals.begin() + i);
                    ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::PopID();
                    break; // Stop loop to prevent out-of-bounds errors
                }
                ImGui::EndChild();
                ImGui::EndChild();

                ImGui::EndChild();

                ImGui::SameLine();

                GenerateSignal(x, y, signals[i]);
                ImGui::SameLine();
                float width = ImGui::GetContentRegionAvail().x;  // Available width
                float height = ImGui::GetContentRegionAvail().y; // Available height
                if (ImPlot::BeginPlot("Sine Waves", ImVec2(width, 240))) {
                    ImPlot::PlotLine(("Signal " + std::to_string(i)).c_str(), x.data(), y.data(), x.size());
                    ImPlot::EndPlot();
                }
            }
            else if (dynamic_cast<const white_signal*>(signals[i].get())) {
                white_signal* white_sig = dynamic_cast<white_signal*>(signals[i].get());

                ImGui::BeginChild("sigSetting", ImVec2(300, 0), ImGuiChildFlags_None, window_flags);
                ImGui::PushItemWidth(200);
                ImGui::Text("White Noise");
                ImGui::NewLine();
                //ImGui::InputFloat("Frequency", &white_sig->frequency, 0.1f, 10.0f, "%.3f");
                ////ImGui::Spacing();
                ImGui::InputDouble("Amplitude", &white_sig->amplitude, 0.05f, 10.0f, "%.3f");
                ImGui::PopItemWidth();

                if (ImGui::Button("Delete")) {

                    signals.erase(signals.begin() + i);
                    //ImGui::EndChild();
                    //ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::PopID();
                    break; // Stop loop to prevent out-of-bounds errors
                }

                //if (ImGuiKnobs::Knob("Phase", &white_sig->phase, -2.0f, 2.0f, 0.1f, "%.1fpi", ImGuiKnobVariant_Tick)) {
                //    // value was changed
                //}
                //ImGui::SameLine();

                //ImGui::BeginChild("DeleteParentParentContainer", ImVec2(0, 0), ImGuiChildFlags_None, window_flags);
                //ImGui::BeginChild("DeleteParentContainer", ImVec2(120, 100), ImGuiChildFlags_None, window_flags);
                //ImGui::EndChild();
                //ImGui::SameLine();
                //ImGui::BeginChild("DeleteContainer", ImVec2(120, 100), ImGuiChildFlags_None, window_flags);
                //ImGui::NewLine();

                //ImGui::EndChild();
                //ImGui::EndChild();


                ImGui::EndChild();

                ImGui::SameLine();

                //white_sig->reset();

                GenerateSignal(x, y, signals[i]);
                ImGui::SameLine();
                float width = ImGui::GetContentRegionAvail().x;  // Available width
                float height = ImGui::GetContentRegionAvail().y; // Available height
                if (ImPlot::BeginPlot("White Noise", ImVec2(width, 240))) {
                    ImPlot::PlotLine(("Signal " + std::to_string(i)).c_str(), x.data(), y.data(), x.size());
                    ImPlot::EndPlot();
                }
            }
            else if (dynamic_cast<const pulse_train*>(signals[i].get())) {
                pulse_train* pulse_train_sig = dynamic_cast<pulse_train*>(signals[i].get());

                ImGui::BeginChild("sigSetting", ImVec2(300, 0), ImGuiChildFlags_None, window_flags);
                ImGui::PushItemWidth(200);
                ImGui::Text("Sine Wave");
                ImGui::NewLine();
                ImGui::InputDouble("Frequency", &pulse_train_sig->frequency, 0.1f, 10.0f, "%.3f");
                //ImGui::Spacing();
                ImGui::InputDouble("Amplitude", &pulse_train_sig->amplitude, 0.1f, 10.0f, "%.3f");
                ImGui::PopItemWidth();

                if (ImGuiKnobs::Knob("Duty Cycle", &pulse_train_sig->duty_cycle, -2.0f, 2.0f, 0.1f, "%.1fpi", ImGuiKnobVariant_Tick)) {
                    // value was changed
                }
                ImGui::SameLine();

                ImGui::BeginChild("DeleteParentParentContainer", ImVec2(0, 0), ImGuiChildFlags_None, window_flags);
                ImGui::BeginChild("DeleteParentContainer", ImVec2(120, 100), ImGuiChildFlags_None, window_flags);
                ImGui::EndChild();
                ImGui::SameLine();
                ImGui::BeginChild("DeleteContainer", ImVec2(120, 100), ImGuiChildFlags_None, window_flags);
                ImGui::NewLine();
                if (ImGui::Button("Delete")) {

                    signals.erase(signals.begin() + i);
                    ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::EndChild();
                    ImGui::PopID();
                    break; // Stop loop to prevent out-of-bounds errors
                }
                ImGui::EndChild();
                ImGui::EndChild();

                ImGui::EndChild();

                ImGui::SameLine();
                GenerateSignal(x, y, signals[i]);
                ImGui::SameLine();
                float width = ImGui::GetContentRegionAvail().x;  // Available width
                float height = ImGui::GetContentRegionAvail().y; // Available height
                if (ImPlot::BeginPlot("Sine Waves", ImVec2(width, 240))) {
                    ImPlot::PlotLine(("Signal " + std::to_string(i)).c_str(), x.data(), y.data(), x.size());
                    ImPlot::EndPlot();
                }
            }
            /*edfsfdsfsdf*/


            ImGui::EndChild();
            ImGui::PopID();

        }
        ImGui::EndChild();

        ImGui::PopStyleVar();

        GenerateAddedSignal(x, y, signals);

        if (ImPlot::BeginPlot("Sine Waves")) {
            ImPlot::PlotLine("Sum of Signals", x.data(), y.data(), x.size());
            ImPlot::EndPlot();
        }
        ImGui::End();

        #include "imgui_while_ending.h"

        if ((is_periodicaly == 1)&&(std::chrono::steady_clock::now() - start) > std::chrono::milliseconds(sampling_interval * 1000))
        {
            start = std::chrono::steady_clock::now();
            save_signal(y, samplingFreq, sampleDuration, sampling_interval, channel_num, sensor_type, daq_serial_num, data_folder_address);
        }
    }


#include "imgui_ending.h"


    return 0;
}

