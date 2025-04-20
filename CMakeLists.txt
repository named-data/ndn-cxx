cmake_minimum_required(VERSION 3.15)
project(ndn-cxx VERSION 0.9.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(BUILD_STATIC_LIBS "Build static libraries" OFF)
# option(WITH_TESTS "Build tests" OFF)
# option(WITH_EXAMPLES "Build examples" OFF)
# option(WITH_TOOLS "Build tools" ON)
option(WITH_OSX_KEYCHAIN "Use macOS Keychain as default TPM (macOS only)" OFF)
option(WITH_SQLITE_LOCKING "Enable filesystem locking in sqlite3" ON)

# Find required packages
find_package(Boost 1.71.0 REQUIRED COMPONENTS
    chrono
    log
    thread
    program_options
)

find_package(OpenSSL 1.1.1 REQUIRED)
find_package(SQLite3 REQUIRED)

# System checks
include(CheckLibraryExists)
check_library_exists(atomic __atomic_fetch_add_4 "" HAVE_ATOMIC)
if(HAVE_ATOMIC)
    list(APPEND EXTRA_LIBS atomic)
endif()

find_package(Threads REQUIRED)
list(APPEND EXTRA_LIBS Threads::Threads)

# Version configuration
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ndn-cxx/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/ndn-cxx/version.hpp
    @ONLY
)

# Main library target
add_library(ndn-cxx)

# Source files
file(GLOB_RECURSE NDN_CXX_SOURCES 
    "ndn-cxx/*.cpp"
    "ndn-cxx/**/*.cpp"
)

# Exclude platform-specific files
if(NOT APPLE)
    list(FILTER NDN_CXX_SOURCES EXCLUDE REGEX ".*-osx\\.cpp$")
    list(FILTER NDN_CXX_SOURCES EXCLUDE REGEX ".*-osx\\.mm$")
endif()

if(NOT ANDROID)
    list(FILTER NDN_CXX_SOURCES EXCLUDE REGEX ".*-android\\.cpp$")
    list(FILTER NDN_CXX_SOURCES EXCLUDE REGEX ".*-android\\.hpp$")
endif()

target_sources(ndn-cxx PRIVATE ${NDN_CXX_SOURCES})

target_include_directories(ndn-cxx
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ndn-cxx
    PUBLIC
        Boost::chrono
        Boost::log
        Boost::thread
        OpenSSL::Crypto
        SQLite::SQLite3
        ${EXTRA_LIBS}
)

# Installation
include(GNUInstallDirs)

install(TARGETS ndn-cxx
    EXPORT ndn-cxx-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ndn-cxx/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ndn-cxx
    FILES_MATCHING PATTERN "*.hpp"
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ndn-cxx/version.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ndn-cxx
)

# Generate and install package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ndn-cxx-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ndn-cxx-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ndn-cxx-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ndn-cxx
)

install(EXPORT ndn-cxx-targets
    FILE ndn-cxx-targets.cmake
    NAMESPACE ndn-cxx::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ndn-cxx
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ndn-cxx-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ndn-cxx-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ndn-cxx
)

