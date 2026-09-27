# Mediator pattern - CampusConnect Messenger (Messenger-like)
add_library(campus_chat
    src/User.cpp
    src/Member.cpp
    src/SupportBot.cpp
    src/MessengerServer.cpp
)
target_include_directories(campus_chat PUBLIC include)

add_executable(mediator_demo main.cpp)
target_link_libraries(mediator_demo PRIVATE campus_chat)
set_target_properties(mediator_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
