#!/usr/bin/env python3

from lldb import ScriptedProcess

class ScriptedMachCoreProcess(ScriptedProcess):
    def __init__(self, target: lldb.SBTarget):
        self.memory_regions = []
        self.threads = []
        self.loaded_images = []
        self.stops = []
        self.target = target

    def __init__(self, dictionary: lldb.SBStructuredData):
        # User-defined
        pass

    ### Main functionalities
    def get_num_memory_regions(self) -> int:
        return len(self.memory_region)
    def get_memory_region_at_index(self, idx: int) -> lldb.SBMemoryRegionInfos:
        return self.memory_region[idx]
    def get_num_threads(self) -> int:
        return len(self.threads)
    def get_thread_at_index(self, idx: int) -> lldb.SBThread:
        return self.threads[idx]
    def get_register_for_thread(self, tid: int):
        # Follow register data structure used in OS Plugins.
        return reg
    def read_memory_at_address(self, addr: int) -> lldb.SBData:
        # dict[addr:data] ?
        return addr
    def get_loaded_images(self) -> list[str]:
        return self.loaded_images

    ### Process state
    def can_debug(self) -> bool:
        return True
    def is_alive(self) -> bool:
        return True

example = ScriptedMachCoreProcess("lol")
print(example.sb_target)
print(example.get_num_threads())
