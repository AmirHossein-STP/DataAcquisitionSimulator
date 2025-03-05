
#include <vector>
#include <iostream>
#include <string>
#include "SignalGeneratorImgui.h"
#include "binary_file.hpp"

# define M_PI           3.14159265358979323846  /* pi */

class signal {
public:
    virtual ~signal() = default;  // Virtual destructor for polymorphism
    virtual double out(double x) const { return 0; }
};

class sin_signal : public signal {

public:

    sin_signal(float frequency, float phase, float amplitude)
        : signal() {  // Initialize base class first
        this->frequency = frequency;
        this->phase = phase;
        this->amplitude = amplitude;
    }
    float frequency;
    float phase;
    float amplitude;
    double out(double x) const override {
        double y;
        y = amplitude * sin(2 * M_PI * frequency * x + phase);
        return y;
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


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    bool isDarkMode = true; // Default: Dark Mode

    std::vector<std::unique_ptr<signal>> signals;


    int samplingFreq = 1000;
    float sampleDuration = 10;

    #include "imgui_init.h"

    int samples = 1000;
    std::vector<double> x, y;



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

        ImGui::BeginChild("settingPanel", ImVec2(0, 130), ImGuiChildFlags_Borders, window_flags);
        ImGui::PushItemWidth(200);
        ImGui::Text("Settings");
        ImGui::NewLine();
        ImGui::InputInt("Sampling Frequency (Hz)", &samplingFreq, 1, 200000);
        ImGui::SameLine();
        ImGui::InputFloat("Sample Duration (Second)", &sampleDuration, 0.1f, 10.0f, "%.3f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        //if (ImGui::Button("Close Application")) {
        //    PostQuitMessage(0); // Sends WM_QUIT message to exit the app
        //}
        ImGui::SameLine();
        if (ImGui::Button("save")) {
            std::string address = "../../Data";
            int channel_num = 1;
            ACQCONFIG config;
            config.daq_serial_number = 1;
            config.acq_interval = 100;
            config.acq_duration = 10;
            config.sampling_freq = 1000;
            config.channels[0].status = 1;
            config.channels[0].sensitivity = 1;
            config.channels[0].sensor_type = 1;
            config.parse_status = 0;
            config.start_channel = 0;
            config.channel_count = 1;
            BinaryFile binaryFile(address, config, channel_num);
            binaryFile.insertData(y);
            binaryFile.close();
        }
        if (ImGui::Button(isDarkMode ? "Switch to Light Mode" : "Switch to Dark Mode")) {
            isDarkMode = !isDarkMode; // Toggle the mode
            if (isDarkMode) {
                ImGui::StyleColorsDark();  // Apply Dark Mode
            }
            else {
                ImGui::StyleColorsLight(); // Apply Light Mode
            }
        }
        ImGui::EndChild();
        ImGui::NewLine();

        samples = int(samplingFreq * sampleDuration);
        x.resize(samples);
        y.resize(samples);
        for (int i = 0; i < samples; i++) {
            x[i] = i / double(samplingFreq);
        }

        if (ImGui::Button("Add Sine Wave")) {
            signals.push_back(std::make_unique<sin_signal>(1.0, 0.0, 1.0));
        }

        ImGui::BeginChild("sigPanelContainer", ImVec2(0, 320), ImGuiChildFlags_Borders, window_flags);
        for (size_t i = 0; i < signals.size(); i++) {
       
            sin_signal* sin_sig = dynamic_cast<sin_signal*>(signals[i].get());
            ImGui::PushID(i);  // Ensures uniqueness
            ImGui::BeginChild("sigPanel", ImVec2(0, 260), ImGuiChildFlags_Borders, window_flags);
            ImGui::BeginChild("sigSetting", ImVec2(300, 0), ImGuiChildFlags_None, window_flags);
            ImGui::PushItemWidth(200);
            ImGui::Text("Sine Wave");
            ImGui::NewLine();
            ImGui::InputFloat("Frequency", &sin_sig->frequency, 0.1f, 10.0f, "%.3f");
            //ImGui::Spacing();
            ImGui::InputFloat("Amplitude", &sin_sig->amplitude, 0.1f, 10.0f, "%.3f");
            ImGui::PopItemWidth();

            if (ImGuiKnobs::Knob("Phase", &sin_sig->phase, -2.0f, 2.0f, 0.1f, "%.1fpi", ImGuiKnobVariant_Tick)) {
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
    }


#include "imgui_ending.h"


    return 0;
}

