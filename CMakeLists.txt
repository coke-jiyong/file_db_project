project(file_db)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")


find_package(OpenSSL REQUIRED)
add_library(${PROJECT_NAME} STATIC src/sys_utils.c src/user_db.c)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)
target_include_directories(${PROJECT_NAME} PUBLIC src/include)


add_executable(test_file_db src/test/test.c src/test/unity.c)
target_link_libraries(test_file_db ${PROJECT_NAME})