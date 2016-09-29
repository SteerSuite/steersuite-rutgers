file(GLOB COLLISIONAI_SRC src/*.cpp)
file(GLOB COLLISIONAI_HDR include/*.h)

add_library(collisionAI SHARED ${COLLISIONAI_SRC} ${COLLISIONAI_HDR})
target_include_directories(collisionAI PRIVATE
  ./include
  ../external
  ../steerlib/include
  ../util/include
)
target_link_libraries(collisionAI steerlib util)
add_dependencies(collisionAI steerlib util)

install(TARGETS collisionAI
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(FILES ${COLLISIONAI_HDR} DESTINATION include/collisionAI)
