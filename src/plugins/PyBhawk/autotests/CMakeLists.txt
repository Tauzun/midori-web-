macro(pybhawk_tests)
    foreach(_testname ${ARGN})
        add_test(NAME pybhawk-${_testname} COMMAND pybhawktestrunner ${CMAKE_CURRENT_SOURCE_DIR}/${_testname}.py)
        set_tests_properties(pybhawk-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

add_executable(pybhawktestrunner pybhawktestrunner.cpp)
target_link_libraries(pybhawktestrunner MidoriPrivate Qt5::Test)

pybhawk_tests(
    basictest
)
