#include <c10/util/intrusive_ptr.h>
#include <c10d/ProcessGroup.hpp>
#include <torch/library.h>

namespace c10d {
namespace ops {

// Below are ProcessGroup's corresponding ops for each backend. Ops are but
// routed through the dispatcher to be dispatched to the appropriate backend.
// Currently a no-op as the process group does not have a list of backends.
std::tuple<std::vector<at::Tensor>, c10::intrusive_ptr<Work>> broadcast_cpu_(
    at::TensorList tensors,
    const c10::intrusive_ptr<ProcessGroup>& process_group,
    int64_t root_rank,
    int64_t root_tensor,
    int64_t timeout) {
  auto tensor_vec = tensors.vec();
  auto work = process_group->broadcast(
      tensor_vec,
      BroadcastOptions{
          root_rank, root_tensor, std::chrono::milliseconds(timeout)});

  return std::tuple<std::vector<at::Tensor>, c10::intrusive_ptr<Work>>(
      std::move(tensor_vec), work);
}

std::tuple<std::vector<at::Tensor>, c10::intrusive_ptr<Work>> broadcast_cuda_(
    at::TensorList tensors,
    const c10::intrusive_ptr<ProcessGroup>& process_group,
    int64_t root_rank,
    int64_t root_tensor,
    int64_t timeout) {
  auto tensor_vec = tensors.vec();
  auto work = process_group->broadcast(
      tensor_vec,
      BroadcastOptions{
          root_rank, root_tensor, std::chrono::milliseconds(timeout)});

  return std::tuple<std::vector<at::Tensor>, c10::intrusive_ptr<Work>>(
      std::move(tensor_vec), work);
}

// register functions to dispatcher
namespace {
TORCH_LIBRARY_IMPL(c10d, CPU, m) {
  m.impl("broadcast_", broadcast_cpu_);
}

TORCH_LIBRARY_IMPL(c10d, CUDA, m) {
  m.impl("broadcast_", broadcast_cuda_);
}
} // namespace

} // namespace ops
} // namespace c10d
