! Test lowering of OpenMP metadirective with loop-associated variants.

! RUN: %flang_fc1 -fopenmp -emit-hlfir -fopenmp-version=51 %s -o - | FileCheck %s

!===----------------------------------------------------------------------===!
! Basic loop-associated variants via static selection
!===----------------------------------------------------------------------===!

! CHECK-LABEL: func.func @_QPtest_parallel_do()
! CHECK:         omp.parallel {
! CHECK:           omp.wsloop
! CHECK:             omp.loop_nest
! CHECK:             omp.yield
! CHECK:           omp.terminator
! CHECK:         return
subroutine test_parallel_do()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: parallel do) &
  !$omp & default(nothing)
  do i = 1, 100
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_do()
! CHECK-NOT:     omp.parallel
! CHECK:         omp.wsloop
! CHECK:           omp.loop_nest
! CHECK:           omp.yield
! CHECK:         return
subroutine test_do()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: do) &
  !$omp & default(nothing)
  do i = 1, 100
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_simd()
! CHECK-NOT:     omp.wsloop
! CHECK:         omp.simd
! CHECK:           omp.loop_nest
! CHECK:           omp.yield
! CHECK:         return
subroutine test_simd()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: simd) &
  !$omp & default(nothing)
  do i = 1, 100
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_do_simd()
! CHECK-NOT:     omp.parallel
! CHECK:         omp.wsloop
! CHECK:           omp.simd
! CHECK:             omp.loop_nest
! CHECK:             omp.yield
! CHECK:         return
subroutine test_do_simd()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: do simd) &
  !$omp & default(nothing)
  do i = 1, 100
  end do
end subroutine

!===----------------------------------------------------------------------===!
! Static mismatch falls through to standalone fallback
!===----------------------------------------------------------------------===!

! CHECK-LABEL: func.func @_QPtest_loop_static_mismatch()
! CHECK-NOT:     omp.wsloop
! CHECK-NOT:     omp.loop_nest
! CHECK:         omp.barrier
! CHECK:         return
subroutine test_loop_static_mismatch()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor("unknown")}: parallel do) &
  !$omp & default(barrier)
  do i = 1, 100
  end do
end subroutine

!===----------------------------------------------------------------------===!
! Dynamic user condition with loop-associated variant
!===----------------------------------------------------------------------===!

! CHECK-LABEL: func.func @_QPtest_dynamic_loop(
! CHECK-SAME:    %[[ARG0:.*]]: !fir.ref<!fir.logical<4>>
! CHECK:         %[[DECL:.*]]:2 = hlfir.declare %[[ARG0]]
! CHECK:         %[[LOAD:.*]] = fir.load %[[DECL]]#0
! CHECK:         %[[COND:.*]] = fir.convert %[[LOAD]] : (!fir.logical<4>) -> i1
! CHECK:         fir.if %[[COND]] {
! CHECK:           omp.parallel {
! CHECK:             omp.wsloop
! CHECK:               omp.loop_nest
! CHECK:         } else {
! CHECK:           omp.simd
! CHECK:             omp.loop_nest
! CHECK:         }
! CHECK:         return
subroutine test_dynamic_loop(flag)
  logical, intent(in) :: flag
  integer :: i
  !$omp metadirective &
  !$omp & when(user={condition(flag)}: parallel do) &
  !$omp & default(simd)
  do i = 1, 100
  end do
end subroutine

!===----------------------------------------------------------------------===!
! Loop-associated variants with clauses
!===----------------------------------------------------------------------===!

! CHECK-LABEL: func.func @_QPtest_schedule()
! CHECK:         omp.wsloop schedule(static)
! CHECK:           omp.loop_nest
! CHECK:         return
subroutine test_schedule()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: do schedule(static)) &
  !$omp & default(nothing)
  do i = 1, 100
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_reduction()
! CHECK:         omp.wsloop {{.*}} reduction(@add_reduction_i32
! CHECK:           omp.loop_nest
! CHECK:         return
subroutine test_reduction()
  integer :: i, s
  s = 0
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: do reduction(+:s)) &
  !$omp & default(nothing)
  do i = 1, 100
    s = s + i
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_private()
! CHECK:         omp.wsloop private(
! CHECK:           omp.loop_nest
! CHECK:         return
subroutine test_private()
  integer :: i, x
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: do private(x)) &
  !$omp & default(nothing)
  do i = 1, 100
    x = i
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_collapse()
! CHECK:         omp.wsloop
! CHECK:           omp.loop_nest ({{.*}}, {{.*}}) : i32 {{.*}} collapse(2)
! CHECK:         return
subroutine test_collapse()
  integer :: i, j
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: do collapse(2)) &
  !$omp & default(nothing)
  do i = 1, 100
    do j = 1, 100
    end do
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_safelen()
! CHECK:         omp.simd {{.*}} safelen(4)
! CHECK:           omp.loop_nest
! CHECK:         return
subroutine test_safelen()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: simd safelen(4)) &
  !$omp & default(nothing)
  do i = 1, 100
  end do
end subroutine

! CHECK-LABEL: func.func @_QPtest_num_threads()
! CHECK:         omp.parallel num_threads({{.*}}) {
! CHECK:           omp.wsloop
! CHECK:             omp.loop_nest
! CHECK:         return
subroutine test_num_threads()
  integer :: i
  !$omp metadirective &
  !$omp & when(implementation={vendor(llvm)}: parallel do num_threads(4)) &
  !$omp & default(nothing)
  do i = 1, 100
  end do
end subroutine
