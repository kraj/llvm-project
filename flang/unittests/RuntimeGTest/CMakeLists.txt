add_flang_unittest(FlangRuntimeTests
  CharacterTest.cpp
  RuntimeCrashTest.cpp
  CrashHandlerFixture.cpp
  NumericalFormatTest.cpp
  RuntimeCrashTest.cpp
  CrashHandlerFixture.cpp
  ListInputTest.cpp
)

target_link_libraries(FlangRuntimeTests
  PRIVATE
  FortranRuntime
)
