project(file_db)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")


add_executable(${PROJECT_NAME} src/sys_utils.c src/user_db.c)
find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::Crypto)