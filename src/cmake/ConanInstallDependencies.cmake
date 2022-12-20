# function for setting target name for build and user directory for conan

function(conan_install_dependencies TARGET_ARCH TARGET_NAME USER_DIR)
    # Find conan util
    find_program(CONAN_FOUND "conan" NO_CACHE)
    if(CONAN_FOUND)
        # Define conan's target name and target profile
        set(CONAN_PROFILE "default")
        if(${TARGET_ARCH} MATCHES "x86_64")
            set(CONAN_TARGET_NAME "x86_64")
            set(CONAN_TARGET_FOUND ON)
        elseif(${TARGET_ARCH} MATCHES "i386")
            set(CONAN_TARGET_NAME "x86")
            set(CONAN_TARGET_FOUND ON)
        elseif(${TARGET_ARCH} MATCHES "armv7")
            set(CONAN_TARGET_NAME "armv7hf")
            set(CONAN_PROFILE "armhf")
            set(CONAN_TARGET_FOUND ON)
        else()
            set(CONAN_TARGET_FOUND OFF)
        endif()

        # If conan's target was defined correctly
        if(CONAN_TARGET_FOUND)
            message(STATUS "Conan target: ${CONAN_TARGET_NAME}")
            message(STATUS "Conan profile: ${CONAN_PROFILE}")

            set(CONAN_INSTALL_COMMAND conan install .. --profile ${CONAN_PROFILE} -g cmake_find_package -s arch=${CONAN_TARGET_NAME} -s arch_build=${CONAN_TARGET_NAME} -if ${CMAKE_BINARY_DIR} --build=missing)
            set(ENV{CONAN_USER_HOME} ${USER_DIR})
            execute_process(
                COMMAND ${CONAN_INSTALL_COMMAND}
                RESULT_VARIABLE CMD_ERROR
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            message(STATUS "CMD_ERROR:" ${CMD_ERROR})
            if(${CMD_ERROR} MATCHES "0")
                message(STATUS "Conan successful installed dependencies")
            else()
                message(FATAL_ERROR "Conan failed installation of dependencies")
            endif()
        else()
            message(FATAL_ERROR "Invalid target architecture: ${TARGET_ARCH}")
        endif()
    else()
        message(FATAL_ERROR  "Conan package manager required, please download it from https://conan.io/downloads.html")
    endif()
endfunction()
