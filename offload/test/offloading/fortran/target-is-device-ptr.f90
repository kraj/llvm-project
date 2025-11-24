! Validate that a device pointer allocated via OpenMP runtime APIs can be
! consumed by a TARGET region using the is_device_ptr clause.
! REQUIRES: flang, amdgcn-amd-amdhsa
! UNSUPPORTED: nvptx64-nvidia-cuda
! UNSUPPORTED: nvptx64-nvidia-cuda-LTO
! UNSUPPORTED: aarch64-unknown-linux-gnu
! UNSUPPORTED: aarch64-unknown-linux-gnu-LTO
! UNSUPPORTED: x86_64-unknown-linux-gnu
! UNSUPPORTED: x86_64-unknown-linux-gnu-LTO

! RUN: %libomptarget-compile-fortran-run-and-check-generic

program is_device_ptr_target
  use omp_lib
  use iso_c_binding
  implicit none

  integer, parameter :: n = 4
  integer, target :: host(n)
  type(c_ptr) :: device_ptr
  integer(c_int) :: rc
  integer :: i

  do i = 1, n
    host(i) = i
  end do

  device_ptr = omp_target_alloc(int(n, c_size_t) * int(c_sizeof(host(1)), c_size_t), &
                                omp_get_default_device())
  if (.not. c_associated(device_ptr)) then
    print *, "device alloc failed"
    stop 1
  end if

  rc = omp_target_memcpy(device_ptr, c_loc(host), &
                         int(n, c_size_t) * int(c_sizeof(host(1)), c_size_t), &
                         0_c_size_t, 0_c_size_t, &
                         omp_get_default_device(), omp_get_initial_device())
  if (rc .ne. 0) then
    print *, "host->device memcpy failed"
    call omp_target_free(device_ptr, omp_get_default_device())
    stop 1
  end if

  call fill_on_device(device_ptr)

  rc = omp_target_memcpy(c_loc(host), device_ptr, &
                         int(n, c_size_t) * int(c_sizeof(host(1)), c_size_t), &
                         0_c_size_t, 0_c_size_t, &
                         omp_get_initial_device(), omp_get_default_device())
  call omp_target_free(device_ptr, omp_get_default_device())

  if (rc .ne. 0) then
    print *, "device->host memcpy failed"
    stop 1
  end if

  if (all(host == [2, 4, 6, 8])) then
    print *, "PASS"
  else
    print *, "FAIL", host
  end if

contains
  subroutine fill_on_device(ptr)
    type(c_ptr) :: ptr
    integer, pointer :: p(:)
    call c_f_pointer(ptr, p, [n])

    !$omp target is_device_ptr(ptr)
      p(1) = 2
      p(2) = 4
      p(3) = 6
      p(4) = 8
    !$omp end target
  end subroutine fill_on_device
end program is_device_ptr_target

!CHECK: PASS
