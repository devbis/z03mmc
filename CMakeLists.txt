# We will always be building Release in order to enable all needed optimizations
# Debug symbols will be still present in the ELF file
SET(CMAKE_BUILD_TYPE Release)

# Prepare the toolchain for Telink 8251/8258 microcontroller
include(cmake/TelinkSDK.cmake)

# Main project settings
PROJECT(z03mmc C)

CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
ENABLE_LANGUAGE(ASM)

ADD_SUBDIRECTORY(src)
