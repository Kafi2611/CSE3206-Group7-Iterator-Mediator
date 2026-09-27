# Iterator pattern - CampusConnect news feed / friends (Facebook-like)
add_library(campus_feed
    src/CampusConnect.cpp
    src/FeedIterators.cpp
    src/FeedScreen.cpp
)
target_include_directories(campus_feed PUBLIC include)

add_executable(iterator_demo main.cpp)
target_link_libraries(iterator_demo PRIVATE campus_feed)
set_target_properties(iterator_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
