//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// Implementation of POSIX realpath.
///
//===----------------------------------------------------------------------===//

#include "src/stdlib/realpath.h"
#include "src/string/memory_utils/inline_memcpy.h"
#include "src/string/memory_utils/inline_memmove.h"

#include "hdr/errno_macros.h"
#include "hdr/fcntl_macros.h"
#include "hdr/limits_macros.h"
#include "hdr/sys_stat_macros.h"
#include "hdr/types/mode_t.h"
#include "hdr/types/size_t.h"
#include "src/__support/CPP/optional.h"
#include "src/__support/CPP/string.h"
#include "src/__support/CPP/string_view.h"
#include "src/__support/CPP/utility.h"
#include "src/__support/OSUtil/linux/stat/kernel_statx_types.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/getcwd.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/readlink.h"
#include "src/__support/OSUtil/linux/syscall_wrappers/statx.h"
#include "src/__support/OSUtil/path.h"
#include "src/__support/common.h"
#include "src/__support/error_or.h"
#include "src/__support/libc_errno.h"
#include "src/__support/macros/config.h"
#include "src/stdlib/realpath.h"

namespace LIBC_NAMESPACE_DECL {
namespace {

#ifdef SYMLOOP_MAX
constexpr size_t MAX_SYMLINK_TRAVERSALS = SYMLOOP_MAX;
#else
constexpr size_t MAX_SYMLINK_TRAVERSALS = 40;
#endif

// Container for a fully resolved, canonical path.
//
// The contained path is always in its canonical form. It is:
// - Absolute
// - Symlink-free
// - Without a trailing separator
// - Devoid of path traversals like "." or ".."
class ResolvedPath {
public:
  ResolvedPath() { set_to_root(); }

  void set_to_root() { path_ = path::SEPARATOR; }

  cpp::optional<Error> set_to_cwd() {
    char buf[PATH_MAX];
    ErrorOr<ssize_t> ret = linux_syscalls::getcwd(buf, PATH_MAX);
    if (!ret) {
      if (ret.error() == ERANGE)
        return Error(ENAMETOOLONG);
      return Error(ret.error());
    }

    if (*ret <= 0)
      return Error(EIO);

    path_ = cpp::string_view(buf, *ret - 1);
    return cpp::nullopt;
  }

  // Removes the trailing path component.
  void set_to_parent() {
    size_t sep_index = cpp::string_view(path_).find_last_of(path::SEPARATOR);

    // Never move past the root separator. For example,
    // ensures that set_to_parent on "/hello" only resizes to "/".
    path_.resize(sep_index >= 1 ? sep_index : 1);
  }

  // Adds a single component to the end of this path.
  cpp::optional<Error> push_component(cpp::string_view component) {
    if (!path::is_root(path_)) {
      if (cpp::optional<Error> err = push_raw(path::SEPARATOR); err)
        return err;
    }

    return push_raw(component);
  }

  // Releases ownership of the underlying C-string and resets this path.
  //
  // Must be free'd by the caller.
  char *release() { return path_.release_c_str(); }

  const char *c_str() const { return path_.c_str(); }

  // Copies the content of this path to `dst`.
  void copy_to(char *dst) {
    inline_memcpy(dst, path_.c_str(), path_.size() + 1);
  }

private:
  cpp::optional<Error> push_raw(cpp::string_view value) {
    // -1 because PATH_MAX includes a null-terminator.
    size_t remaining_bytes = (PATH_MAX - 1) - path_.size();
    if (value.size() > remaining_bytes)
      return Error(ENAMETOOLONG);

    path_ += value;
    return cpp::nullopt;
  }

  cpp::optional<Error> push_raw(char c) {
    return push_raw(cpp::string_view(&c, 1));
  }

  cpp::string path_;
};

// A view over path components yet to be processed by realpath.
//
// When `realpath("./a/../b")` is called, the input path can be viewed as
// a queue of components, where components closest to the root are processed
// first. For example:
//
//   ```
//   PendingPath p("./a/..");
//   assert(p.advance_component() == ".");
//   assert(p.advance_component() == "a");
//   p.prepend_components("b/c");
//   assert(p.advance_component() == "b");
//   assert(p.advance_component() == "c");
//   assert(p.advance_component() == "..");
//   assert(p.empty());
//   ```
class PendingPath {
public:
  explicit PendingPath() { set_empty(); }

  // Whether all path components have been consumed.
  LIBC_INLINE bool empty() const { return view().empty(); }

  // Whether the pending path is absolute.
  LIBC_INLINE bool is_absolute() const { return path::is_absolute(view()); }

  // Takes the next path component,
  // starting with the component closest to the root.
  cpp::string_view advance_component() {
    const cpp::string_view path = view();

    const size_t component_start = path.find_first_not_of(path::SEPARATOR);
    if (component_start == cpp::string_view::npos) {
      set_empty();
      return "";
    }

    const size_t component_end =
        path.find_first_of(path::SEPARATOR, /* From = */ component_start);
    if (component_end == cpp::string_view::npos) {
      set_empty();
      return path.substr(component_start);
    }

    start_ += component_end;
    return path.substr(component_start, component_end - component_start);
  }

  LIBC_INLINE cpp::optional<Error> prepend(cpp::string_view src) {
    if (src.size() > start_)
      return Error(ENAMETOOLONG);

    start_ -= src.size();
    inline_memmove(path_ + start_, src.data(), src.size());
    return cpp::nullopt;
  }

  LIBC_INLINE cpp::optional<Error> prepend(char c) {
    return prepend(cpp::string_view(&c, 1));
  }

  LIBC_INLINE cpp::optional<Error> prepend_link(const char *link_path) {
    cpp::string_view curr = view();
    if (!curr.empty() && !curr.starts_with(path::SEPARATOR)) {
      if (cpp::optional<Error> err = prepend(path::SEPARATOR); err)
        return err;
    }

    ErrorOr<ssize_t> bytes_written =
        linux_syscalls::readlink(link_path, path_, start_);
    if (!bytes_written)
      return Error(bytes_written.error());

    if (*bytes_written <= 0)
      return Error(EIO); // Should not be possible, check to avoid underflow.

    cpp::string_view target(path_, static_cast<size_t>(*bytes_written));

    // Check if readlink ran out of space.
    if (target.size() == start_)
      return Error(ENAMETOOLONG);

    return prepend(target);
  }

private:
  LIBC_INLINE cpp::string_view view() const {
    return cpp::string_view(path_ + start_, PATH_MAX - start_);
  }

  LIBC_INLINE void set_empty() { start_ = PATH_MAX; }

  size_t start_;
  char path_[PATH_MAX];
};

ErrorOr<mode_t> read_file_type(const char *path) {
  internal::kernel_statx_buf buf;
  ErrorOr<int> ret =
      linux_syscalls::statx(AT_FDCWD, path, AT_SYMLINK_NOFOLLOW,
                            internal::KERNEL_STATX_TYPE_MASK, &buf);
  if (!ret)
    return Error(ret.error());

  if (!(buf.stx_mask & internal::KERNEL_STATX_TYPE_MASK))
    return Error(EIO);

  return static_cast<mode_t>(buf.stx_mode);
}

cpp::optional<Error> resolve_path(cpp::string_view path,
                                  ResolvedPath &resolved_path) {
  PendingPath pending_path;
  if (cpp::optional<Error> err = pending_path.prepend(path); err)
    return err;

  size_t symlinks_followed = 0;
  while (!pending_path.empty()) {
    cpp::string_view component = pending_path.advance_component();
    if (component.empty() || component == path::CURRENT_DIR_COMPONENT)
      continue;

    if (component == path::PARENT_DIR_COMPONENT) {
      resolved_path.set_to_parent();
      continue;
    }

    if (cpp::optional<Error> err = resolved_path.push_component(component); err)
      return err;

    ErrorOr<mode_t> mode = read_file_type(resolved_path.c_str());
    if (!mode)
      return Error(mode.error());

    if (S_ISLNK(*mode)) {
      if (symlinks_followed >= MAX_SYMLINK_TRAVERSALS)
        return Error(ELOOP);
      symlinks_followed += 1;

      cpp::optional<Error> err =
          pending_path.prepend_link(resolved_path.c_str());
      if (err)
        return err;
      resolved_path.set_to_parent();

      // If the symlink resolved to an absolute path,
      // discard the path we've accumulated in `resolved_path`.
      if (pending_path.is_absolute())
        resolved_path.set_to_root();
      continue;
    }

    // If the path is not a directory, but there is more to resolve, then error.
    // For example, realpath("/path/to/file.txt/") give ENOTDIR.
    if (!S_ISDIR(*mode) && !pending_path.empty())
      return Error(ENOTDIR);
  }

  return cpp::nullopt;
}

ErrorOr<char *> realpath_impl(const char *__restrict path_cstr,
                              char *__restrict resolved_path_buf) {
  if (path_cstr == nullptr)
    return Error(EINVAL);

  cpp::string_view path(path_cstr);
  if (path.size() == 0)
    return Error(ENOENT);

  if (path.size() >= PATH_MAX)
    return Error(ENAMETOOLONG);

  ResolvedPath resolved_path;
  if (!path::is_absolute(path)) {
    if (cpp::optional<Error> err = resolved_path.set_to_cwd(); err)
      return *err;
  }

  if (cpp::optional<Error> err = resolve_path(path, resolved_path); err)
    return *err;

  if (resolved_path_buf != nullptr) {
    resolved_path.copy_to(resolved_path_buf);
    return resolved_path_buf;
  }
  return resolved_path.release();
}

} // namespace

LLVM_LIBC_FUNCTION(char *, realpath,
                   (const char *__restrict path,
                    char *__restrict resolved_path)) {
  ErrorOr<char *> res = realpath_impl(path, resolved_path);
  if (!res) {
    libc_errno = res.error();
    return nullptr;
  }
  return *res;
}

} // namespace LIBC_NAMESPACE_DECL
