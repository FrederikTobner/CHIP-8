macro(CHIP8_Check_Dependencies)
    if(UNIX)
    # Check dependecies under unix-like systems   
    CHECK_INCLUDE_FILE("curses.h" CURSES_AVAILABLE)
    CHECK_INCLUDE_FILE("unistd.h" UNISTD_AVAILABLE)
    if(NOT ${CURSES_AVAILABLE})
        message(FATAL_ERROR "curses.h is required to build the emulator under unix-like systems. \
    \   \   Please make sure it is available to the compiler and try again after that")
    endif() # curses.h
    if(NOT ${UNISTD_AVAILABLE})
        message(FATAL_ERROR "unistd.h is required to build the emulator under unix-like systems. \
    \   \   Please make sure it is available to the compiler and try again after that")
    endif() # unistd.h
    elseif(WIN32)
        # Check dependecies under windows
        CHECK_INCLUDE_FILE("conio.h" CONIO_AVAILABLE)
        CHECK_INCLUDE_FILE("windows.h" WINDOWS_AVAILABLE)
        if(NOT ${CONIO_AVAILABLE})
            message(FATAL_ERROR "conio.h is required to build the emulator under windows. \
        \   \   Please make sure it is available to the compiler and try again after that")
        endif() # conio.h
        if(NOT ${WINDOWS_AVAILABLE})
            message(FATAL_ERROR "windows.h is required to build the emulator under windows. \
        \   \   Please make sure it is available to the compiler and try again after that"
            )
        endif() # windows.h
    endif()
endmacro(CHIP8_Check_Dependencies)
