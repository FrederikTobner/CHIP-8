# Makes the SDL2::SDL2 and SDL2::SDL2main targets available.
# An installed SDL2 is preferred, SDL2 is downloaded and built only if none is found
# (or if CP8_FETCH_SDL is ON).
macro(CHIP8_Find_SDL2)
    if(NOT CP8_FETCH_SDL)
        find_package(SDL2 ${CP8_SDL2_MIN_VERSION} QUIET CONFIG)
    endif()

    if(SDL2_FOUND)
        message(STATUS "Using installed SDL2 ${SDL2_VERSION}")
    else()
        message(STATUS "Fetching SDL2 ${CP8_SDL2_FETCH_TAG}")
        include(FetchContent)
        set(SDL_SHARED OFF CACHE BOOL "" FORCE)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        set(SDL_TEST OFF CACHE BOOL "" FORCE)
        set(SDL2_DISABLE_INSTALL ON CACHE BOOL "" FORCE)
        FetchContent_Declare(SDL2
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG ${CP8_SDL2_FETCH_TAG}
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(SDL2)
        # Older SDL2 versions do not provide namespaced targets when built as part of the project
        if(NOT TARGET SDL2::SDL2)
            add_library(SDL2::SDL2 ALIAS SDL2-static)
        endif()
        if(NOT TARGET SDL2::SDL2main)
            add_library(SDL2::SDL2main ALIAS SDL2main)
        endif()
    endif()
endmacro(CHIP8_Find_SDL2)
