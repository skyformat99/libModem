cmake_minimum_required(VERSION 2.8)
project(libModem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ReimuCXXFlags} -std=c++14")

set(SOURCE_FILES
    libModem.cpp
    libModem.hpp AT_Functions/AT_Functions.cpp AT_Functions/AT_Functions.hpp AT_Functions/CSQ.cpp AT_Functions/HDRRSSI.cpp AT_Functions/CSQLVL.cpp AT_Functions/HRSSILVL.cpp AT_Functions/CLIP.cpp AT_Functions/CEND.cpp AT_Functions/HCMGR.cpp)

add_library(Modem SHARED ${SOURCE_FILES})
add_library(Modem_static STATIC ${SOURCE_FILES})
set_target_properties(Modem_static PROPERTIES OUTPUT_NAME Modem)

add_executable(libModem_Tests Tests.cpp)
target_link_libraries(libModem_Tests Modem Reimu sqlite3 pthread)

install(FILES libModem.hpp DESTINATION include/libModem)
install(TARGETS Modem DESTINATION lib)
