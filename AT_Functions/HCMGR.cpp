//
// Created by root on 17-5-7.
//

#include "AT_Functions.hpp"

void libModemInternal::ATFunctions::_HCMGR(std::string linedata, Reimu::libModem *ctx) {
	char *pRawData = (char *)linedata.c_str();

	if (ctx->OverridedHandler == &_HCMGR) {
		std::vector<uint8_t> &buf = ctx->Buffers[0x1000];

		if (linedata.size() == 0) {
			buf.pop_back();
			buf.pop_back();

			log.Log(60, "[libModem @ %s] Converting SMS (%zu bytes) from UTF16BE to UTF8...",
				ctx->SerialPort.c_str(), buf.size());
			std::vector<uint8_t> converted = Reimu::StringOps::EncodingConvert(buf.data(),
											   buf.size(), "UTF16BE", "UTF8");
			converted.push_back(0);
			log.Log(60, "[libModem @ %s] SMS content: %s", ctx->SerialPort.c_str(), converted.data());
			buf.clear();
			ctx->OverridedHandler = NULL;
		} else {
			buf.insert(buf.end(), linedata.begin(), linedata.end());
		}

	} else {

		char *buf = pRawData + 7;

		std::vector<std::string> parsed = Reimu::StringOps::Explode(buf, ",");

		if (parsed.size() >= 7) {
			log.Log(50, "[libModem @ %s] SMS from %s at %s-%s-%s %s:%s:%s", ctx->SerialPort.c_str(),
				parsed[0].c_str(), parsed[1].c_str(), parsed[2].c_str(), parsed[3].c_str(),
				parsed[4].c_str(),
				parsed[5].c_str(), parsed[6].c_str());

			ctx->OverridedHandler = &_HCMGR;
		}
	}
}

