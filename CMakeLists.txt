cmake_minimum_required (VERSION 3.0)

project (Simple_CS_app)

# include_directories (
#     "${CMAKE_CURRENT_SOURCE_DIR}/Camera"
#     )
# add_subdirectory (Camera)
# set (CAMERA_LIB ${CAMERA_LIB} Camera)

# add the executable
add_executable (Server server.c)
add_executable (Client client.c)


#target_link_libraries (PointPosition ${CAMERA_LIB} ${EIGEN3_LIBS} ${OpenCV_LIBS})