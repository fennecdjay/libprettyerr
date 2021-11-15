set(LIBRARY_NAME "${PROJECT_NAME}")

#######################
# Dependency download #
#######################
set(downloaded_headers "${PROJECT_BINARY_DIR}/download/include")
set(termcolor_header "${downloaded_headers}/termcolor.h")
if (NOT EXISTS "${termcolor_header}")
    file(DOWNLOAD https://raw.githubusercontent.com/euppal/libtermcolor/c8b439f/include/termcolor.h
            "${termcolor_header}"
            EXPECTED_HASH "SHA256=dedd2c3f85c1cdcc6037aea037b8b485509d3101b9c496f8aedccd8336ddd270")
endif (NOT EXISTS "${termcolor_header}")
# TODO: Remove ^

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
        "$<BUILD_INTERFACE:${downloaded_headers}>"
        "$<INSTALL_INTERFACE:include>"
)
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
install(FILES "${Header_Files}" "${termcolor_header}" "${_export_file}" DESTINATION "include")

if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
install(TARGETS ${installable_libs}
        DESTINATION "lib"
        EXPORT "${LIBRARY_NAME}Targets")
