cmake_minimum_required (VERSION 3.0)

project (Simple_CS_app)

SET(GCC_COVERAGE_LINK_FLAGS  "systemd")

# add the executable
add_executable (Imaqliq_Server server.c)
add_executable (Client client.c)

target_link_libraries(Imaqliq_Server ${GCC_COVERAGE_LINK_FLAGS})