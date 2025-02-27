//===- bolt/RuntimeLibs/RuntimeLibrary.h - Runtime Library ------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the RuntimeLibrary class, which
// provides all the necessary utilities to link runtime libraries during binary
// rewriting, such as the instrumentation runtime library.
//
//===----------------------------------------------------------------------===//

#ifndef BOLT_RUNTIMELIBS_RUNTIME_LIBRARY_H
#define BOLT_RUNTIMELIBS_RUNTIME_LIBRARY_H

#include "bolt/Core/Linker.h"
#include "llvm/ADT/StringRef.h"
#include <vector>

namespace llvm {

class MCStreamer;

namespace bolt {

class BinaryContext;

class RuntimeLibrary {
  // vtable anchor.
  virtual void anchor();

public:
  virtual ~RuntimeLibrary() = default;

  uint64_t getRuntimeFiniAddress() const { return RuntimeFiniAddress; }

  uint64_t getRuntimeStartAddress() const { return RuntimeStartAddress; }

  /// Add custom sections added by the runtime libraries.
  virtual void
  addRuntimeLibSections(std::vector<std::string> &SecNames) const = 0;

  /// Validity check and modify if necessary all the command line options
  /// for this runtime library.
  virtual void adjustCommandLineOptions(const BinaryContext &BC) const = 0;

  /// Emit data structures that will be necessary during runtime.
  virtual void emitBinary(BinaryContext &BC, MCStreamer &Streamer) = 0;

  /// Link with the library code.
  virtual void link(BinaryContext &BC, StringRef ToolPath, BOLTLinker &Linker,
                    BOLTLinker::SectionsMapper MapSections) = 0;

protected:
  /// The fini and init address set by the runtime library.
  uint64_t RuntimeFiniAddress{0};
  uint64_t RuntimeStartAddress{0};

  /// Get the full path to a runtime library specified by \p LibFileName and \p
  /// ToolPath.
  static std::string getLibPathByToolPath(StringRef ToolPath,
                                          StringRef ToolSubPath,
                                          StringRef LibFileName);

  /// Create architecture-specific ToolSubPath to be used in the full path
  static std::string createToolSubPath(StringRef ArchName);

  /// Get the full path to a runtime library by the install directory.
  static std::string getLibPathByInstalled(StringRef ToolSubPath, StringRef LibFileName);

  /// Gets the full path to a runtime library based on whether it exists
  /// in the install libdir or runtime libdir.
  static std::string getLibPath(StringRef ToolPath, StringRef ToolSubPath,
                                StringRef LibFileName);

  /// Load a static runtime library specified by \p LibPath.
  static void loadLibrary(StringRef LibPath, BOLTLinker &Linker,
                          BOLTLinker::SectionsMapper MapSections);
};

} // namespace bolt
} // namespace llvm

#endif // BOLT_RUNTIMELIBS_RUNTIME_LIBRARY_H
