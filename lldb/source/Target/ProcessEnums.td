include "../../include/lldb/Core/EnumeratorBase.td"

def follow_fork_mode_values : EnumValues<
  "lldb::FollowForkMode", [
    EnumValue<"eFollowParent", "parent",
      "Continue tracing the parent process and detach the child.">,
    EnumValue<"eFollowChild", "child",
      "Trace the child process and detach the parent.">,
  ]>;

