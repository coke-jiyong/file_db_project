cmake_minimum_required(VERSION 3.16.0)
project(file_db)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")

add_library(${PROJECT_NAME} STATIC src/sys_utils.c src/user_db.c src/hkdf.c)
target_link_libraries(${PROJECT_NAME} crypto)
target_include_directories(${PROJECT_NAME} PUBLIC src/include)




add_executable(test_file_db src/test/test.c src/test/unity.c)
target_link_libraries(test_file_db ${PROJECT_NAME})
set_target_properties(test_file_db PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/test
)


add_executable(db_cli src/main.c)
target_link_libraries(db_cli ${PROJECT_NAME})


