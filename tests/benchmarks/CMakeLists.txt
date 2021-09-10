set(bhawk_benchmarks_SRCS )

qt5_add_resources(bhawk_benchmarks_SRCS benchmarks.qrc)

macro(bhawk_benchmarks)
    foreach(_benchmarkname ${ARGN})
        add_executable(${_benchmarkname} ${_benchmarkname}.cpp ${bhawk_benchmarks_SRCS})
        target_link_libraries(${_benchmarkname} Qt5::Test MidoriPrivate)
        #add_test(NAME bhawk-${_benchmarkname} COMMAND ${_benchmarkname})
        #ecm_mark_as_test(${_benchmarkname})
        #set_tests_properties(bhawk-${_benchmarkname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_benchmarkname)
endmacro()

bhawk_benchmarks(
    #adblockmatchrule
    adblockparserule
)
