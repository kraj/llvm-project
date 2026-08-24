!===----------------------------------------------------------------------===!
! This directory can be used to add Integration tests involving multiple
! stages of the compiler (for eg. from Fortran to LLVM IR). It should not
! contain executable tests. We should only add tests here sparingly and only
! if there is no other way to test. Repeat this message in each test that is
! added to this directory and sub-directories.
!===----------------------------------------------------------------------===!

! Verify that host OpenMP callback analysis makes Flang's aggregate capture
! container noalias, allowing its fields to be loaded before an opaque call.
!
! RUN: %flang_fc1 -O2 -fopenmp -emit-llvm %s -o - | FileCheck %s

subroutine capture_lifetime(input, enabled)
  integer(8), intent(in) :: input
  logical(1), intent(in) :: enabled(64)
  integer(8) :: captured
  integer :: i

  interface
    subroutine opaque()
    end subroutine
    subroutine use_value(value)
      integer(8), value :: value
    end subroutine
  end interface

  captured = input
  !$omp parallel shared(captured, enabled) private(i)
  call opaque()
  do i = 1, 64
    if (enabled(i)) call use_value(captured)
  end do
  !$omp end parallel
end subroutine

! CHECK-LABEL: define internal void @capture_lifetime_..omp_par(
! CHECK-SAME: ptr noalias readonly captures(none) [[CAPTURES:%.*]])
! CHECK: omp.par.entry:
! CHECK-NEXT: [[ENABLED:%.*]] = load ptr, ptr [[CAPTURES]], align 8
! CHECK-NEXT: [[CAPTURE_FIELD:%.*]] = getelementptr i8, ptr [[CAPTURES]], i64 8
! CHECK-NEXT: [[CAPTURED:%.*]] = load ptr, ptr [[CAPTURE_FIELD]], align 8
! CHECK-NEXT: tail call void @opaque_()
! CHECK: omp.par.region3:
! CHECK-NOT: load ptr, ptr [[CAPTURE_FIELD]]
! CHECK: omp.par.region4:
! CHECK-NOT: load ptr, ptr [[CAPTURE_FIELD]]
! CHECK: load i64, ptr [[CAPTURED]], align 8
