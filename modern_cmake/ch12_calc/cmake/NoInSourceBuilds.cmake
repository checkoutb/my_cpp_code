
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR
    "\n"
    "in source build are not allowed.\n"
    "plz use: cmake -B <destination>\n"
    "plz remove file you created: rm -rf CMakeFiles CMakeCache.txt\n"
  )
endif()
