# Setup prepare_builtins tools
set( LLVM_LINK_COMPONENTS
  BitReader
  BitWriter
  Core
  IRReader
  Support
)

if( LIBCLC_STANDALONE_BUILD )
  message("cross ${CMAKE_CROSSCOMPILING}")
  if(CMAKE_CROSSCOMPILING)
    find_program(PREPARE_BUILTINS prepare_builtins REQUIRED)
    message("found ${PREPARE_BUILTINS}")
    set( prepare_builtins_exe ${PREPARE_BUILTINS} )
  else()
    add_llvm_executable( prepare_builtins utils/prepare-builtins.cpp )
    set( prepare_builtins_exe prepare_builtins PARENT_SCOPE )
    set( prepare_builtins_target prepare_builtins PARENT_SCOPE )
    # These were not properly reported in early LLVM and we don't need them
    target_compile_options( prepare_builtins PRIVATE -fno-rtti -fno-exceptions )
  endif()
else()
  add_llvm_utility( prepare_builtins prepare-builtins.cpp )
  setup_host_tool( prepare_builtins PREPARE_BUILTINS prepare_builtins_exe prepare_builtins_target )
  # These were not properly reported in early LLVM and we don't need them
  target_compile_options( prepare_builtins PRIVATE -fno-rtti -fno-exceptions )
endif()
