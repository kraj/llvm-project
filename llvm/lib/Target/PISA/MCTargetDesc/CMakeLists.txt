add_llvm_component_library(LLVMPISADesc
  PISAEnum.cpp
  PISAMCAsmInfo.cpp
  PISAMCTargetDesc.cpp
  PISATargetStreamer.cpp
  PISAInstPrinter.cpp
  PISAMCExpr.cpp
  PISARegEncoder.cpp

  LINK_COMPONENTS
  CodeGenTypes
  Core
  MC
  PISAInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  PISA
  )

