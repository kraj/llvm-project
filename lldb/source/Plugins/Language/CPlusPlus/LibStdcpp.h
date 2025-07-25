//===-- LibStdcpp.h ---------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_SOURCE_PLUGINS_LANGUAGE_CPLUSPLUS_LIBSTDCPP_H
#define LLDB_SOURCE_PLUGINS_LANGUAGE_CPLUSPLUS_LIBSTDCPP_H

#include "lldb/DataFormatters/TypeSummary.h"
#include "lldb/DataFormatters/TypeSynthetic.h"
#include "lldb/Utility/Stream.h"
#include "lldb/ValueObject/ValueObject.h"

namespace lldb_private {
namespace formatters {
bool LibStdcppStringSummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &options); // libstdc++ std::string

bool LibStdcppSmartPointerSummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions
        &options); // libstdc++ std::shared_ptr<> and std::weak_ptr<>

bool LibStdcppUniquePointerSummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &options); // libstdc++ std::unique_ptr<>

bool LibStdcppVariantSummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &options); // libstdc++ std::variant<>

SyntheticChildrenFrontEnd *
LibstdcppMapIteratorSyntheticFrontEndCreator(CXXSyntheticChildren *,
                                             lldb::ValueObjectSP);

SyntheticChildrenFrontEnd *
LibStdcppTupleSyntheticFrontEndCreator(CXXSyntheticChildren *,
                                       lldb::ValueObjectSP);

SyntheticChildrenFrontEnd *
LibStdcppBitsetSyntheticFrontEndCreator(CXXSyntheticChildren *,
                                        lldb::ValueObjectSP);

SyntheticChildrenFrontEnd *
LibStdcppOptionalSyntheticFrontEndCreator(CXXSyntheticChildren *,
                                          lldb::ValueObjectSP);

SyntheticChildrenFrontEnd *
LibStdcppVectorIteratorSyntheticFrontEndCreator(CXXSyntheticChildren *,
                                                lldb::ValueObjectSP);

SyntheticChildrenFrontEnd *
LibStdcppSharedPtrSyntheticFrontEndCreator(CXXSyntheticChildren *,
                                           lldb::ValueObjectSP);

SyntheticChildrenFrontEnd *
LibStdcppUniquePtrSyntheticFrontEndCreator(CXXSyntheticChildren *,
                                           lldb::ValueObjectSP);

bool LibStdcppVariantSummaryProvider(ValueObject &valobj, Stream &stream,
                                     const TypeSummaryOptions &options);

} // namespace formatters
} // namespace lldb_private

#endif // LLDB_SOURCE_PLUGINS_LANGUAGE_CPLUSPLUS_LIBSTDCPP_H
