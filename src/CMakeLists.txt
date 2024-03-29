set(LIBRARY_NAME "${PROJECT_NAME}")

# TODO: Remove once PR is merged to libtermcolor
add_subdirectory("termcolor")

set(Header_Files "internal/error.h" "prettyerr.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "internal/error.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/internal>"
        "$<INSTALL_INTERFACE:include>"
)
target_link_libraries("${LIBRARY_NAME}" PUBLIC "termcolor")
set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

# Symbol exporter
include(GenerateExportHeader)
set(_export_file "${CMAKE_CURRENT_SOURCE_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

#################
# install rules #
#################

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "1")

set(installable_libs "${LIBRARY_NAME}" "${PROJECT_NAME}_compiler_flags")
install(FILES "${Header_Files}" "${_export_file}" DESTINATION "include")

if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
install(TARGETS ${installable_libs}
        DESTINATION "lib"
        EXPORT "${LIBRARY_NAME}Targets")
