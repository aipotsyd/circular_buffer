if(MSVC)
    add_compile_options("/W4"  "/std:c++17" "$<$<CONFIG:RELEASE>:/O2>" "-D_SCL_SECURE_NO_WARNINGS" "/diagnostics:caret")
else()
    add_compile_options("-Wall" "-Wextra" "-Werror" "$<$<CONFIG:RELEASE>:-O3>")
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options("-stdlib=libc++")
    else()
        # nothing special for gcc at the moment
    endif()
endif()

add_executable(
    cb_test 
        "src/cb_test.cpp"
		"src/circular_buffer.h"
)

target_compile_features(cb_test PUBLIC cxx_std_17)

add_custom_command(TARGET cb_test POST_BUILD COMMAND cb_test -b -d yes)
