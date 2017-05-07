//
// Created by root on 17-5-7.
//

#include "AT_Functions.hpp"

void libModemInternal::ATFunctions::_CLIP(std::string linedata, Reimu::libModem *ctx) {
	char *pRawData = (char *)linedata.c_str();

	char *buf = strchr(pRawData, ':');

	if (!buf)
		return;

	buf++;

	std::vector<std::string> parsed = Reimu::StringOps::Explode(buf, ",");

	if (parsed.size()) {
		ctx->CallingNumber = parsed[0];
		ctx->Calls.push_back((uint16_t)ctx->Calls.size());

		if (ctx->CallBacks.CallStatus) {
			ctx->CallBacks.CallStatus(11, parsed[0], NULL);
		}

		log.Log(50, "[libModem @ %s] Updated Call Number: %s", ctx->SerialPort.c_str(), ctx->CallingNumber.c_str());
	}
}
