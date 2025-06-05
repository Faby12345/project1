cmake_minimum_required(VERSION 3.16)

project(project1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC   ON)
set(CMAKE_AUTOMOC  ON)
set(CMAKE_AUTORCC  ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find both Widgets and Network modules
find_package(QT   NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED       COMPONENTS Widgets Network)

# List all your sources here
set(PROJECT_SOURCES
    main.cpp

    MainWindow.h
    MainWindow.cpp

    ChatDialog.h

    ArtObject.h
    artobject.cpp

    Painting.h
    painting.cpp

    Sculpture.h
    sculpture.cpp

    DigitalArt.h
    digitalart.cpp

    ArtRepository.h
    artrepository.cpp
)

if (Qt${QT_VERSION_MAJOR}_MAJOR EQUAL 6)
    qt_add_executable(project1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(project1 ${PROJECT_SOURCES}
      api.h api.cpp
      ArtRepositoryInterface.h
      csvrepository.h csvrepository.cpp
      jsonrepository.h jsonrepository.cpp
      ../art_data.json
      ../art_data_csv.csv
      Command.h
      test.h test.cpp


    )
endif()

# Link to Widgets and Network
target_link_libraries(project1
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
)

# macOS bundle settings
if (QT_VERSION VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.project1)
endif()
set_target_properties(project1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS project1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (Qt${QT_VERSION_MAJOR}_MAJOR EQUAL 6)
    qt_finalize_executable(project1)
endif()
