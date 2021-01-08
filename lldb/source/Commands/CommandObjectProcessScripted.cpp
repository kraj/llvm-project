//===-- CommandObjectProcessScripted.cpp ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CommandObjectProcessScripted.h"

#include "lldb/Host/OptionParser.h"
#include "lldb/Interpreter/CommandObject.h"
#include "lldb/Interpreter/CommandReturnObject.h"
#include "lldb/Interpreter/OptionGroupPythonClassWithDict.h"

using namespace lldb;
using namespace lldb_private;

// CommandObjectProcessScriptedLoad
#pragma mark CommandObjectProcessScriptedLoad
#define LLDB_OPTIONS_process_scripted_load
#include "CommandOptions.inc"

class CommandObjectProcessScriptedLoad : public CommandObjectParsed {
private:
  class CommandOptions : public OptionGroup {
  public:
    CommandOptions() : OptionGroup() {}
    ~CommandOptions() override = default;

    Status SetOptionValue(uint32_t option_idx, llvm::StringRef option_arg,
                          ExecutionContext *execution_context) override {
      Status error;
      const int short_option =
          g_process_scripted_load_options[option_idx].short_option;

      switch (short_option) {
      case 'S':
        m_module = std::string(option_arg);
        break;
      default:
        llvm_unreachable("Unimplemented option");
      }

      return error;
    }

    void OptionParsingStarting(ExecutionContext *execution_context) override {
      m_module = "";
    }

    llvm::ArrayRef<OptionDefinition> GetDefinitions() override {
      return llvm::makeArrayRef(g_process_scripted_load_options);
    }

    std::string m_module;
  };

  CommandOptions m_options;
  OptionGroupPythonClassWithDict m_class_options;
  OptionGroupOptions m_all_options;

  Options *GetOptions() override { return &m_all_options; }

protected:
  bool DoExecute(Args &command, CommandReturnObject &result) override {
#if LLDB_ENABLE_PYTHON
    if (m_class_options.GetName().empty()) {
      result.AppendErrorWithFormat(
          "%s needs a Python class name (-l argument).\n", m_cmd_name.c_str());
      result.SetStatus(eReturnStatusFailed);
      return false;
    }

    if (m_options.m_module.empty()) {
      result.AppendErrorWithFormat("%s needs a module name (-s argument).\n",
                                   m_cmd_name.c_str());
      result.SetStatus(eReturnStatusFailed);
      return false;
    }

    ScriptInterpreter *interpreter = GetDebugger().GetScriptInterpreter();

    if (interpreter &&
        !interpreter->CheckObjectExists(m_class_options.GetName().c_str())) {
      result.AppendWarning(
          "The provided class does not exist - please define it "
          "before attempting to use this frame recognizer");
    }

    Target &target = GetSelectedOrDummyTarget();
    if (target.IsValid()) {
      FileSpec script_spec(m_options.m_module);
      ScriptedProcessLaunchInfo launch_info =
          (m_class_options.IsClass())
              ? ScriptedProcessLaunchInfo(script_spec,
                                          m_class_options.GetName())
              : ScriptedProcessLaunchInfo(m_class_options.GetStructuredData());
      target.SetScriptedProcessLaunchInfo(launch_info);
    }
#endif
    result.SetStatus(eReturnStatusSuccessFinishNoResult);
    return result.Succeeded();
  }

public:
  CommandObjectProcessScriptedLoad(CommandInterpreter &interpreter)
      : CommandObjectParsed(
            interpreter, "process scripted load",
            "Load a scripted process.\n"
            "You can either specify a script file and the implementation class "
            "or you can specify a dictionary of key (-k) and value (-v) pairs "
            "that will be used to populate an SBStructuredData Dictionary, "
            "which "
            "will be passed to the constructor of the class implementing the "
            "scripted step.  See the Python Reference for more details.",
            nullptr),
        m_options(), m_class_options("process scripted load") {
    m_all_options.Append(&m_class_options, LLDB_OPT_SET_1 | LLDB_OPT_SET_2,
                         LLDB_OPT_SET_ALL);
    m_all_options.Append(&m_options, LLDB_OPT_SET_3 | LLDB_OPT_SET_4,
                         LLDB_OPT_SET_ALL);
    m_all_options.Finalize();
    /*    FIXME: Update Long Help
    //    SetHelpLong(R"(
    //                Frame recognizers allow for retrieving information about
    //                special frames based on ABI, arguments or other special
    //                properties of that frame, even without source code or
    //                debug info. Currently, one use case is to extract function
    //                arguments that would otherwise be unaccesible, or augment
    //                existing arguments.
    //
    //                Adding a custom frame recognizer is possible by
    //                implementing a Python class and using the 'frame
    //                recognizer add' command. The Python class should have a
    //                'get_recognized_arguments' method and it will receive an
    //                argument of type lldb.SBFrame representing the current
    //                frame that we are trying to recognize. The method should
    //                return a (possibly empty) list of lldb.SBValue objects
    //                that represent the recognized arguments.
    //
    //                An example of a recognizer that retrieves the file
    //                descriptor values from libc functions 'read', 'write' and
    //                'close' follows:
    //
    //                class LibcFdRecognizer(object):
    //                def get_recognized_arguments(self, frame):
    //                if frame.name in ["read", "write", "close"]:
    //                fd = frame.EvaluateExpression("$arg1").unsigned
    //                value = lldb.target.CreateValueFromExpression("fd",
    //                "(int)%d" % fd) return [value] return []
    //
    //                The file containing this implementation can be imported
    //                via 'command script import' and then we can register this
    //                recognizer with 'frame recognizer add'. It's important to
    //                restrict the recognizer to the libc library (which is
    //                libsystem_kernel.dylib on macOS) to avoid matching
    //                functions with the same name in other modules:
    //
    //                (lldb) command script import .../fd_recognizer.py
    //                (lldb) frame recognizer add -l
    //                fd_recognizer.LibcFdRecognizer -n read -s
    //                libsystem_kernel.dylib
    //
    //                When the program is stopped at the beginning of the 'read'
    //                function in libc, we can view the recognizer arguments in
    //                'frame variable':
    //
    //                (lldb) b read
    //                (lldb) r
    //                Process 1234 stopped
    //                * thread #1, queue = 'com.apple.main-thread', stop reason
    //                = breakpoint 1.3 frame #0: 0x00007fff06013ca0
    //                libsystem_kernel.dylib`read (lldb) frame variable (int) fd
    //                = 3
    //
    //                )");
    */
  }
  ~CommandObjectProcessScriptedLoad() override = default;
};

CommandObjectProcessScripted::CommandObjectProcessScripted(
    CommandInterpreter &interpreter)
    : CommandObjectMultiword(
          interpreter, "process scripted",
          "Commands for operating on scripted processes.",
          "process plugin <subcommand> [<subcommand-options>]") {
  LoadSubCommand("load", CommandObjectSP(new CommandObjectProcessScriptedLoad(
                             interpreter)));
  // TODO: Implement CommandObjectProcessPluginScriptedGenerate
  //  LoadSubCommand(
  //      "generate",
  //      CommandObjectSP(new CommandObjectProcessScriptedLoad(interpreter)));
}
