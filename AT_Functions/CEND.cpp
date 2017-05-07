//
// Created by root on 17-5-7.
//

#include "AT_Functions.hpp"

void libModemInternal::ATFunctions::_CEND(std::string linedata, Reimu::libModem *ctx) {
	ctx->CallingNumber = "";
	ctx->Calls.pop_back();

	log.Log(50, "[libModem @ %s] Call ended", ctx->SerialPort.c_str());

	if (ctx->CallBacks.CallStatus) {
		ctx->CallBacks.CallStatus(21, "", NULL);
	}
}