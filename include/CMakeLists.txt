# copy headers via install target (exclude hidden directories)
install_lib(DIRECTORY flecc_in_c
            DESTINATION include
            PATTERN ".*" EXCLUDE)

# register headers for reformating
file(GLOB_RECURSE Sources "flecc_in_c/*.h")
add_file_to_format(${Sources})
