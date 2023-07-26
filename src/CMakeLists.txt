add_library(FW
        STATIC
        #Interface Files
        interface/FWContext.cpp
        implementation/System.cpp
        #Implementation Files
        implementation/FWImpl.cpp
        implementation/message_handler/net/NetworkHandler.cpp
        implementation/message_handler/net/infrastructure/SerialSocket.cpp
        implementation/message_handler/net/infrastructure/UDPSocket.cpp
        implementation/utility/StringUtility.cpp
        implementation/message_handler/mavlink_handler/MavlinkSubscriber.cpp
        implementation/message_handler/mavlink_handler/MavlinkCommander.cpp
        implementation/message_handler/MessageHandler.cpp implementation/message_handler/net/GenericSocket.cpp)

target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/interface)
target_include_directories(FW PUBLIC ${CMAKE_CURRENT_LIST_DIR}/implementation)