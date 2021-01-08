lldb_tablegen(ScriptedProcessOptions.inc -gen-lldb-option-defs
  SOURCE ScriptedProcessOptions.td
  TARGET LLDBPluginScriptedProcessOptionGen)

add_lldb_library(lldbPluginScriptedProcess PLUGIN
  ScriptedProcess.cpp

  LINK_LIBS
    lldbCore
    lldbTarget
    lldbUtility
    lldbPluginProcessUtility
  LINK_COMPONENTS
    BinaryFormat
    Object
    Support
  )

add_dependencies(lldbPluginScriptedProcess
  LLDBPluginScriptedProcessOptionGen
)
