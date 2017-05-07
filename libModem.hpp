#ifndef Reimu_libModem_Hpp
#define Reimu_libModem_Hpp

#include <set>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <shared_mutex>

#include <cerrno>
#include <cstring>
#include <cinttypes>

#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <libReimu/Logging/Logging.hpp>
#include <libReimu/Exception/Exception.hpp>
#include <libReimu/StringOps/StringOps.hpp>

namespace Reimu {
    class libModem {
    private:
	Kanna::Logging log;
	std::vector<uint8_t> RawData;



	void SerialPortListener();
	void ProcessData(std::string linedata);
    public:
	enum ModemModes {
	    General = 0, Huawei = 1, MTK = 2
	};

	enum AccessTechnologies {
	    Unknown = 0,
	    GSM = 0x1, GPRS = 0x2, EDGE = 0x4,
	    CDMA = 0x10, EVDO_0 = 0x20, EVDO_A = 0x40,
	    WCDMA = 0x1000, HSDPA = 0x2000, HSUPA = 0x4000, HSPAP = 0x8000,
	    TDSCDMA = 0x10000,
	    LTE = 0x100000
	};

	struct CallBackCollection {
	    void (*SignalChange)(struct SignalValues *signal, struct SignalValues *signal_hdr, void *userp) = NULL;
	    void (*CallStatus)(int direction, std::string number, void *userp) = NULL;
	};

	struct SignalValues {
	    int16_t dBm = -32767;
	    uint8_t CSQ = 255;
	    uint8_t Percent = 255;
	};

	std::string SerialPort;

	ModemModes Mode = General;
	struct SignalValues Signal;
	struct SignalValues Signal_HDR;

	AccessTechnologies SupportedAccessTechnology = Unknown;
	AccessTechnologies CurrentAccessTechnology = Unknown;

	std::vector<uint16_t> Calls;
	std::string CallingNumber;

	std::unordered_map<int, std::vector<uint8_t>> Buffers;

	void (*OverridedHandler)(std::string linedata, Reimu::libModem *ctx) = NULL;
	CallBackCollection CallBacks;

	libModem(){};
	libModem(ModemModes mode, std::string tty_device);

	void Dispatch();

    };
}

#endif