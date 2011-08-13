
set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )
add_definitions(-DCOMPILING_TESTS)

include_directories(.. ../elements)

########### AttributeManager Test ###############

set( TestAttributeManager_SRCS TestAttributeManager.cpp )
kde4_add_unit_test( TestAttributeManager TESTNAME kformula-AttributeManager  ${TestAttributeManager_SRCS} )
target_link_libraries( TestAttributeManager koformulashape ${QT_QTTEST_LIBRARY} )

########### Load Test ###############

set(TestLoad_SRCS TestLoad.cpp)
kde4_add_unit_test(TestLoad TESTNAME kformula-Load  ${TestLoad_SRCS})
target_link_libraries(TestLoad koformulashape ${QT_QTTEST_LIBRARY})

########### Load and Save Test ###############

set(TestLoadAndSave_SRCS TestLoadAndSave.cpp)
kde4_add_unit_test(TestLoadAndSave TESTNAME kformula-LoadAndSave  ${TestLoadAndSave_SRCS})
target_link_libraries(TestLoadAndSave koformulashape ${QT_QTTEST_LIBRARY})

########### Layout Test ###############

set(TestLayout_SRCS TestLayout.cpp)
kde4_add_unit_test(TestLayout TESTNAME kformula-Layout  ${TestLayout_SRCS})
target_link_libraries(TestLayout koformulashape ${QT_QTTEST_LIBRARY})

########### Cursor Test ###############

set(TestCursor_SRCS
TestCursor.cpp
   ../KoFormulaShape.cpp
   ../FormulaDocument.cpp
   ../FormulaToolWidget.cpp
   ../FormulaCommandUpdate.cpp
   ../KoFormulaShapeFactory.cpp
   ../KoFormulaTool.cpp
   ../FormulaCommand.cpp
)
if(NOT WIN32)
set(TestCursor_SRCS
    ${TestCursor_SRCS}
    ../FormulaCursor.cpp)
endif(NOT WIN32)

kde4_add_ui_files( TestCursor_SRCS
    ../FormulaToolWidget.ui
)

kde4_add_unit_test(TestCursor TESTNAME kformula-Cursor  ${TestCursor_SRCS})
target_link_libraries(TestCursor koformulashape ${QT_QTTEST_LIBRARY})
