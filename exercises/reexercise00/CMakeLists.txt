
set(libraries glad glfw itugl ${APPLE_LIBRARIES})

file(GLOB_RECURSE target_inc "*.h" )
file(GLOB_RECURSE target_src "*.cpp" )

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/shaders" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif ()

add_executable(${TARGETNAME} ${target_inc} ${target_src})
target_link_libraries(${TARGETNAME} ${libraries})
