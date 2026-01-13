project(ServerHTTP CXX)

add_executable(ServerHTTP main.cpp server.cpp)

install(TARGETS ServerHTTP DESTINATION /bin)
install(DIRECTORY static DESTINATION share/http-server)
