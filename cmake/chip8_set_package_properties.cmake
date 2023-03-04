macro(CHIP8_Set_Package_Properties)
     # Setting project properties used by CPack
     set(PROJECT_PAGE "https://github.com/FrederikTobner/CHIP-8")
     set(PROJECT_HELP_PAGE "https://github.com/FrederikTobner/CHIP-8/wiki")
     set(PROJECT_VENDOR "Frederik Tobner")
     set(PROJECT_CONTACT "f.tobner@gmail.com")
     set(PROJECT_VERSIONED_NAME "${PROJECT_NAME}_${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")
     # Sets properties for the package created using cpack
     set(CPACK_BUILD_SOURCE_DIRS "${PROJECT_BINARY_DIR}/src")
     # Creates a desktop shortcut
     set(CPACK_CREATE_DESKTOP_LINKS ${PROJECT_NAME})
     # Sets the package description
     set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} Emulator")
     # Creates a short cut in the start menu
     set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})
     # Configurating the installation directory
     set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_VERSIONED_NAME})
     # Setting the name of the package
     set(CPACK_PACKAGE_NAME ${PROJECT_NAME}) 
     # Setting the project vendor  
     set(CPACK_PACKAGE_VENDOR ${PROJECT_VENDOR})
     # Setting the project version
     set(CPACK_PACKAGE_VERSION "${CHIP-8_VERSION_MAJOR}.${CHIP-8_VERSION_MINOR}.${CHIP-8_VERSION_PATCH}")
     # Add license as resouce
     set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
     
     # Sets properties for NSIS (nullsoft scriptable install system - https://sourceforge.net/projects/nsis/)
     # Setting the branding text that is displayed at the bottom of the install window.   
     set(CPACK_NSIS_BRANDING_TEXT ${PROJECT_NAME})
     # Setting contact info
     set(CPACK_NSIS_CONTACT ${PROJECT_CONTACT})
     # Configurating help page
     set(CPACK_NSIS_HELP_LINK ${PROJECT_HELP_PAGE})
     # Add option to add the installed program to the path 
     set(CPACK_NSIS_MODIFY_PATH "ON")
     # Setting the icon if the installer
     set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/icons/file_type_chip8.ico")
     # Setting the package name of the nsis package
     set(CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
     # About URL provided in the installer
     set(CPACK_NSIS_URL_INFO_ABOUT ${PROJECT_PAGE})     
endmacro(CHIP8_Set_Package_Properties)
