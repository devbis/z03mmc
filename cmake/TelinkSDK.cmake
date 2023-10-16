# Check mandatory parameters
if(NOT SDK_PREFIX)
    message(FATAL_ERROR "No SDK_PREFIX specified (it must point to the root of Telink Zigbee SDK)")
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

SET(CMAKE_C_FLAGS "-O2 -ffunction-sections -fdata-sections -Wall -fpack-struct -fshort-enums -finline-small-functions -std=gnu99 -fshort-wchar -fms-extensions -nostdlib" CACHE INTERNAL "c compiler flags")
SET(CMAKE_ASM_FLAGS "-fomit-frame-pointer -fshort-enums -Wall -Wpacked -Wcast-align -fdata-sections -ffunction-sections -fno-use-cxa-atexit -fno-threadsafe-statics" CACHE INTERNAL "asm compiler flags")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -Wl,-uss_apsmeSwitchKeyReq -Os -fshort-enums -nostartfiles -fno-rtti -fno-exceptions -fno-use-cxa-atexit -fno-threadsafe-statics -Wl,--gc-sections " CACHE INTERNAL "executable linker flags")

MACRO(REMOVE_C_FLAG flag)
    string(REPLACE "${flag}" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
ENDMACRO()

# reduce size by 15% using -O2 instead of -O3
REMOVE_C_FLAG("-O3")

FUNCTION(ADD_BIN_TARGET TARGET TOOLS_PATH)
    IF(EXECUTABLE_OUTPUT_PATH)
      SET(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    ELSE()
      SET(FILENAME "${TARGET}")
    ENDIF()
    ADD_CUSTOM_TARGET("${TARGET}.bin"
        DEPENDS ${TARGET}
        COMMAND ${CMAKE_OBJCOPY} -Obinary ${FILENAME} ${FILENAME}.bin
        COMMAND python3 ${TOOLS_PATH}/tl_check_fw.py ${FILENAME}.bin
    )
ENDFUNCTION()

FUNCTION(ADD_OTA_TARGET TARGET TOOLS_PATH)
    IF(EXECUTABLE_OUTPUT_PATH)
      SET(FILENAME "${EXECUTABLE_OUTPUT_PATH}/${TARGET}")
    ELSE()
      SET(FILENAME "${TARGET}")
    ENDIF()
    ADD_CUSTOM_TARGET("${TARGET}.zigbee"
        DEPENDS ${TARGET}.bin
        COMMAND python3 ${TOOLS_PATH}/make_ota.py ${FILENAME}.bin
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
