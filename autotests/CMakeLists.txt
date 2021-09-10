include(ECMMarkAsTest)

set(bhawk_autotests_SRCS )

qt5_add_resources(bhawk_autotests_SRCS autotests.qrc)

macro(bhawk_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} ${_testname}.cpp ${bhawk_autotests_SRCS})
        target_link_libraries(${_testname} Qt5::Test MidoriPrivate)
        add_test(NAME bhawk-${_testname} COMMAND ${_testname})
        ecm_mark_as_test(${_testname})
        set_tests_properties(bhawk-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

bhawk_tests(
    qztoolstest
    cookiestest
    adblocktest
    updatertest
    locationbartest
    webviewtest
    webtabtest
    sqldatabasetest
)

set(bhawk_autotests_SRCS ${CMAKE_SOURCE_DIR}/tests/modeltest/modeltest.cpp)
include_directories(${CMAKE_SOURCE_DIR}/tests/modeltest)
bhawk_tests(
    tabmodeltest
)

set(bhawk_autotests_SRCS passwordbackendtest.cpp)
include_directories(${OPENSSL_INCLUDE_DIR})
bhawk_tests(
    databasepasswordbackendtest
    databaseencryptedpasswordbackendtest
)

set(bhawk_autotests_SRCS
    qml/qmltestitem.cpp
    qml/qmltesthelper.cpp
)

macro(bhawk_qml_tests)
    foreach(_testname ${ARGN})
        add_executable(${_testname} qml/${_testname}.cpp ${bhawk_autotests_SRCS})
        target_link_libraries(${_testname} Qt5::Test MidoriPrivate)
        add_test(NAME bhawk-qml-${_testname} COMMAND ${_testname})
        ecm_mark_as_test(${_testname})
        set_tests_properties(bhawk-qml-${_testname} PROPERTIES RUN_SERIAL TRUE)
    endforeach(_testname)
endmacro()

bhawk_qml_tests(
    qmlbookmarksapitest
    qmltopsitesapitest
    qmlhistoryapitest
    qmlcookiesapitest
    qmlclipboardapitest
    qmltabsapitest
    qmlwindowsapitest
    qmluserscriptapitest
)
