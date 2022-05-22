# We will always be building Release in order to enable all needed optimizations
# Debug symbols will be still present in the ELF file
SET(CMAKE_BUILD_TYPE Release)

# Prepare the toolchain for JN5169 microcontroller
#SET(JENNIC_CHIP JN5169)
include(cmake/TelinkSDK.cmake)

# Main project settings
PROJECT(zatc C)

CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
ENABLE_LANGUAGE(ASM)

#DUMP_COMPILER_SETTINGS()

ADD_SUBDIRECTORY(src)
