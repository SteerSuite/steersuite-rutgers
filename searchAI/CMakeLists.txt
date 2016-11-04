file(GLOB SEARCHAI_SRC src/*.cpp)
file(GLOB SEARCHAI_HDR include/*.h)

add_library(searchAI SHARED ${SEARCHAI_SRC} ${SEARCHAI_HDR})
target_include_directories(searchAI PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(searchAI steerlib util)
add_dependencies(searchAI steerlib util)

install(TARGETS searchAI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES ${SEARCHAI_HDR} DESTINATION include/searchAI)
