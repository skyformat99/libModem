//
// Created by root on 17-5-7.
//

#include "AT_Functions.hpp"

Kanna::Logging libModemInternal::ATFunctions::log;

static std::unordered_map<std::string, void *> ATFunctionsList;
static std::shared_timed_mutex Lock_ATFunctionsList;

static bool Inited = 0;

void libModemInternal::ATFunctions::Init() {
	if (Inited)
		return;

	Lock_ATFunctionsList.lock();
	if (Inited) {
		Lock_ATFunctionsList.unlock();
		return;
	}
	Inited = 1;

	Register("+CSQ", &_CSQ);
	Register("^CRSSI", &_CSQ);
	Register("^CSQLVL", &_CSQLVL);
	Register("^RSSILVL", &_CSQLVL);
	Register("^HDRRSSI", &_HDRRSSI);
	Register("^HDRCSQLVL", &_HDRCSQLVL);
	Register("^HRSSILVL", &_HDRCSQLVL);
	Register("+CLIP", &_CLIP);
	Register("^CEND", &_CEND);
	Register("^HCMGR", &_HCMGR);


	Lock_ATFunctionsList.unlock();

}


void libModemInternal::ATFunctions::Register(std::string cmdname, void (*func)(std::string, Reimu::libModem *)) {
	log.Log(50, "[libModem::ATFunctions] Registered handler for AT command '%s' at %p", cmdname.c_str(), func);

	ATFunctionsList.insert(std::pair<std::string, void *>(cmdname, (void *)func));
}

void *libModemInternal::ATFunctions::Get(std::string cmdname) {
	auto tgt = ATFunctionsList.find(cmdname);

	if (tgt == ATFunctionsList.end()) {
		log.Log(40, "[libModem::ATFunctions] Warning: No handler found for AT command '%s'", cmdname.c_str());
		return NULL;
	} else
		return tgt->second;
}