# Check mandatory parameters
if(NOT SDK_PREFIX)
    message(FATAL_ERROR "No SDK_PREFIX specified (it must point to the root of JN-SW-4170 SDK)")
endif()

if(NOT TOOLCHAIN_PREFIX)
    message(FATAL_ERROR "No TOOLCHAIN_PREFIX specified (it must point to the root of desired compiler bundle)")
endif()

SET(CMAKE_SYSTEM_NAME Generic)
SET(TARGET_PREFIX "tc32-elf")

SET(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
SET(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/include)
SET(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/lib)

IF(NOT CMAKE_C_COMPILER)
        SET(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TARGET_PREFIX}-gcc${TOOL_EXECUTABLE_SUFFIX})
ENDIF()

set(TELINK_PLATFORM "8258")

SET(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -Wall -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -nostdlib" CACHE INTERNAL "c compiler flags")
#SET(CMAKE_C_FLAGS "-ffunction-sections -fdata-sections -Wall -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions" CACHE INTERNAL "c compiler flags")
#SET(CMAKE_CXX_FLAGS "-fomit-frame-pointer -fshort-enums -Wall -Wpacked -Wcast-align -fdata-sections -ffunction-sections -fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics" CACHE INTERNAL "cxx compiler flags")
SET(CMAKE_ASM_FLAGS "-fomit-frame-pointer -fshort-enums -Wall -Wpacked -Wcast-align -fdata-sections -ffunction-sections -fno-use-cxa-atexit -fno-threadsafe-statics" CACHE INTERNAL "asm compiler flags")

#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -Wl,-uss_apsmeSwitchKeyReq -Wl,-u_AppWarmStart -mredzone-size=4 -mbranch-cost=3 -fomit-frame-pointer -Os -fshort-enums -nostartfiles -fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics -Wl,--gc-sections -Wl,--defsym=__stack_size=5000 -Wl,--defsym,__minimum_heap_size=2000 " CACHE INTERNAL "executable linker flags")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -Wl,-uss_apsmeSwitchKeyReq -Os -fshort-enums -nostartfiles -fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics -Wl,--gc-sections " CACHE INTERNAL "executable linker flags")


FUNCTION(ADD_BIN_TARGET TARGET)
    IF(EXECUTABLE_OUTPUT_PATH)
      SET(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    ELSE()
      SET(FILENAME "${TARGET}")
    ENDIF()
    ADD_CUSTOM_TARGET("${TARGET}.bin"
        DEPENDS ${TARGET}
        COMMAND ${CMAKE_OBJCOPY} -Obinary ${FILENAME} ${FILENAME}.bin
    )
ENDFUNCTION()


FUNCTION(PRINT_SIZE_OF_TARGETS TARGET)
    IF(EXECUTABLE_OUTPUT_PATH)
      SET(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    ELSE()
      SET(FILENAME "${TARGET}")
    ENDIF()
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_SIZE} ${FILENAME})
ENDFUNCTION()
