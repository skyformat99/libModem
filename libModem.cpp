#include "libModem.hpp"

#define RawData_BeginWith(str)		(strstr(pRawData, str) == pRawData)

Reimu::libModem::libModem(Reimu::libModem::ModemModes mode, std::string tty_device) {
	Mode = mode;
	SerialPort = tty_device;
}

static int16_t CSQ2dBm(uint8_t CSQ) {
	double offset = (double)50.0/(double)31.0*(double)CSQ;

	return -125+(int16_t)offset;
}

void Reimu::libModem::Dispatch() {
	SerialPortListener();
}


void Reimu::libModem::SerialPortListener() {
	int fd_tty;

	fd_tty = open(SerialPort.c_str(), O_RDWR|O_SYNC);

	log.Log(60, "[libModem @ %s] Serial port opened, fd=%d", SerialPort.c_str(), fd_tty);

	uint8_t buf_read[512];
	ssize_t bytes_read = 0;

	while (1) {
		bytes_read = read(fd_tty, buf_read, 512);

		log.Log(60, "[libModem @ %s] Read %zd bytes", SerialPort.c_str(), bytes_read);

		if (bytes_read > 0) {
			RawData.insert(RawData.end(), buf_read, buf_read+bytes_read);
			log.Log(60, "[libModem @ %s] %zd bytes in buffer", SerialPort.c_str(), RawData.size());
			if (memmem(RawData.data(), RawData.size(), "\r\n", 2)) {
				log.Log(60, "[libModem @ %s] <CR><LF> found, processing", SerialPort.c_str());

				std::vector<std::string> Lines = Reimu::StringOps::Explode((const char *)RawData.data(), RawData.size(), "\r\n", 2);
				log.Log(60, "[libModem @ %s] Buffer contains %zu lines", SerialPort.c_str(), Lines.size());

				long j = 1;
				for (auto &thisline : Lines) {
					if (j != Lines.size()) {
						log.Log(60, "[libModem @ %s] Processing line %ld", SerialPort.c_str(), j);
						ProcessData(thisline);
					} else {
						log.Log(60, "[libModem @ %s] Moving last line (%ld) to beginning of buffer", SerialPort.c_str(), j);
						RawData.resize(thisline.size());
						std::move(thisline.data(), thisline.data()+thisline.size(), RawData.data());
					}
					j++;
				}

			}
		} else {
			if (bytes_read == 0)
				throw Reimu::Exception(ECONNRESET);
			else
				throw Reimu::Exception(EIO);
		}
	}
}

void Reimu::libModem::ProcessData(std::string linedata) {
	char *pRawData = (char *)linedata.data();

	char *buf;

	log.Log(60, "[libModem @ %s] Line length: %zu, contents: %s", SerialPort.c_str(), linedata.size(), pRawData);

	if (RawData_BeginWith("+CSQ") || RawData_BeginWith("^CRSSI")) {
		buf = pRawData + 5;

		std::vector<int64_t> parsed = Reimu::StringOps::ExtractNumbers(buf);

		if (parsed.size()) {
			Signal.CSQ = (uint8_t)parsed[0];
			Signal.dBm = CSQ2dBm(Signal.CSQ);

			log.Log(50, "[libModem @ %s] Updated Signal: CSQ=%u, dBm=%d", SerialPort.c_str(), Signal.CSQ, Signal.dBm);
		}
	} else if (RawData_BeginWith("^HDRRSSI")) {
		buf = pRawData + 8;

		std::vector<int64_t> parsed = Reimu::StringOps::ExtractNumbers(buf);

		if (parsed.size()) {
			Signal_HDR.CSQ = (uint8_t)parsed[0];
			Signal_HDR.dBm = CSQ2dBm(Signal_HDR.CSQ);

			log.Log(50, "[libModem @ %s] Updated HDR Signal: CSQ=%u, dBm=%d", SerialPort.c_str(), Signal_HDR.CSQ, Signal_HDR.dBm);
		}
	} else if (RawData_BeginWith("^CSQLVL")) {
		buf = pRawData + 6;

		std::vector<int64_t> parsed = Reimu::StringOps::ExtractNumbers(buf);

		if (parsed.size()) {
			Signal.Percent = (uint8_t)parsed[0];

			log.Log(50, "[libModem @ %s] Updated Signal: Percent=%d", SerialPort.c_str(), Signal.Percent);
		}
	} else if (RawData_BeginWith("^HDRCSQLVL")) {
		buf = pRawData + 9;

		std::vector<int64_t> parsed = Reimu::StringOps::ExtractNumbers(buf);

		if (parsed.size()) {
			Signal_HDR.Percent = (uint8_t)parsed[0];

			log.Log(50, "[libModem @ %s] Updated HDR Signal: Percent=%d", SerialPort.c_str(), Signal_HDR.Percent);
		}
	}

	RawData.clear();

}

