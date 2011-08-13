project(formula-shape)

find_package(KDE4 REQUIRED)
find_package(KOfficeLibs REQUIRED)

include (KDE4Defaults)

include_directories(${KDE4_INCLUDES} ${QT_INCLUDES} ${KOFFICELIBS_INCLUDE_DIR} ${KOFFICELIBS_INCLUDE_DIR}/private/)

add_subdirectory(formulashape)
add_subdirectory(filters)
add_subdirectory(doc)

