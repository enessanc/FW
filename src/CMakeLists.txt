add_library(FW
        STATIC
        #Interface Files
        interface/FWContext.cpp
        interface/System.cpp
        #Implementation Files
        implementation/FWImpl.cpp
        implementation/SystemImpl.cpp
        implementation/mavlink_message_handler/MavlinkMessageHandler.cpp
        implementation/mavlink_message_handler/net/infrastructure/SerialConnection.cpp
        implementation/mavlink_message_handler/net/infrastructure/UDPConnection.cpp
        )

target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/interface)