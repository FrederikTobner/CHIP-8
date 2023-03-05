macro(CHIP8_Detect_Platform)
    # Adding os-specific compiler definitions
    if(UNIX)
    add_compile_definitions(OS_UNIX_LIKE)    
    if(NOT APPLE) 
        if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
            add_compile_definitions(OS_LINUX)
        elseif(CMAKE_SYSTEM_NAME MATCHES "kFreeBSD.*")
            add_compile_definitions(OS_FREEBSD)
        elseif(CMAKE_SYSTEM_NAME MATCHES "kNetBSD.*|NetBSD.*")
            add_compile_definitions(OS_NETBSD)
        elseif(CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
            add_compile_definitions(OS_OPENBSD)
        elseif(CMAKE_SYSTEM_NAME MATCHES ".*GNU.*")
            add_compile_definitions(OS_GNU)
        elseif(CMAKE_SYSTEM_NAME MATCHES ".*BSDI.*")
            add_compile_definitions(OS_BSDI)
        elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly.*|FreeBSD")
            add_compile_definitions(OS_FREEBSD)
        elseif(CMAKE_SYSTEM_NAME MATCHES "SYSV5.*")
            add_compile_definitions(OS_SYSV5)
        elseif(CMAKE_SYSTEM_NAME MATCHES "Solaris.*|SunOS.*")
            add_compile_definitions(OS_SOLARIS)
        elseif(CMAKE_SYSTEM_NAME MATCHES "HP-UX.*")
            add_compile_definitions(OS_HPUX)
        elseif(CMAKE_SYSTEM_NAME MATCHES "AIX.*")
            add_compile_definitions(OS_AIX)
        elseif(CMAKE_SYSTEM_NAME MATCHES "Minix.*")
            add_compile_definitions(OS_MINIX)
        else()
            add_compile_definitions(OS_UNKNOWN_UNIX)
        endif()
    else() 
        if(CMAKE_SYSTEM_NAME MATCHES ".*Darwin.*")
            add_compile_definitions(OS_DARWIN)
        elseif(CMAKE_SYSTEM_NAME MATCHES ".*MacOS.*")
            add_compile_definitions(OS_MACOS)
        elseif(CMAKE_SYSTEM_NAME MATCHES ".*tvOS.*")
            add_compile_definitions(OS_TVOS)
        elseif(CMAKE_SYSTEM_NAME MATCHES ".*iOS.*")
            add_compile_definitions(OS_IOS)
        else()
            add_compile_definitions(OS_UNKNOWN_APPLE)
        endif()
    endif()
    elseif(WIN32)
    add_compile_definitions(OS_WINDOWS)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Haiku.*")
    message(FATAL_ERROR "The emulator has not been implemented for Haiku yet")
    elseif(NINTENDO_3DS)
    message(FATAL_ERROR "The emulator has not been implemented for the 3DS yet")
    else()
    message(FATAL_ERROR "The underlying operating system was not recognized")
    endif()
endmacro(CHIP8_Detect_Platform)