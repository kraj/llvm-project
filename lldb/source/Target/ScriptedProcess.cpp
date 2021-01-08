//===-- ScriptedProcess.cpp -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===--------------------------------------------------------------------===//

#include "lldb/Core/Debugger.h"
#include "lldb/Core/Module.h"

//#include "lldb/Core/ModuleSpec.h"

#include "lldb/Core/PluginManager.h"

//#include "lldb/Core/Section.h"

#include "lldb/Host/OptionParser.h"

#include "lldb/Interpreter/OptionArgParser.h"
#include "lldb/Interpreter/OptionGroupBoolean.h"
#include "lldb/Interpreter/ScriptInterpreter.h"

#include "lldb/Target/MemoryRegionInfo.h"
#include "lldb/Target/ScriptedProcess.h"
//#include "lldb/Target/SectionLoadList.h"
//#include "lldb/Target/Target.h"
//#include "lldb/Target/UnixSignals.h"
//#include "lldb/Utility/LLDBAssert.h"
//#include "lldb/Utility/Log.h"
//#include "lldb/Utility/State.h"
//#include "llvm/BinaryFormat/Magic.h"
//#include "llvm/Support/MemoryBuffer.h"
//#include "llvm/Support/Threading.h"

//#include <memory>

using namespace lldb;
using namespace lldb_private;

lldb::ProcessSP ScriptedProcess::CreateInstance(lldb::TargetSP target_sp,
                                                lldb::ListenerSP listener_sp,
                                                const FileSpec *file,
                                                bool can_connect) {

  // TODO: Check FileSpec content ?
  if (!target_sp->IsScriptable()) {
    return nullptr;
  }

  const ScriptedProcessLaunchInfo &launch_info =
      *target_sp->GetScriptedProcessLaunchInfo();

  return std::make_shared<ScriptedProcess>(target_sp, listener_sp, launch_info);
}

bool ScriptedProcess::CanDebug(lldb::TargetSP target_sp,
                               bool plugin_specified_by_name) {
  return true;
}

ScriptedProcess::ScriptedProcess(lldb::TargetSP target_sp,
                                 lldb::ListenerSP listener_sp,
                                 const ScriptedProcessLaunchInfo &launch_info)
    : Process(target_sp, listener_sp), m_launch_info(launch_info),
      m_interpreter_sp(nullptr), m_python_object_sp(nullptr) {
  if (!target_sp)
    return;

  m_interpreter_sp.reset(target_sp->GetDebugger().GetScriptInterpreter());

  if (!m_interpreter_sp)
    return;

  const FileSpec &script_file = m_launch_info.GetScriptFileSpec();

  std::string scripted_process_plugin_class_name(
      script_file.GetFilename().AsCString(""));
  if (!scripted_process_plugin_class_name.empty()) {
    const std::string scripted_process_plugin_path = script_file.GetPath();
    const bool init_session = false;
    Status error;
    if (m_interpreter_sp->LoadScriptingModule(
            scripted_process_plugin_path.c_str(), init_session, error)) {
      // Strip the ".py" extension if there is one
      size_t py_extension_pos = scripted_process_plugin_class_name.rfind(".py");
      if (py_extension_pos != std::string::npos)
        scripted_process_plugin_class_name.erase(py_extension_pos);
      // Add ".OperatingSystemPlugIn" to the module name to get a string like
      // "modulename.OperatingSystemPlugIn"
      scripted_process_plugin_class_name += ".OperatingSystemPlugIn";

      StructuredData::ObjectSP object_sp =
          m_interpreter_sp->ScriptedProcess_Create(NULL, 0,
                                                   LLDB_INVALID_ADDRESS);
      //      m_interpreter_sp->OSPlugin_CreatePluginObject(scripted_process_plugin_class_name.c_str(),
      //      process->CalculateProcess());
      if (object_sp && object_sp->IsValid())
        m_python_object_sp = object_sp;
    }
  }
}

ScriptedProcess::~ScriptedProcess() {
  Clear();
  // We need to call finalize on the process before destroying ourselves to
  // make sure all of the broadcaster cleanup goes as planned. If we destruct
  // this class, then Process::~Process() might have problems trying to fully
  // destroy the broadcaster.
  Finalize();
}

void ScriptedProcess::Initialize() {
  static llvm::once_flag g_once_flag;

  llvm::call_once(g_once_flag, []() {
    PluginManager::RegisterPlugin(GetPluginNameStatic(),
                                  GetPluginDescriptionStatic(), CreateInstance);
  });
}

void ScriptedProcess::Terminate() {
  PluginManager::UnregisterPlugin(ScriptedProcess::CreateInstance);
}

ConstString ScriptedProcess::GetPluginName() { return GetPluginNameStatic(); }

uint32_t ScriptedProcess::GetPluginVersion() { return 1; }

Status ScriptedProcess::DoDestroy() { return Status(); }

void ScriptedProcess::RefreshStateAfterStop() {
  //
  //  if (!m_active_exception)
  //    return;
  //
  //  constexpr uint32_t BreakpadDumpRequested = 0xFFFFFFFF;
  //  if (m_active_exception->ExceptionRecord.ExceptionCode ==
  //      BreakpadDumpRequested) {
  //      // This "ExceptionCode" value is a sentinel that is sometimes used
  //      // when generating a dump for a process that hasn't crashed.
  //
  //      // TODO: The definition and use of this "dump requested" constant
  //      // in Breakpad are actually Linux-specific, and for similar use
  //      // cases on Mac/Windows it defines different constants, referring
  //      // to them as "simulated" exceptions; consider moving this check
  //      // down to the OS-specific paths and checking each OS for its own
  //      // constant.
  //    return;
  //  }
  //
  //  lldb::StopInfoSP stop_info;
  //  lldb::ThreadSP stop_thread;
  //
  //  Process::m_thread_list.SetSelectedThreadByID(m_active_exception->ThreadId);
  //  stop_thread = Process::m_thread_list.GetSelectedThread();
  //  ArchSpec arch = GetArchitecture();
  //
  //  if (arch.GetTriple().getOS() == llvm::Triple::Linux) {
  //    uint32_t signo = m_active_exception->ExceptionRecord.ExceptionCode;
  //
  //    if (signo == 0) {
  //        // No stop.
  //      return;
  //    }
  //
  //    stop_info = StopInfo::CreateStopReasonWithSignal(
  //                                                     *stop_thread, signo);
  //  } else if (arch.GetTriple().getVendor() == llvm::Triple::Apple) {
  //    stop_info = StopInfoMachException::CreateStopReasonWithMachException(
  //                                                                         *stop_thread,
  //                                                                         m_active_exception->ExceptionRecord.ExceptionCode,
  //                                                                         2,
  //                                                                         m_active_exception->ExceptionRecord.ExceptionFlags,
  //                                                                         m_active_exception->ExceptionRecord.ExceptionAddress,
  //                                                                         0);
  //  } else {
  //    std::string desc;
  //    llvm::raw_string_ostream desc_stream(desc);
  //    desc_stream << "Exception "
  //    << llvm::format_hex(
  //                        m_active_exception->ExceptionRecord.ExceptionCode,
  //                        8)
  //    << " encountered at address "
  //    << llvm::format_hex(
  //                        m_active_exception->ExceptionRecord.ExceptionAddress,
  //                        8);
  //    stop_info = StopInfo::CreateStopReasonWithException(
  //                                                        *stop_thread,
  //                                                        desc_stream.str().c_str());
  //  }
  //
  //  stop_thread->SetStopInfo(stop_info);
}

bool ScriptedProcess::IsAlive() { return true; }

bool ScriptedProcess::WarnBeforeDetach() const { return false; }

size_t ScriptedProcess::ReadMemory(lldb::addr_t addr, void *buf, size_t size,
                                   Status &error) {
  // Don't allow the caching that lldb_private::Process::ReadMemory does since
  // we have it all cached in our dump file anyway.

  return DoReadMemory(addr, buf, size, error);
}

size_t ScriptedProcess::DoReadMemory(lldb::addr_t addr, void *buf, size_t size,
                                     Status &error) {

  m_interpreter_sp->ScriptedProcess_ReadMemoryAtAddress(m_python_object_sp,
                                                        addr, size);
  return size;
}

ArchSpec ScriptedProcess::GetArchitecture() {
  llvm::Triple triple;
  //  triple.setVendor(llvm::Triple::VendorType::UnknownVendor);
  //  triple.setArch(llvm::Triple::ArchType::x86);
  //  triple.setOS(llvm::Triple::OSType::Win32);
  return ArchSpec(triple);
}

Status ScriptedProcess::GetMemoryRegionInfo(lldb::addr_t load_addr,
                                            MemoryRegionInfo &region) {
  //  BuildMemoryRegions();
  //  region = MinidumpParser::GetMemoryRegionInfo(*m_memory_regions,
  //  load_addr);
  return Status();
}

Status ScriptedProcess::GetMemoryRegions(MemoryRegionInfos &region_list) {
  Status error;
  auto res =
      m_interpreter_sp->ScriptedProcess_GetNumMemoryRegions(m_python_object_sp);

  if (!res) {
    error.SetErrorString(
        "ScriptedProcess: Couldn't get number of memory regions!");
    return error;
  }

  uint64_t size = res->GetAsInteger()->GetValue();

  if (size == UINT64_MAX) {
    error.SetErrorString("ScriptedProcess: Invalid number of memory region!");
    return error;
  }

  for (uint64_t i = 0; i < size; i++) {
    // FIXME: Update interface method to handle extra arg (index)
    MemoryRegionInfoSP mem_region_sp =
        m_interpreter_sp->ScriptedProcess_GetMemoryRegionAtIndex(
            m_python_object_sp, i);

    if (!mem_region_sp) {
      // FIXME: Interpolate index in error string
      error.SetErrorString(
          "ScriptedProcess: Couldn't fetch memory region at index BLA");
      return error;
    }
    region_list.push_back(*mem_region_sp.get());
  }

  return error;
}

void ScriptedProcess::Clear() { Process::m_thread_list.Clear(); }

bool ScriptedProcess::UpdateThreadList(ThreadList &old_thread_list,
                                       ThreadList &new_thread_list) {
  return new_thread_list.GetSize(false) > 0;
}

bool ScriptedProcess::GetProcessInfo(ProcessInstanceInfo &info) {
  info.Clear();
  info.SetProcessID(GetID());
  info.SetArchitecture(GetArchitecture());
  lldb::ModuleSP module_sp = GetTarget().GetExecutableModule();
  if (module_sp) {
    const bool add_exe_file_as_first_arg = false;
    info.SetExecutableFile(GetTarget().GetExecutableModule()->GetFileSpec(),
                           add_exe_file_as_first_arg);
  }
  return true;
}
