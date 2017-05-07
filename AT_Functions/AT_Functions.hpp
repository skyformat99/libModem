//
// Created by root on 17-5-7.
//

#ifndef LIBMODEM_AT_FUNCTIONS_HPP
#define LIBMODEM_AT_FUNCTIONS_HPP

#include "../libModem.hpp"

namespace libModemInternal {
    class ATFunctions {
    private:
	static Kanna::Logging log;
    public:
	static void Init();
	static void Register(std::string cmdname, void (*func)(std::string, Reimu::libModem *));
	static void *Get(std::string cmdname);

	static void _CSQ(std::string linedata, Reimu::libModem *ctx);
	static void _CSQLVL(std::string linedata, Reimu::libModem *ctx);
	static void _HDRRSSI(std::string linedata, Reimu::libModem *ctx);
	static void _HDRCSQLVL(std::string linedata, Reimu::libModem *ctx);
	static void _CLIP(std::string linedata, Reimu::libModem *ctx);
	static void _CEND(std::string linedata, Reimu::libModem *ctx);
	static void _HCMGR(std::string linedata, Reimu::libModem *ctx);
    };
}

#endif //LIBMODEM_AT_FUNCTIONS_HPP
