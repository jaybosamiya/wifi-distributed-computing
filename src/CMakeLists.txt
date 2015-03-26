# CMakeLists.txt

cmake_minimum_required(VERSION 2.8)
project( wifi-distributed-computing )
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package( PCAP REQUIRED )
add_library( util OBJECT util.cpp )
add_executable( wifi-math-server server.cpp $<TARGET_OBJECTS:util> )
add_executable( wifi-math-client client.cpp $<TARGET_OBJECTS:util> )
target_link_libraries( wifi-math-server ${PCAP_LIBRARY} )
target_link_libraries( wifi-math-client ${PCAP_LIBRARY} )
