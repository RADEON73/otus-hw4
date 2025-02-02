cmake_minimum_required(VERSION 3.29)

if (DEFINED ENV{USE_VCPKG_TOOLCHAIN} AND "$ENV{USE_VCPKG_TOOLCHAIN}" STREQUAL "1")
    set(CMAKE_TOOLCHAIN_FILE "E:/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

project(print_ip VERSION 1.0.0)

option(WITH_GTEST "Whether to build Google test" OFF) #без тестов в этот раз, нет в условиях :)

add_executable(main 
main.cpp
)

set_target_properties(main PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

if (MSVC)
    target_compile_options(main PRIVATE /W4)
else ()
    target_compile_options(main PRIVATE -Wall -Wextra -pedantic) 
endif()

if(WITH_GTEST)
	if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
	set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
	endif()

	find_package(GTest CONFIG REQUIRED)
	add_executable(gtests gtests.cpp)
	
	set_target_properties(gtests PROPERTIES
		CXX_STANDARD 20
		CXX_STANDARD_REQUIRED ON
	)
	
	target_link_libraries(gtests PRIVATE 
	GTest::gtest 
	GTest::gtest_main
	)
	
	if (MSVC)
	    target_compile_options(gtests PRIVATE /W4)
	else ()
	    target_compile_options(gtests PRIVATE -Wall -Wextra -pedantic)
	endif()
	
	enable_testing()
	add_test(NAME GTEST COMMAND gtests)
endif()

install(TARGETS main RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)
include(CPack)




