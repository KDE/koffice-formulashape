add_subdirectory( pics ) 
add_subdirectory( fonts ) 
add_subdirectory( tests )
add_subdirectory( templates )

include_directories(elements)

########### Formula shape ###############

set( koformulashape_SRCS
   KoFormulaShapePlugin.cpp
   KoFormulaShape.cpp
   FormulaCommandUpdate.cpp
   KoFormulaShapeFactory.cpp
   KoFormulaTool.cpp
   FormulaCommand.cpp
   KoFormulaToolFactory.cpp

   FormulaToolWidget.cpp
   FormulaDocument.cpp
   AttributeManager.cpp
   FormulaRenderer.cpp
   FormulaData.cpp
   FormulaCursor.cpp
   FormulaEditor.cpp
   FormulaCommand.cpp
   ElementFactory.cpp 
   Dictionary.cpp

   elements/BasicElement.cpp
   elements/FixedElement.cpp
   elements/RowElement.cpp
   elements/FormulaElement.cpp 
   elements/TextElement.cpp
   elements/FractionElement.cpp
   elements/SpaceElement.cpp
   elements/GlyphElement.cpp
   elements/IdentifierElement.cpp
   elements/OperatorElement.cpp
   elements/MultiscriptElement.cpp
   elements/SubSupElement.cpp
   elements/UnderOverElement.cpp
   elements/FencedElement.cpp 
   elements/TableElement.cpp
   elements/TableRowElement.cpp
   elements/TableDataElement.cpp
   elements/RootElement.cpp
   elements/EncloseElement.cpp
   elements/ActionElement.cpp
   elements/PaddedElement.cpp
   elements/ErrorElement.cpp
   elements/StyleElement.cpp
   elements/TokenElement.cpp
   elements/NumberElement.cpp
   elements/StringElement.cpp
   elements/PhantomElement.cpp
   elements/SquareRootElement.cpp
   elements/AnnotationElement.cpp
   elements/UnknownElement.cpp
)

kde4_add_ui_files( koformulashape_SRCS
    FormulaToolWidget.ui
)

kde4_add_plugin(koformulashape ${koformulashape_SRCS})
target_link_libraries(koformulashape ${KOMAIN_LIBS})

########### install files ###############

install(TARGETS koformulashape DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES koformulashape.desktop DESTINATION ${SERVICES_INSTALL_DIR})

