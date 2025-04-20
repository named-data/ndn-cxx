# Find required packages
find_package(Boost 1.71.0 REQUIRED COMPONENTS program_options)

# Find all tool source files
file(GLOB TOOL_SOURCES "*.cpp")

foreach(TOOL_SOURCE ${TOOL_SOURCES})
    get_filename_component(TOOL_NAME ${TOOL_SOURCE} NAME_WE)
    add_executable(${TOOL_NAME} ${TOOL_SOURCE})
    target_link_libraries(${TOOL_NAME}
        PRIVATE
            ndn-cxx-lib
            Boost::program_options
    )
    install(TARGETS ${TOOL_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endforeach() 