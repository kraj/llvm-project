define void @f2(ptr %v) #0 {
entry:
  %v.addr = alloca ptr, align 8, addrspace(5)
  store ptr %v, ptr addrspace(5) %v.addr, align 8
  call void @f3(ptr %v)
  ret void
}

define weak hidden void @f3(ptr %v) #0 {
entry:
  store i32 12345, ptr %v
  ret void
}

attributes #0 = { noinline }
