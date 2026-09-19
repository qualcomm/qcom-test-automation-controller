#include <iostream>
#include <sstream>
#include <string>
#include "TACDev.h"

// Simulates exactly what taccl does:
//   1. InitializeTACDev()
//   2. OpenHandleByDescription("COM16")  -- no GetDeviceCount first
static void tryOpen(const char* port) {
    std::string p(port);
    TAC_HANDLE h = OpenHandleByDescription(p.data());
    char err[512]{};
    GetLastTACError(err, sizeof(err));
    if (h != kBadHandle) {
        char name[256]{};
        GetName(h, name, sizeof(name));
        bool state = false;
        TAC_RESULT r = GetBatteryState(h, &state);
        char err2[512]{};
        GetLastTACError(err2, sizeof(err2));
        std::cout << "OPENED \"" << port << "\" name=\"" << name << "\"" << std::endl;
        std::cout << "  GetBatteryState -> result=" << r << " state=" << state
                  << " lastErr=\"" << err2 << "\"" << std::endl;
        CloseTACHandle(h);
    } else {
        std::cout << "FAILED \"" << port << "\" lastErr=\"" << err << "\"" << std::endl;
    }
}

int main(int argc, char** argv) {
    bool withEnum = (argc > 1 && std::string(argv[1]) == "--enum");

    InitializeTACDev();

    if (withEnum) {
        // Mimic GetDeviceCount call first (like probe did before)
        int count = 0;
        GetDeviceCount(&count);
        std::cout << "Device count: " << count << std::endl;
    } else {
        std::cout << "Skipping GetDeviceCount (simulating taccl path)" << std::endl;
    }

    tryOpen("COM16");
    tryOpen("VTP1");
    return 0;
}
