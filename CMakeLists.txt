cmake_minimum_required(VERSION 3.29.1)
project(file_db)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

set(OPENSSL_ROOT_DIR /usr/local)
find_package(OpenSSL 3.3.0 REQUIRED)  
add_library(${PROJECT_NAME} STATIC src/sys_utils.c src/user_db.c src/hkdf.c)
target_link_libraries(${PROJECT_NAME} PUBLIC OpenSSL::Crypto OpenSSL::SSL)
target_include_directories(${PROJECT_NAME} PUBLIC src/include)


add_executable(test_file_db src/test/test.c src/test/unity.c)
target_link_libraries(test_file_db ${PROJECT_NAME})
#target_include_directories(test_file_db PRIVATE ${OPENSSL_INCLUDE_DIR})
#target_link_libraries(test_file_db ${PROJECT_NAME} )
