#重新定义当前目标的源文件的__FILE__宏
function(RedefineFileMacro targetname)
    get_target_property(source_files "${targetname}" SOURCES)
    foreach(sourcefile ${source_files})
        get_property(defs SOURCE "${sourcefile}"
                PROPERTY COMPILE_DEFINITIONS)
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        string(REPLACE . "" relpath ${filepath})
        list(APPEND defs "__FILE__=\"${relpath}\"")
        set_property(
                SOURCE "${sourcefile}"
                PROPERTY COMPILE_DEFINITIONS ${defs}
        )
    endforeach()
endfunction()