cmake_minimum_required (VERSION 3.15)
project (eco VERSION 1.0.0)
enable_language (C ASM)


option (BUILD_SHARED_LIBRARY "Build shared library" ON)
option (BUILD_STATIC_LIBRARY "Build static library" ON)
option (ECO_BUILD_TESTS "Build tests" ON)
option (ECO_WITH_VALGRIND "Enable valgrind support" ON)

if (ECO_BUILD_TESTS)
  enable_testing ()
endif (ECO_BUILD_TESTS)


set (CMAKE_C_STANDARD 99)
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og -ggdb")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3")


file (GLOB ECO_C_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file (GLOB ECO_ASM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.S)
foreach (file ${ECO_ASM_SRC})
  set_property (
    SOURCE ${file}
    APPEND PROPERTY COMPILE_OPTIONS "-x" "assembler-with-cpp")
endforeach()
list (APPEND ECO_SRC ${ECO_C_SRC} ${ECO_ASM_SRC})


include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)
if (ECO_WITH_VALGRIND)
  add_definitions (-DECO_WITH_VALGRIND=1)
endif (ECO_WITH_VALGRIND)


install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ".")
if (CMAKE_INSTALL_PREFIX)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/eco.pc ${CMAKE_BINARY_DIR}/eco.pc @ONLY)
  install (FILES ${CMAKE_BINARY_DIR}/eco.pc DESTINATION lib/pkgconfig)
endif (CMAKE_INSTALL_PREFIX)


if (BUILD_SHARED_LIBRARY)
  add_library (eco_so SHARED ${ECO_SRC})
  set_target_properties (eco_so
    PROPERTIES
      POSITION_INDEPENDENT_CODE ON
      OUTPUT_NAME "eco")
  install (TARGETS eco_so DESTINATION "lib")
endif (BUILD_SHARED_LIBRARY)


if (BUILD_STATIC_LIBRARY)
  add_library (eco_a STATIC ${ECO_SRC})
  set_target_properties (eco_a
    PROPERTIES
      OUTPUT_NAME "eco")
  install (TARGETS eco_a DESTINATION "lib")
endif (BUILD_STATIC_LIBRARY)


if (ECO_BUILD_TESTS)
  add_executable (t ${CMAKE_CURRENT_SOURCE_DIR}/t/t.c $<TARGET_OBJECTS:eco_a>)
  add_dependencies (t eco_a)
  add_test (t ${CMAKE_BINARY_DIR}/t)
endif (ECO_BUILD_TESTS)
