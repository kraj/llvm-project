; RUN: llc -verify-machineinstrs -O2 -mtriple=spirv64-unknown-unknown %s -o - | FileCheck %s --check-prefixes=CHECK,TYPED
; RUN: %if spirv-tools %{ llc -O2 -mtriple=spirv64-unknown-unknown %s -o - -filetype=obj | spirv-val %}

; RUN: llc -verify-machineinstrs -O2 -mtriple=spirv32-unknown-unknown %s -o - | FileCheck %s --check-prefixes=CHECK,TYPED
; RUN: %if spirv-tools %{ llc -O2 -mtriple=spirv32-unknown-unknown %s -o - -filetype=obj | spirv-val %}

; RUN: llc -verify-machineinstrs -O2 -mtriple=spirv64-unknown-unknown --spirv-ext=+SPV_KHR_untyped_pointers %s -o - | FileCheck %s --check-prefixes=CHECK,UNTYPED
; RUN: %if spirv-tools %{ llc -O2 -mtriple=spirv64-unknown-unknown --spirv-ext=+SPV_KHR_untyped_pointers %s -o - -filetype=obj | spirv-val %}

; A constant byte offset from an address space cast of a global becomes
; G_ADDRSPACE_CAST followed by G_PTR_ADD. Both stay constant expressions in
; global initializers. The cast yields an i8 pointer, so the byte offset is
; the Element operand of the access chain, without a leading zero index.

; CHECK-DAG: %[[#I8:]] = OpTypeInt 8 0
; TYPED-DAG: %[[#GEN:]] = OpTypePointer Generic %[[#I8]]
; TYPED-DAG: %[[#CROSS:]] = OpTypePointer CrossWorkgroup %[[#I8]]
; UNTYPED-DAG: %[[#GEN:]] = OpTypeUntypedPointerKHR Generic
; UNTYPED-DAG: %[[#CROSS:]] = OpTypeUntypedPointerKHR CrossWorkgroup
; CHECK-DAG: %[[#C8:]] = OpConstant %[[#OFF:]] 8{{$}}
; CHECK-DAG: %[[#C4:]] = OpConstant %[[#OFF]] 4{{$}}
; CHECK-DAG: %[[#C16:]] = OpConstant %[[#OFF]] 16{{$}}
; CHECK-DAG: %[[#C72:]] = OpConstant %[[#OFF]] 72{{$}}

; One cast of a struct global shared by a direct initializer and two offsets.
; CHECK-DAG: %[[#OBJ:]] = {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#]]
; CHECK-DAG: %[[#OBJ_CAST:]] = OpSpecConstantOp %[[#GEN]] PtrCastToGeneric %[[#OBJ]]
; CHECK-DAG: {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#OBJ_CAST]]
; TYPED-DAG: %[[#OBJ_Y:]] = OpSpecConstantOp %[[#GEN]] InBoundsPtrAccessChain %[[#OBJ_CAST]] %[[#C8]]{{$}}
; TYPED-DAG: %[[#OBJ_N:]] = OpSpecConstantOp %[[#GEN]] InBoundsPtrAccessChain %[[#OBJ_CAST]] %[[#C16]]{{$}}
; UNTYPED-DAG: %[[#OBJ_Y:]] = OpSpecConstantOp %[[#GEN]] UntypedInBoundsPtrAccessChainKHR %[[#I8]] %[[#OBJ_CAST]] %[[#C8]]{{$}}
; UNTYPED-DAG: %[[#OBJ_N:]] = OpSpecConstantOp %[[#GEN]] UntypedInBoundsPtrAccessChainKHR %[[#I8]] %[[#OBJ_CAST]] %[[#C16]]{{$}}
; CHECK-DAG: {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#OBJ_Y]]
; CHECK-DAG: {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#OBJ_N]]

; Workgroup to CrossWorkgroup goes through Generic.
; CHECK-DAG: %[[#LDS:]] = {{OpVariable|OpUntypedVariableKHR}} %[[#]] Workgroup
; CHECK-DAG: %[[#LDS_GEN:]] = OpSpecConstantOp %[[#]] PtrCastToGeneric %[[#LDS]]
; CHECK-DAG: %[[#LDS_CROSS:]] = OpSpecConstantOp %[[#CROSS]] GenericCastToPtr %[[#LDS_GEN]]
; TYPED-DAG: %[[#LDS_OFF:]] = OpSpecConstantOp %[[#CROSS]] InBoundsPtrAccessChain %[[#LDS_CROSS]] %[[#C4]]{{$}}
; UNTYPED-DAG: %[[#LDS_OFF:]] = OpSpecConstantOp %[[#CROSS]] UntypedInBoundsPtrAccessChainKHR %[[#I8]] %[[#LDS_CROSS]] %[[#C4]]{{$}}
; CHECK-DAG: {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#LDS_OFF]]

; A null base is a pointer, also inside an aggregate initializer.
; CHECK-DAG: %[[#NULL:]] = OpConstantNull %[[#GEN]]
; TYPED-DAG: %[[#NULL_OFF:]] = OpSpecConstantOp %[[#GEN]] InBoundsPtrAccessChain %[[#NULL]] %[[#C72]]{{$}}
; UNTYPED-DAG: %[[#NULL_OFF:]] = OpSpecConstantOp %[[#GEN]] UntypedInBoundsPtrAccessChainKHR %[[#I8]] %[[#NULL]] %[[#C72]]{{$}}
; CHECK-DAG: {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#NULL_OFF]]
; CHECK-DAG: %[[#ARR:]] = {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#]]
; CHECK-DAG: %[[#ARR_CAST:]] = OpSpecConstantOp %[[#GEN]] PtrCastToGeneric %[[#ARR]]
; TYPED-DAG: %[[#ARR_OFF:]] = OpSpecConstantOp %[[#GEN]] InBoundsPtrAccessChain %[[#ARR_CAST]] %[[#C8]]{{$}}
; TYPED-DAG: %[[#NULL_OFF8:]] = OpSpecConstantOp %[[#GEN]] InBoundsPtrAccessChain %[[#NULL]] %[[#C8]]{{$}}
; UNTYPED-DAG: %[[#ARR_OFF:]] = OpSpecConstantOp %[[#GEN]] UntypedInBoundsPtrAccessChainKHR %[[#I8]] %[[#ARR_CAST]] %[[#C8]]{{$}}
; UNTYPED-DAG: %[[#NULL_OFF8:]] = OpSpecConstantOp %[[#GEN]] UntypedInBoundsPtrAccessChainKHR %[[#I8]] %[[#NULL]] %[[#C8]]{{$}}
; CHECK-DAG: %[[#ARR_OFF_W:]] = OpSpecConstantOp %[[#GEN]] Bitcast %[[#ARR_OFF]]
; CHECK-DAG: %[[#NULL_OFF8_W:]] = OpSpecConstantOp %[[#GEN]] Bitcast %[[#NULL_OFF8]]
; CHECK-DAG: %[[#TABLE:]] = OpSpecConstantComposite %[[#]] %[[#ARR_OFF_W]] %[[#ARR_CAST]] %[[#NULL_OFF8_W]]
; CHECK-DAG: {{OpVariable|OpUntypedVariableKHR}} %[[#]] CrossWorkgroup {{(%[0-9]+ )?}}%[[#TABLE]]

; CHECK: OpFunction
; CHECK-NOT: PtrCastToGeneric
; CHECK-NOT: AccessChain
; CHECK: OpFunctionEnd

%struct.test = type { i64, [2 x i64], i64 }

@obj = addrspace(1) global %struct.test zeroinitializer, align 8
@obj_ptr = addrspace(1) global ptr addrspace(4) addrspacecast (ptr addrspace(1) @obj to ptr addrspace(4)), align 8
@obj_y = addrspace(1) global ptr addrspace(4) getelementptr (i8, ptr addrspace(4) addrspacecast (ptr addrspace(1) @obj to ptr addrspace(4)), i64 8), align 8
@obj_n = addrspace(1) global ptr addrspace(4) getelementptr (i8, ptr addrspace(4) addrspacecast (ptr addrspace(1) @obj to ptr addrspace(4)), i64 16), align 8

@lds = addrspace(3) global [16 x i8] poison, align 1
@lds_off = addrspace(1) global ptr addrspace(1) getelementptr (i8, ptr addrspace(1) addrspacecast (ptr addrspace(3) @lds to ptr addrspace(1)), i64 4), align 8

@null_off = addrspace(1) global ptr addrspace(4) getelementptr (i8, ptr addrspace(4) addrspacecast (ptr addrspace(1) null to ptr addrspace(4)), i64 72), align 8

@arr = addrspace(1) global [32 x i8] zeroinitializer, align 1
@table = addrspace(1) global [3 x ptr addrspace(4)] [ptr addrspace(4) getelementptr (i8, ptr addrspace(4) addrspacecast (ptr addrspace(1) @arr to ptr addrspace(4)), i64 8), ptr addrspace(4) addrspacecast (ptr addrspace(1) @arr to ptr addrspace(4)), ptr addrspace(4) getelementptr (i8, ptr addrspace(4) addrspacecast (ptr addrspace(1) null to ptr addrspace(4)), i64 8)], align 8

define spir_func void @f() addrspace(4) {
  unreachable
}
