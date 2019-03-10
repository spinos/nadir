file (GLOB APP_SOURCE
*.h
*.cpp)

add_executable (testapp ${APP_SOURCE})

set_target_properties( testapp PROPERTIES CXX_STANDARD 11 )

target_link_libraries (testapp ../ExampleLib)
