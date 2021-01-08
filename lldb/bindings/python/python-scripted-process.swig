%pythoncode %{
from abc import ABC, abstractmethod
from typing import List

import lldb

class ScriptedProcess(ABC):
    @abstractmethod
    def __init__(self):
        pass

    # Optional initializer
    def __init__(self, dictionary: lldb.SBStructuredData):
        pass

    ### Main funcitonnalities
    @abstractmethod
    def get_num_memory_regions(self) -> int:
        pass

    @abstractmethod
    def get_memory_region_at_index(self, idx: int) -> lldb.SBMemoryRegionInfo:
        pass

    @abstractmethod
    def get_num_threads(self):
        pass

    @abstractmethod
    def get_thread_at_index(self, idx: int) -> lldb.SBThread:
        pass

    @abstractmethod
    def get_register_for_thread(self, tid:int):
        pass

    @abstractmethod
    def read_memory_at_address(self, addr:int) -> lldb.SBData:
        pass

    @abstractmethod
    def get_loaded_images(self) -> List[str]: # -> List[lldb.SBModule]:
        pass

    ### Process state
    @abstractmethod
    def can_debug(self) -> bool:
        pass

    @abstractmethod
    def is_alive(self) -> bool:
        pass
%}
