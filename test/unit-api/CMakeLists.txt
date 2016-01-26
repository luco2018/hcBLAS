# Object libraries require CMAKE 2.8.8 version 
CMAKE_MINIMUM_REQUIRED (VERSION 2.8.8) 
MESSAGE(STATUS "CMAKE VERSION ${CMAKE_VERSION}")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
# Find HCC compiler
FIND_PACKAGE(HC++ 1.0 REQUIRED)

file(GLOB SRCS *.cpp)

# Choice to take compilation flags from source or package
if(EXISTS ${MCWHCCBUILD})
  execute_process(COMMAND ${HCC_CONFIG} --build --cxxflags
                         OUTPUT_VARIABLE HCC_CXXFLAGS)
  execute_process(COMMAND ${HCC_CONFIG} --build --ldflags --gtest 
                            OUTPUT_VARIABLE HCC_LDFLAGS)
else(EXISTS ${MCWHCCBUILD})
  execute_process(COMMAND ${HCC_CONFIG} --install --cxxflags
                            OUTPUT_VARIABLE HCC_CXXFLAGS)
  execute_process(COMMAND ${HCC_CONFIG}  --install --ldflags --gtest
                            OUTPUT_VARIABLE HCC_LDFLAGS)
endif(EXISTS ${MCWHCCBUILD})


string(STRIP "${HCC_CXXFLAGS}" HCC_CXXFLAGS)
string(STRIP "${HCC_LDFLAGS}" HCC_LDFLAGS)
set (HCC_CXXFLAGS "${HCC_CXXFLAGS}")
set (HCC_LDFLAGS "${HCC_LDFLAGS}")

SET_PROPERTY(SOURCE ${SRCS} APPEND_STRING PROPERTY COMPILE_FLAGS " ${HCC_CXXFLAGS} -DGTEST_HAS_TR1_TUPLE=0 ")
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
ADD_EXECUTABLE(bin/unit-api-test ${SRCS})
SET(LINK "-lhc_am -lblas")
SET_PROPERTY(TARGET bin/unit-api-test APPEND_STRING PROPERTY LINK_FLAGS " ${HCC_LDFLAGS} ${LINK}")

TARGET_LINK_LIBRARIES(bin/unit-api-test hcblas)

add_test(NAME unit-api-test COMMAND ./bin/unit-api-test)