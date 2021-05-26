file(GLOB_RECURSE BOT_SOURCES "*.cpp" "*.h")

include_directories(SYSTEM
    ${PROJECT_SOURCE_DIR}/contrib/cpp-sc2/include
    ${PROJECT_SOURCE_DIR}/contrib/cpp-sc2/contrib/protobuf/src
    ${PROJECT_BINARY_DIR}/contrib/cpp-sc2/generated
    "."
)

link_directories(${PROJECT_BINARY_DIR}/contrib/cpp-sc2/bin)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# Create the executable.
add_executable(Monte ${BOT_SOURCES})
target_link_libraries(Monte
    sc2api sc2lib sc2utils sc2protocol civetweb libprotobuf
)

# Linux specific.
if (UNIX AND NOT APPLE)
	target_link_libraries(Monte pthread dl)
endif ()
