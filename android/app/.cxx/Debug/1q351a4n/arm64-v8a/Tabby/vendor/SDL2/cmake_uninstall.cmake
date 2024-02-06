if (NOT EXISTS "/Users/tabby/Projects/C++/TabbyEngine/android/app/.cxx/Debug/1q351a4n/arm64-v8a/install_manifest.txt")
    message(FATAL_ERROR "Cannot find install manifest: \"/Users/tabby/Projects/C++/TabbyEngine/android/app/.cxx/Debug/1q351a4n/arm64-v8a/install_manifest.txt\"")
endif(NOT EXISTS "/Users/tabby/Projects/C++/TabbyEngine/android/app/.cxx/Debug/1q351a4n/arm64-v8a/install_manifest.txt")

file(READ "/Users/tabby/Projects/C++/TabbyEngine/android/app/.cxx/Debug/1q351a4n/arm64-v8a/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach (file ${files})
    message(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
    execute_process(
        COMMAND /Users/tabby/Library/Android/sdk/cmake/3.22.1/bin/cmake -E remove "$ENV{DESTDIR}${file}"
        OUTPUT_VARIABLE rm_out
        RESULT_VARIABLE rm_retval
    )
    if(NOT ${rm_retval} EQUAL 0)
        message(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
    endif (NOT ${rm_retval} EQUAL 0)
endforeach(file)

