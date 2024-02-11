set(SOLUTION_BIN "${CMAKE_PROJECT_NAME}_sam-solution")
add_executable(${SOLUTION_BIN} 
                        main.cpp
                        client.cpp
                        server.cpp)
target_link_libraries(${SOLUTION_BIN} ${ECHO_WAVE_LIB}) # Need to link the proto library built