//
// Created by root on 17-5-7.
//

#include "libModem.hpp"


int main(int argc, char **argv) {
	if (argv[1]) {
		Reimu::libModem mdm(Reimu::libModem::Huawei, argv[1]);
		mdm.Dispatch();
	}
}