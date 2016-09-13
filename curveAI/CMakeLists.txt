file(GLOB CURVEAI_SRC src/*.cpp)
file(GLOB CURVEAI_HDR include/*.h)

add_library(curveAI SHARED ${CURVEAI_SRC} ${CURVEAI_HDR})
target_include_directories(curveAI PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(curveAI steerlib util)
add_dependencies(curveAI steerlib util)

install(TARGETS curveAI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES ${CURVEAI_HDR} DESTINATION include/curveAI)
