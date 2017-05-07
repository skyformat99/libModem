#include "libModem.hpp"
#include "AT_Functions/AT_Functions.hpp"

#define RawData_BeginWith(str)		(strstr(pRawData, str) == pRawData)

Reimu::libModem::libModem(Reimu::libModem::ModemModes mode, std::string tty_device) {
	Mode = mode;
	SerialPort = tty_device;
	libModemInternal::ATFunctions::Init();
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

	void (*Handler)(std::string, Reimu::libModem *) = NULL;

	log.Log(60, "[libModem @ %s] Line length: %zu, contents: %s", SerialPort.c_str(), linedata.size(), linedata.c_str());

	if (OverridedHandler)
		OverridedHandler(linedata, this);
	else {
		char *buf = (char *)memmem(pRawData, linedata.size(), ":", 1);

		if (buf) {
			std::string cmdname(pRawData, buf);
			Handler = (void (*)(std::string, Reimu::libModem *))libModemInternal::ATFunctions::Get(cmdname);
			if (Handler)
				Handler(linedata, this);
		} else {
			if (linedata.size() == 4)
				if (0 == memcmp(pRawData, "RING", 4)) {
					log.Log(50, "[libModem @ %s] Ringing!!", SerialPort.c_str());
				}
		}
	}

	RawData.clear();
}

