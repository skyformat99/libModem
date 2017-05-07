//
// Created by root on 17-5-7.
//

#include "AT_Functions.hpp"

static inline int16_t CSQ2dBm(uint8_t CSQ) {
	double offset = (double)50.0/(double)31.0*(double)CSQ;

	return -125+(int16_t)offset;
}

void libModemInternal::ATFunctions::_HDRRSSI(std::string linedata, Reimu::libModem *ctx) {
	char *pRawData = (char *)linedata.c_str();

	char *buf = pRawData + 5;

	std::vector<int64_t> parsed = Reimu::StringOps::ExtractNumbers(buf);

	if (parsed.size()) {
		ctx->Signal_HDR.CSQ = (uint8_t)parsed[0];
		ctx->Signal_HDR.dBm = CSQ2dBm(ctx->Signal_HDR.CSQ);

		log.Log(50, "[libModem @ %s] Updated HDR Signal: CSQ=%u, dBm=%d", ctx->SerialPort.c_str(), ctx->Signal_HDR.CSQ, ctx->Signal_HDR.dBm);
	}
}





