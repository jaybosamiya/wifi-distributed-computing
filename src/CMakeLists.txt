# CMakeLists.txt

cmake_minimum_required(VERSION 2.8)
project( wifi-distributed-computing )
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package( PCAP REQUIRED )
add_library( common OBJECT util.cpp pcap_manager.cpp math_packet.cpp )
add_executable( wifi-math-server server.cpp $<TARGET_OBJECTS:common> )
add_executable( wifi-math-client client.cpp $<TARGET_OBJECTS:common> )
add_executable( rpn-tester rpn_tester.cpp $<TARGET_OBJECTS:common> )
target_link_libraries( wifi-math-server ${PCAP_LIBRARY} )
target_link_libraries( wifi-math-client ${PCAP_LIBRARY} )
target_link_libraries( rpn-tester ${PCAP_LIBRARY} )
