get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

set(download_dir "${PROJECT_BINARY_DIR}/termcolor")
set(termcolor_header "${download_dir}/termcolor.h")
if (NOT EXISTS "${termcolor_header}")
    file(DOWNLOAD https://raw.githubusercontent.com/euppal/libtermcolor/c8b439f/include/termcolor.h
            "${termcolor_header}"
            EXPECTED_HASH "SHA256=dedd2c3f85c1cdcc6037aea037b8b485509d3101b9c496f8aedccd8336ddd270")
endif (NOT EXISTS "${termcolor_header}")

set(termcolor_src "${download_dir}/termcolor.c")
if (NOT EXISTS "${termcolor_src}")
    file(DOWNLOAD https://raw.githubusercontent.com/SamuelMarks/libtermcolor/9abe3f4/src/termcolor.c
            "${termcolor_src}"
            EXPECTED_HASH "SHA256=ab84d8f3b2d9aa6d1f13fe5794eabb8aad1a92943e2bb0abc00e1eb757af99d7")
endif (NOT EXISTS "${termcolor_src}")


set(Header_Files "${termcolor_header}")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "${termcolor_src}")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${download_dir}>"
        "$<INSTALL_INTERFACE:include>"
)
set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

#################
# install rules #
#################

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "1")

set(installable_libs "${LIBRARY_NAME}" "${PROJECT_NAME}_compiler_flags")
install(FILES "${Header_Files}" DESTINATION "include")

if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
install(TARGETS ${installable_libs}
        DESTINATION "lib"
        EXPORT "${LIBRARY_NAME}Targets")
