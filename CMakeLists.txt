cmake_minimum_required(VERSION 3.0)
project(re2dfa) 

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_STANDARD 17)

link_directories(re2dfa ${PROJECT_SOURCE_DIR}/)
add_executable(re2dfa main.cpp task.cpp)

set_target_properties(re2dfa PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")

if(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin")
       target_link_libraries(re2dfa RegexCheckerCore_Darwin)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
       target_link_libraries(re2dfa RegexCheckerCore_Linux)
elseif(CMAKE_HOST_WIN32)
       target_link_libraries(re2dfa RegexCheckerCore_Windows)
endif()

install(TARGETS re2dfa DESTINATION .)
