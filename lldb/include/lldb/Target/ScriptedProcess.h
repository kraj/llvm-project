//===-- ScriptableProcess.cpp -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_SOURCE_PLUGINS_SCRIPTABLE_PROCESS_H
#define LLDB_SOURCE_PLUGINS_SCRIPTABLE_PROCESS_H

#include "lldb/Target/Process.h"
//#include "lldb/Target/StopInfo.h"
//#include "lldb/Target/Target.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/Status.h"
//
//#include "llvm/Support/Format.h"
//#include "llvm/Support/raw_ostream.h"

namespace lldb_private {

class ScriptedProcess : public Process {
public:
  static lldb::ProcessSP CreateInstance(lldb::TargetSP target_sp,
                                        lldb::ListenerSP listener_sp,
                                        const FileSpec *crash_file_path,
                                        bool can_connect);

  static void Initialize();

  static void Terminate();

  static ConstString GetPluginNameStatic();

  static const char *GetPluginDescriptionStatic();

  ScriptedProcess(lldb::TargetSP target_sp, lldb::ListenerSP listener_sp,
                  const ScriptedProcessLaunchInfo &launch_info);

  ~ScriptedProcess() override;

  bool CanDebug(lldb::TargetSP target_sp,
                bool plugin_specified_by_name) override;

  DynamicLoader *GetDynamicLoader() override { return nullptr; }

  ConstString GetPluginName() override;

  uint32_t GetPluginVersion() override;

  SystemRuntime *GetSystemRuntime() override { return nullptr; }

  Status DoDestroy() override;

  void RefreshStateAfterStop() override;

  bool IsAlive() override;

  bool WarnBeforeDetach() const override;

  size_t ReadMemory(lldb::addr_t addr, void *buf, size_t size,
                    Status &error) override;

  size_t DoReadMemory(lldb::addr_t addr, void *buf, size_t size,
                      Status &error) override;

  ArchSpec GetArchitecture();

  Status GetMemoryRegionInfo(lldb::addr_t load_addr,
                             MemoryRegionInfo &range_info) override;

  Status
  GetMemoryRegions(lldb_private::MemoryRegionInfos &region_list) override;

  bool GetProcessInfo(ProcessInstanceInfo &info) override;

  Status WillResume() override {
    Status error;
    error.SetErrorStringWithFormat(
        "error: %s does not support resuming processes",
        GetPluginName().GetCString());
    return error;
  }

protected:
  void Clear();

  bool UpdateThreadList(ThreadList &old_thread_list,
                        ThreadList &new_thread_list) override;

private:
  const ScriptedProcessLaunchInfo &m_launch_info;
  lldb::ScriptInterpreterSP m_interpreter_sp;
  lldb_private::StructuredData::ObjectSP m_python_object_sp;
  //   lldb::DataBufferSP m_core_data;
  //   llvm::ArrayRef<minidump::Thread> m_thread_list;
  //   const minidump::ExceptionStream *m_active_exception;
  //   bool m_is_wow64;
  //   llvm::Optional<MemoryRegionInfos> m_memory_regions;
};

} // namespace lldb_private

#endif // LLDB_SOURCE_PLUGINS_SCRIPTABLE_PROCESS_H
