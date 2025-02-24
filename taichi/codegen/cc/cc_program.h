#pragma once

#include "taichi/common/core.h"
#include "taichi/program/kernel.h"
#include "taichi/program/program.h"
#include "taichi/common/dynamic_loader.h"
#include "taichi/util/action_recorder.h"
#include "struct_cc.h"
#include "cc_runtime.h"
#include "cc_kernel.h"
#include "cc_layout.h"
#include "cc_utils.h"
#include "codegen_cc.h"
#include "context.h"
#include "taichi/util/lang_util.h"
#include <vector>
#include <memory>

namespace taichi {
class DynamicLoader;
}  // namespace taichi

namespace taichi::lang {

using namespace taichi::lang::cccp;
using CCFuncEntryType = void(cccp::CCContext *);

class CCProgramImpl : public ProgramImpl {
 public:
  explicit CCProgramImpl(CompileConfig &config);

  FunctionType compile(const CompileConfig &compile_config,
                       Kernel *kernel) override;

  std::size_t get_snode_num_dynamically_allocated(
      SNode *snode,
      uint64 *result_buffer) override {
    return 0;  // TODO: support sparse in cc.
  }

  void materialize_runtime(KernelProfilerBase *,
                           uint64 **result_buffer_ptr) override;

  void materialize_snode_tree(SNodeTree *tree, uint64 *result_buffer) override;

  void synchronize() override {
    // Not implemented yet.
  }

  std::unique_ptr<AotModuleBuilder> make_aot_module_builder(
      const DeviceCapabilityConfig &caps) override {
    // Not implemented yet.
    return nullptr;
  }

  void destroy_snode_tree(SNodeTree *snode_tree) override {
    // Not implemented yet.
  }

  CCLayout *get_layout() {
    return layout_.get();
  }

  CCRuntime *get_runtime() {
    return runtime_.get();
  }

  ~CCProgramImpl() override {
  }

  CCFuncEntryType *load_kernel(std::string const &name);
  void relink();

  CCContext *update_context(LaunchContextBuilder &ctx);
  void context_to_result_buffer();

  void dump_cache_data_to_disk() override {
    // Do nothing
  }

 protected:
  std::unique_ptr<KernelCompiler> make_kernel_compiler() override {
    TI_NOT_IMPLEMENTED;
  }

 private:
  void add_kernel(std::unique_ptr<CCKernel> kernel);

  std::vector<std::unique_ptr<CCKernel>> kernels_;
  std::unique_ptr<CCContext> context_;
  std::unique_ptr<CCRuntime> runtime_;
  std::unique_ptr<CCLayout> layout_;
  std::unique_ptr<DynamicLoader> dll_;
  std::string dll_path_;
  std::vector<char> args_buf_;
  std::vector<char> root_buf_;
  std::vector<char> gtmp_buf_;
  uint64 *result_buffer_{nullptr};
  bool need_relink_{true};
};
}  // namespace taichi::lang
