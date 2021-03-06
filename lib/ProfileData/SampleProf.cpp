//=-- SampleProf.cpp - Sample profiling format support --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains common definitions used in the reading and writing of
// sample profile data.
//
//===----------------------------------------------------------------------===//

#include "llvm/ProfileData/SampleProf.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/ManagedStatic.h"

using namespace llvm::sampleprof;
using namespace llvm;

namespace {
class SampleProfErrorCategoryType : public std::error_category {
  const char *name() const LLVM_NOEXCEPT override { return "llvm.sampleprof"; }
  std::string message(int IE) const override {
    sampleprof_error E = static_cast<sampleprof_error>(IE);
    switch (E) {
    case sampleprof_error::success:
      return "Success";
    case sampleprof_error::bad_magic:
      return "Invalid sample profile data (bad magic)";
    case sampleprof_error::unsupported_version:
      return "Unsupported sample profile format version";
    case sampleprof_error::too_large:
      return "Too much profile data";
    case sampleprof_error::truncated:
      return "Truncated profile data";
    case sampleprof_error::malformed:
      return "Malformed sample profile data";
    case sampleprof_error::unrecognized_format:
      return "Unrecognized sample profile encoding format";
    case sampleprof_error::unsupported_writing_format:
      return "Profile encoding format unsupported for writing operations";
    case sampleprof_error::truncated_name_table:
      return "Truncated function name table";
    case sampleprof_error::not_implemented:
      return "Unimplemented feature";
    }
    llvm_unreachable("A value of sampleprof_error has no message.");
  }
};
}

static ManagedStatic<SampleProfErrorCategoryType> ErrorCategory;

const std::error_category &llvm::sampleprof_category() {
  return *ErrorCategory;
}

/// \brief Print the sample record to the stream \p OS indented by \p Indent.
void SampleRecord::print(raw_ostream &OS, unsigned Indent) const {
  OS << NumSamples;
  if (hasCalls()) {
    OS << ", calls:";
    for (const auto &I : getCallTargets())
      OS << " " << I.first() << ":" << I.second;
  }
  OS << "\n";
}

/// \brief Print the samples collected for a function on stream \p OS.
void FunctionSamples::print(raw_ostream &OS, unsigned Indent) const {
  OS << TotalSamples << ", " << TotalHeadSamples << ", " << BodySamples.size()
     << " sampled lines\n";

  for (const auto &SI : BodySamples) {
    OS.indent(Indent);
    OS << SI.first << ": " << SI.second;
  }

  for (const auto &CS : CallsiteSamples) {
    OS.indent(Indent);
    OS << CS.first << ": ";
    CS.second.print(OS, Indent + 2);
  }
}
