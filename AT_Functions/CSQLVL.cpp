//
// Created by root on 17-5-7.
//

#include "AT_Functions.hpp"

void libModemInternal::ATFunctions::_CSQLVL(std::string linedata, Reimu::libModem *ctx) {
	char *pRawData = (char *)linedata.c_str();

	char *buf = pRawData + 8;

	std::vector<int64_t> parsed = Reimu::StringOps::ExtractNumbers(buf);

	if (parsed.size()) {
		ctx->Signal.Percent = (uint8_t)parsed[0];
		log.Log(50, "[libModem @ %s] Updated Signal: Percent=%d", ctx->SerialPort.c_str(), ctx->Signal.Percent);
	}
}

