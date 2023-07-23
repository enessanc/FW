add_library(FW
        STATIC
        #Interface Files
        interface/FWContext.cpp
        interface/System.cpp
        #Implementation Files
        implementation/FWImpl.cpp
        implementation/SystemImpl.cpp
        implementation/mavlink_message_handler/net/NetworkHandler.cpp
        implementation/mavlink_message_handler/net/infrastructure/SerialConnection.cpp
        implementation/mavlink_message_handler/net/infrastructure/UDPConnection.cpp
        implementation/utility/StringUtility.cpp implementation/mavlink_message_handler/MavlinkMessageSender.cpp implementation/mavlink_message_handler/MavlinkMessageReceiver.cpp implementation/mavlink_message_handler/MavlinkMessageHandler.cpp)

target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/interface)
target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/implementation)