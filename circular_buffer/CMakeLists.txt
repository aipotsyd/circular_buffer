add_executable(
    cb_test 
        "src/cb_test.cpp"
		"src/circular_buffer.h"
)

add_custom_command(TARGET cb_test POST_BUILD COMMAND cb_test)
