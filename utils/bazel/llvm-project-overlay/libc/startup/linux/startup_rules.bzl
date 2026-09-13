# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""LLVM libc starlark rules for building startup objects."""

load("@bazel_skylib//lib:paths.bzl", "paths")
load("@rules_cc//cc:action_names.bzl", "ACTION_NAMES")
load("@rules_cc//cc:find_cc_toolchain.bzl", "find_cc_toolchain", "use_cc_toolchain")
load("@rules_cc//cc/common:cc_common.bzl", "cc_common")
load("@rules_cc//cc/common:cc_info.bzl", "CcInfo")
load("//libc:libc_build_rules.bzl", "libc_startup_library")

def _get_object_files(deps):
    """Gets object files that are directly provided by a target in deps.

    Args:
        deps: Targets from which to extract object files.

    Returns:
        A tuple of (object files, PIC object files) from linker inputs of deps.
    """
    objects = []
    pic_objects = []
    for dep in deps:
        if CcInfo not in dep:
            fail("CcInfo not found in dep {}".format(dep.label))

        for linker_input in dep[CcInfo].linking_context.linker_inputs.to_list():
            if linker_input.owner != dep.label:
                continue  # Only interested in directly owned linker inputs.

            for lib in linker_input.libraries:
                objects.extend(lib.objects or [])
                pic_objects.extend(lib.pic_objects or [])

    return objects, pic_objects

def _get_object_files_preferring_pic(deps):
    """Returns object files from deps, and whether or not they are PIC."""
    objects, pic_objects = _get_object_files(deps)
    if pic_objects:
        return pic_objects, True
    elif objects:
        return objects, False
    else:
        fail("No object files found in deps")

def _extract_object_file_impl(ctx):
    output = ctx.actions.declare_file(ctx.label.name + ".o")
    input_objs, _ = _get_object_files_preferring_pic([ctx.attr.dep])
    if len(input_objs) != 1:
        fail("Expected exactly one input object, got: {}".format(input_objs))

    input_obj = input_objs[0]

    ctx.actions.symlink(
        output = output,
        target_file = input_obj,
    )

    return [DefaultInfo(files = depset([output]))]

_extract_object_file = rule(
    implementation = _extract_object_file_impl,
    attrs = {
        "dep": attr.label(
            mandatory = True,
            providers = [CcInfo],
        ),
    },
)

def libc_startup_object(name, src, **kwargs):
    """Compiles a C++ source file into a startup object file.

    Args:
        name: The name of the target.
        src: The C++ source file to compile.
        **kwargs: Other arguments to libc_startup_library.
    """
    library_name = name + "_lib"
    libc_startup_library(
        name = library_name,
        srcs = [src],
        **kwargs
    )
    _extract_object_file(
        name = name,
        dep = ":" + library_name,
    )

def _filter_flags(
        flags,
        separate_flag_names,
        joined_flag_prefixes):
    """Filters flags to those in joined_flag_prefixes or separate_flag_names.

    Args:
        flags: The flags to filter.
        separate_flag_names: Names of flags whose value is specified separately
            from the flag (for example "--target value").
        joined_flag_prefixes: Prefixes of flags whose value is joined to the
            flag name (for example, --target=value).
    """
    filtered_flags = []
    skip_next = False
    for i, flag in enumerate(flags):
        if skip_next:
            skip_next = False
            continue

        if flag in separate_flag_names:
            if i + 1 < len(flags):
                filtered_flags.append(flag)
                filtered_flags.append(flags[i + 1])
                skip_next = True
            continue

        for prefix in joined_flag_prefixes:
            if flag.startswith(prefix):
                filtered_flags.append(flag)
                continue

    return filtered_flags

def _merged_relocatable_object_linking_context(
        ctx,
        merged_object,
        is_pic,
        feature_configuration,
        cc_toolchain):
    """Creates a linking context for a merged relocatable object.

    This linking context consists of the merged object file
    and all the linking contexts of its indirect deps.

    Args:
      ctx: The context of the rule.
      merged_object: The merged relocatable object file.
      is_pic: Whether the merged object is PIC.
      feature_configuration: The feature configuration of the rule.
      cc_toolchain: The cc toolchain of the rule.

    Returns:
      A linking context that may be used to depend on the merged relocatable object.
    """

    # Gather transitive inputs that only originate from indirect deps.
    # Direct deps have already been merged into merged_object
    # and so shouldn't be propagated.
    direct_dep_labels = set([dep.label for dep in ctx.attr.deps])
    indirect_dep_linker_inputs = [
        linker_input
        for dep in ctx.attr.deps
        for linker_input in dep[CcInfo].linking_context.linker_inputs.to_list()
        if linker_input.owner not in direct_dep_labels
    ]
    indirect_deps_linking_context = cc_common.create_linking_context(
        linker_inputs = depset(indirect_dep_linker_inputs),
    )

    linking_context, _ = cc_common.create_linking_context_from_compilation_outputs(
        actions = ctx.actions,
        name = ctx.label.name,
        feature_configuration = feature_configuration,
        cc_toolchain = cc_toolchain,
        compilation_outputs = cc_common.create_compilation_outputs(
            # PIC objects are usable downstream even in non-PIC executables.
            objects = depset([merged_object]),
            pic_objects = depset([merged_object]) if is_pic else None,
        ),
        linking_contexts = [indirect_deps_linking_context],
    )
    return linking_context

def _merge_relocatable_object_impl(ctx):
    cc_toolchain = find_cc_toolchain(ctx)
    output = ctx.actions.declare_file(ctx.label.name + ".o")

    # A more general approach would be to generate both PIC and non-PIC merged
    # objects, but just preferring PIC when available should be fine until there
    # is a specific need to propagate both.
    input_objs, is_pic = _get_object_files_preferring_pic(ctx.attr.deps)

    feature_configuration = cc_common.configure_features(
        ctx = ctx,
        cc_toolchain = cc_toolchain,
        requested_features = ctx.features,
        unsupported_features = ctx.disabled_features,
    )
    link_variables = cc_common.create_link_variables(
        cc_toolchain = cc_toolchain,
        feature_configuration = feature_configuration,
        is_linking_dynamic_library = False,
    )
    link_flags = cc_common.get_memory_inefficient_command_line(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_link_executable,
        variables = link_variables,
    )
    linker = cc_common.get_tool_for_action(
        feature_configuration = feature_configuration,
        action_name = ACTION_NAMES.cpp_link_executable,
    )
    relocatable_link_flags = _filter_flags(
        link_flags,
        ["-target", "--target", "--sysroot", "-isysroot"],
        ["-fuse-ld=", "-m", "--target=", "--sysroot="],
    )

    args = ctx.actions.args()
    args.add_all(relocatable_link_flags)

    bindir = paths.dirname(linker)
    if bindir:
        args.add("-B" + bindir)

    args.add("-r")
    args.add("-nostdlib")
    args.add("-o", output)
    args.add_all(input_objs)

    ctx.actions.run(
        outputs = [output],
        inputs = depset(
            input_objs,
            transitive = [cc_toolchain.all_files],
        ),
        executable = linker,
        arguments = [args],
        mnemonic = "MergeRelocatableObject",
        use_default_shell_env = True,
    )

    return [
        DefaultInfo(files = depset([output])),
        CcInfo(
            linking_context = _merged_relocatable_object_linking_context(
                ctx,
                merged_object = output,
                is_pic = is_pic,
                feature_configuration = feature_configuration,
                cc_toolchain = cc_toolchain,
            ),
        ),
    ]

merge_relocatable_object = rule(
    implementation = _merge_relocatable_object_impl,
    doc = """Merges multiple object files into a single relocatable object file.

    This rule mimics CMake's `merge_relocatable_object`,
    running the toolchain's linker driver `-r -nostdlib` on all direct deps.
    """,
    attrs = {
        "deps": attr.label_list(
            mandatory = True,
            providers = [CcInfo],
            doc = "The list of cc targets whose object files should be merged.",
        ),
    },
    toolchains = use_cc_toolchain(),
    fragments = ["cpp"],
    provides = [CcInfo, DefaultInfo],
)
