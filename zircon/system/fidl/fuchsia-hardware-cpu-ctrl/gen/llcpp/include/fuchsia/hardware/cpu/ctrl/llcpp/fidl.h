// WARNING: This file is machine generated by fidlgen.

#pragma once

#include <lib/fidl/internal.h>
#include <lib/fidl/txn_header.h>
#include <lib/fidl/llcpp/array.h>
#include <lib/fidl/llcpp/coding.h>
#include <lib/fidl/llcpp/connect_service.h>
#include <lib/fidl/llcpp/service_handler_interface.h>
#include <lib/fidl/llcpp/string_view.h>
#include <lib/fidl/llcpp/sync_call.h>
#include <lib/fidl/llcpp/traits.h>
#include <lib/fidl/llcpp/transaction.h>
#include <lib/fidl/llcpp/vector_view.h>
#include <lib/fit/function.h>
#include <lib/zx/channel.h>
#include <zircon/fidl.h>

namespace llcpp {

namespace fuchsia {
namespace hardware {
namespace cpu {
namespace ctrl {

struct CpuPerformanceStateInfo;
struct Device_GetPerformanceStateInfo_Response;
struct Device_GetPerformanceStateInfo_Result;
class Device;

extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_Device_GetPerformanceStateInfo_ResultTable;

struct Device_GetPerformanceStateInfo_Result {
  Device_GetPerformanceStateInfo_Result() : ordinal_(Ordinal::Invalid), envelope_{} {}

  enum class Tag : fidl_xunion_tag_t {
    kResponse = 1,  // 0x1
    kErr = 2,  // 0x2
  };

  bool has_invalid_tag() const { return ordinal_ == Ordinal::Invalid; }

  bool is_response() const { return ordinal() == Ordinal::kResponse; }

  static Device_GetPerformanceStateInfo_Result WithResponse(::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response* val) {
    Device_GetPerformanceStateInfo_Result result;
    result.set_response(val);
    return result;
  }

  void set_response(::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response* elem) {
    ordinal_ = Ordinal::kResponse;
    envelope_.data = static_cast<void*>(elem);
  }

  ::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response& mutable_response() {
    ZX_ASSERT(ordinal() == Ordinal::kResponse);
    return *static_cast<::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response*>(envelope_.data);
  }
  const ::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response& response() const {
    ZX_ASSERT(ordinal() == Ordinal::kResponse);
    return *static_cast<::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response*>(envelope_.data);
  }

  bool is_err() const { return ordinal() == Ordinal::kErr; }

  static Device_GetPerformanceStateInfo_Result WithErr(int32_t* val) {
    Device_GetPerformanceStateInfo_Result result;
    result.set_err(val);
    return result;
  }

  void set_err(int32_t* elem) {
    ordinal_ = Ordinal::kErr;
    envelope_.data = static_cast<void*>(elem);
  }

  int32_t& mutable_err() {
    ZX_ASSERT(ordinal() == Ordinal::kErr);
    return *static_cast<int32_t*>(envelope_.data);
  }
  const int32_t& err() const {
    ZX_ASSERT(ordinal() == Ordinal::kErr);
    return *static_cast<int32_t*>(envelope_.data);
  }
  Tag which() const {
    ZX_ASSERT(!has_invalid_tag());
    return static_cast<Tag>(ordinal());
  }

  static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_Device_GetPerformanceStateInfo_ResultTable;
  static constexpr uint32_t MaxNumHandles = 0;
  static constexpr uint32_t PrimarySize = 24;
  [[maybe_unused]]
  static constexpr uint32_t MaxOutOfLine = 16;

 private:
  enum class Ordinal : fidl_xunion_tag_t {
    Invalid = 0,
    kResponse = 1,  // 0x1
    kErr = 2,  // 0x2
  };

  Ordinal ordinal() const {
    return ordinal_;
  }

  static void SizeAndOffsetAssertionHelper();
  Ordinal ordinal_;
  FIDL_ALIGNDECL
  fidl_envelope_t envelope_;
};

constexpr int64_t VOLTAGE_UNKNOWN = -1;

// CpuPerformanceStateInfo::frequency_hz and CpuPerformanceStateInfo::voltage_uv
// are set to this if the frequency and voltage for the given performance state
// are unknown respectively.
constexpr int64_t FREQUENCY_UNKNOWN = -1;

extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_CpuPerformanceStateInfoTable;

// A collection of some basic information for a given performance state.
struct CpuPerformanceStateInfo {
  static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_CpuPerformanceStateInfoTable;
  static constexpr uint32_t MaxNumHandles = 0;
  static constexpr uint32_t PrimarySize = 16;
  [[maybe_unused]]
  static constexpr uint32_t MaxOutOfLine = 0;

  // Frequency the core is operating at in hertz.
  int64_t frequency_hz = {};

  int64_t voltage_uv = {};
};

extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_Device_GetPerformanceStateInfo_ResponseTable;

struct Device_GetPerformanceStateInfo_Response {
  static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_Device_GetPerformanceStateInfo_ResponseTable;
  static constexpr uint32_t MaxNumHandles = 0;
  static constexpr uint32_t PrimarySize = 16;
  [[maybe_unused]]
  static constexpr uint32_t MaxOutOfLine = 0;

  ::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo info = {};
};

extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_DeviceGetPerformanceStateInfoRequestTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_DeviceGetPerformanceStateInfoResponseTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_DeviceGetNumLogicalCoresRequestTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_DeviceGetNumLogicalCoresResponseTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_DeviceGetLogicalCoreIdRequestTable;
extern "C" const fidl_type_t v1_fuchsia_hardware_cpu_ctrl_DeviceGetLogicalCoreIdResponseTable;

class Device final {
  Device() = delete;
 public:

  struct GetPerformanceStateInfoResponse final {
    FIDL_ALIGNDECL
    fidl_message_header_t _hdr;
    ::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Result result;

    static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_DeviceGetPerformanceStateInfoResponseTable;
    static constexpr uint32_t MaxNumHandles = 0;
    static constexpr uint32_t PrimarySize = 40;
    static constexpr uint32_t MaxOutOfLine = 16;
    static constexpr bool HasFlexibleEnvelope = false;
    static constexpr bool ContainsUnion = true;
    static constexpr ::fidl::internal::TransactionalMessageKind MessageKind =
        ::fidl::internal::TransactionalMessageKind::kResponse;
  };
  struct GetPerformanceStateInfoRequest final {
    FIDL_ALIGNDECL
    fidl_message_header_t _hdr;
    uint32_t state;

    static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_DeviceGetPerformanceStateInfoRequestTable;
    static constexpr uint32_t MaxNumHandles = 0;
    static constexpr uint32_t PrimarySize = 24;
    static constexpr uint32_t MaxOutOfLine = 0;
    static constexpr uint32_t AltPrimarySize = 24;
    static constexpr uint32_t AltMaxOutOfLine = 0;
    static constexpr bool HasFlexibleEnvelope = false;
    static constexpr bool ContainsUnion = false;
    static constexpr ::fidl::internal::TransactionalMessageKind MessageKind =
        ::fidl::internal::TransactionalMessageKind::kRequest;
    using ResponseType = GetPerformanceStateInfoResponse;
  };

  struct GetNumLogicalCoresResponse final {
    FIDL_ALIGNDECL
    fidl_message_header_t _hdr;
    uint64_t count;

    static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_DeviceGetNumLogicalCoresResponseTable;
    static constexpr uint32_t MaxNumHandles = 0;
    static constexpr uint32_t PrimarySize = 24;
    static constexpr uint32_t MaxOutOfLine = 0;
    static constexpr bool HasFlexibleEnvelope = false;
    static constexpr bool ContainsUnion = false;
    static constexpr ::fidl::internal::TransactionalMessageKind MessageKind =
        ::fidl::internal::TransactionalMessageKind::kResponse;
  };
  using GetNumLogicalCoresRequest = ::fidl::AnyZeroArgMessage;

  struct GetLogicalCoreIdResponse final {
    FIDL_ALIGNDECL
    fidl_message_header_t _hdr;
    uint64_t id;

    static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_DeviceGetLogicalCoreIdResponseTable;
    static constexpr uint32_t MaxNumHandles = 0;
    static constexpr uint32_t PrimarySize = 24;
    static constexpr uint32_t MaxOutOfLine = 0;
    static constexpr bool HasFlexibleEnvelope = false;
    static constexpr bool ContainsUnion = false;
    static constexpr ::fidl::internal::TransactionalMessageKind MessageKind =
        ::fidl::internal::TransactionalMessageKind::kResponse;
  };
  struct GetLogicalCoreIdRequest final {
    FIDL_ALIGNDECL
    fidl_message_header_t _hdr;
    uint64_t index;

    static constexpr const fidl_type_t* Type = &v1_fuchsia_hardware_cpu_ctrl_DeviceGetLogicalCoreIdRequestTable;
    static constexpr uint32_t MaxNumHandles = 0;
    static constexpr uint32_t PrimarySize = 24;
    static constexpr uint32_t MaxOutOfLine = 0;
    static constexpr uint32_t AltPrimarySize = 24;
    static constexpr uint32_t AltMaxOutOfLine = 0;
    static constexpr bool HasFlexibleEnvelope = false;
    static constexpr bool ContainsUnion = false;
    static constexpr ::fidl::internal::TransactionalMessageKind MessageKind =
        ::fidl::internal::TransactionalMessageKind::kRequest;
    using ResponseType = GetLogicalCoreIdResponse;
  };


  // Collection of return types of FIDL calls in this interface.
  class ResultOf final {
    ResultOf() = delete;
   private:
    template <typename ResponseType>
    class GetPerformanceStateInfo_Impl final : private ::fidl::internal::OwnedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::OwnedSyncCallBase<ResponseType>;
     public:
      GetPerformanceStateInfo_Impl(::zx::unowned_channel _client_end, uint32_t state);
      ~GetPerformanceStateInfo_Impl() = default;
      GetPerformanceStateInfo_Impl(GetPerformanceStateInfo_Impl&& other) = default;
      GetPerformanceStateInfo_Impl& operator=(GetPerformanceStateInfo_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };
    template <typename ResponseType>
    class GetNumLogicalCores_Impl final : private ::fidl::internal::OwnedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::OwnedSyncCallBase<ResponseType>;
     public:
      GetNumLogicalCores_Impl(::zx::unowned_channel _client_end);
      ~GetNumLogicalCores_Impl() = default;
      GetNumLogicalCores_Impl(GetNumLogicalCores_Impl&& other) = default;
      GetNumLogicalCores_Impl& operator=(GetNumLogicalCores_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };
    template <typename ResponseType>
    class GetLogicalCoreId_Impl final : private ::fidl::internal::OwnedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::OwnedSyncCallBase<ResponseType>;
     public:
      GetLogicalCoreId_Impl(::zx::unowned_channel _client_end, uint64_t index);
      ~GetLogicalCoreId_Impl() = default;
      GetLogicalCoreId_Impl(GetLogicalCoreId_Impl&& other) = default;
      GetLogicalCoreId_Impl& operator=(GetLogicalCoreId_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };

   public:
    using GetPerformanceStateInfo = GetPerformanceStateInfo_Impl<GetPerformanceStateInfoResponse>;
    using GetNumLogicalCores = GetNumLogicalCores_Impl<GetNumLogicalCoresResponse>;
    using GetLogicalCoreId = GetLogicalCoreId_Impl<GetLogicalCoreIdResponse>;
  };

  // Collection of return types of FIDL calls in this interface,
  // when the caller-allocate flavor or in-place call is used.
  class UnownedResultOf final {
    UnownedResultOf() = delete;
   private:
    template <typename ResponseType>
    class GetPerformanceStateInfo_Impl final : private ::fidl::internal::UnownedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::UnownedSyncCallBase<ResponseType>;
     public:
      GetPerformanceStateInfo_Impl(::zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint32_t state, ::fidl::BytePart _response_buffer);
      ~GetPerformanceStateInfo_Impl() = default;
      GetPerformanceStateInfo_Impl(GetPerformanceStateInfo_Impl&& other) = default;
      GetPerformanceStateInfo_Impl& operator=(GetPerformanceStateInfo_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };
    template <typename ResponseType>
    class GetNumLogicalCores_Impl final : private ::fidl::internal::UnownedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::UnownedSyncCallBase<ResponseType>;
     public:
      GetNumLogicalCores_Impl(::zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer);
      ~GetNumLogicalCores_Impl() = default;
      GetNumLogicalCores_Impl(GetNumLogicalCores_Impl&& other) = default;
      GetNumLogicalCores_Impl& operator=(GetNumLogicalCores_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };
    template <typename ResponseType>
    class GetLogicalCoreId_Impl final : private ::fidl::internal::UnownedSyncCallBase<ResponseType> {
      using Super = ::fidl::internal::UnownedSyncCallBase<ResponseType>;
     public:
      GetLogicalCoreId_Impl(::zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint64_t index, ::fidl::BytePart _response_buffer);
      ~GetLogicalCoreId_Impl() = default;
      GetLogicalCoreId_Impl(GetLogicalCoreId_Impl&& other) = default;
      GetLogicalCoreId_Impl& operator=(GetLogicalCoreId_Impl&& other) = default;
      using Super::status;
      using Super::error;
      using Super::ok;
      using Super::Unwrap;
      using Super::value;
      using Super::operator->;
      using Super::operator*;
    };

   public:
    using GetPerformanceStateInfo = GetPerformanceStateInfo_Impl<GetPerformanceStateInfoResponse>;
    using GetNumLogicalCores = GetNumLogicalCores_Impl<GetNumLogicalCoresResponse>;
    using GetLogicalCoreId = GetLogicalCoreId_Impl<GetLogicalCoreIdResponse>;
  };

  class SyncClient final {
   public:
    explicit SyncClient(::zx::channel channel) : channel_(std::move(channel)) {}
    ~SyncClient() = default;
    SyncClient(SyncClient&&) = default;
    SyncClient& operator=(SyncClient&&) = default;

    const ::zx::channel& channel() const { return channel_; }

    ::zx::channel* mutable_channel() { return &channel_; }

    // Returns information about a given performance state for this performance
    // domain.
    // Allocates 80 bytes of message buffer on the stack. No heap allocation necessary.
    ResultOf::GetPerformanceStateInfo GetPerformanceStateInfo(uint32_t state);

    // Returns information about a given performance state for this performance
    // domain.
    // Caller provides the backing storage for FIDL message via request and response buffers.
    UnownedResultOf::GetPerformanceStateInfo GetPerformanceStateInfo(::fidl::BytePart _request_buffer, uint32_t state, ::fidl::BytePart _response_buffer);

    // Returns the number of logical cores contained within this performance
    // domain.
    // Allocates 40 bytes of message buffer on the stack. No heap allocation necessary.
    ResultOf::GetNumLogicalCores GetNumLogicalCores();

    // Returns the number of logical cores contained within this performance
    // domain.
    // Caller provides the backing storage for FIDL message via request and response buffers.
    UnownedResultOf::GetNumLogicalCores GetNumLogicalCores(::fidl::BytePart _response_buffer);

    // Returns a global system-wide core ID for the nth core in this
    // performance domain. `index` must be a value in the range [0, n) where
    // n is the value returned by GetNumLogicalCores().
    // Allocates 48 bytes of message buffer on the stack. No heap allocation necessary.
    ResultOf::GetLogicalCoreId GetLogicalCoreId(uint64_t index);

    // Returns a global system-wide core ID for the nth core in this
    // performance domain. `index` must be a value in the range [0, n) where
    // n is the value returned by GetNumLogicalCores().
    // Caller provides the backing storage for FIDL message via request and response buffers.
    UnownedResultOf::GetLogicalCoreId GetLogicalCoreId(::fidl::BytePart _request_buffer, uint64_t index, ::fidl::BytePart _response_buffer);

   private:
    ::zx::channel channel_;
  };

  // Methods to make a sync FIDL call directly on an unowned channel, avoiding setting up a client.
  class Call final {
    Call() = delete;
   public:

    // Returns information about a given performance state for this performance
    // domain.
    // Allocates 80 bytes of message buffer on the stack. No heap allocation necessary.
    static ResultOf::GetPerformanceStateInfo GetPerformanceStateInfo(::zx::unowned_channel _client_end, uint32_t state);

    // Returns information about a given performance state for this performance
    // domain.
    // Caller provides the backing storage for FIDL message via request and response buffers.
    static UnownedResultOf::GetPerformanceStateInfo GetPerformanceStateInfo(::zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint32_t state, ::fidl::BytePart _response_buffer);

    // Returns the number of logical cores contained within this performance
    // domain.
    // Allocates 40 bytes of message buffer on the stack. No heap allocation necessary.
    static ResultOf::GetNumLogicalCores GetNumLogicalCores(::zx::unowned_channel _client_end);

    // Returns the number of logical cores contained within this performance
    // domain.
    // Caller provides the backing storage for FIDL message via request and response buffers.
    static UnownedResultOf::GetNumLogicalCores GetNumLogicalCores(::zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer);

    // Returns a global system-wide core ID for the nth core in this
    // performance domain. `index` must be a value in the range [0, n) where
    // n is the value returned by GetNumLogicalCores().
    // Allocates 48 bytes of message buffer on the stack. No heap allocation necessary.
    static ResultOf::GetLogicalCoreId GetLogicalCoreId(::zx::unowned_channel _client_end, uint64_t index);

    // Returns a global system-wide core ID for the nth core in this
    // performance domain. `index` must be a value in the range [0, n) where
    // n is the value returned by GetNumLogicalCores().
    // Caller provides the backing storage for FIDL message via request and response buffers.
    static UnownedResultOf::GetLogicalCoreId GetLogicalCoreId(::zx::unowned_channel _client_end, ::fidl::BytePart _request_buffer, uint64_t index, ::fidl::BytePart _response_buffer);

  };

  // Messages are encoded and decoded in-place when these methods are used.
  // Additionally, requests must be already laid-out according to the FIDL wire-format.
  class InPlace final {
    InPlace() = delete;
   public:

    // Returns information about a given performance state for this performance
    // domain.
    static ::fidl::DecodeResult<GetPerformanceStateInfoResponse> GetPerformanceStateInfo(::zx::unowned_channel _client_end, ::fidl::DecodedMessage<GetPerformanceStateInfoRequest> params, ::fidl::BytePart response_buffer);

    // Returns the number of logical cores contained within this performance
    // domain.
    static ::fidl::DecodeResult<GetNumLogicalCoresResponse> GetNumLogicalCores(::zx::unowned_channel _client_end, ::fidl::BytePart response_buffer);

    // Returns a global system-wide core ID for the nth core in this
    // performance domain. `index` must be a value in the range [0, n) where
    // n is the value returned by GetNumLogicalCores().
    static ::fidl::DecodeResult<GetLogicalCoreIdResponse> GetLogicalCoreId(::zx::unowned_channel _client_end, ::fidl::DecodedMessage<GetLogicalCoreIdRequest> params, ::fidl::BytePart response_buffer);

  };

  // Pure-virtual interface to be implemented by a server.
  class Interface {
   public:
    Interface() = default;
    virtual ~Interface() = default;
    using _Outer = Device;
    using _Base = ::fidl::CompleterBase;

    class GetPerformanceStateInfoCompleterBase : public _Base {
     public:
      void Reply(::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Result result);
      void ReplySuccess(::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo info);
      void ReplyError(int32_t error);
      void Reply(::fidl::BytePart _buffer, ::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Result result);
      void ReplySuccess(::fidl::BytePart _buffer, ::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo info);
      void Reply(::fidl::DecodedMessage<GetPerformanceStateInfoResponse> params);

     protected:
      using ::fidl::CompleterBase::CompleterBase;
    };

    using GetPerformanceStateInfoCompleter = ::fidl::Completer<GetPerformanceStateInfoCompleterBase>;

    virtual void GetPerformanceStateInfo(uint32_t state, GetPerformanceStateInfoCompleter::Sync _completer) = 0;

    class GetNumLogicalCoresCompleterBase : public _Base {
     public:
      void Reply(uint64_t count);
      void Reply(::fidl::BytePart _buffer, uint64_t count);
      void Reply(::fidl::DecodedMessage<GetNumLogicalCoresResponse> params);

     protected:
      using ::fidl::CompleterBase::CompleterBase;
    };

    using GetNumLogicalCoresCompleter = ::fidl::Completer<GetNumLogicalCoresCompleterBase>;

    virtual void GetNumLogicalCores(GetNumLogicalCoresCompleter::Sync _completer) = 0;

    class GetLogicalCoreIdCompleterBase : public _Base {
     public:
      void Reply(uint64_t id);
      void Reply(::fidl::BytePart _buffer, uint64_t id);
      void Reply(::fidl::DecodedMessage<GetLogicalCoreIdResponse> params);

     protected:
      using ::fidl::CompleterBase::CompleterBase;
    };

    using GetLogicalCoreIdCompleter = ::fidl::Completer<GetLogicalCoreIdCompleterBase>;

    virtual void GetLogicalCoreId(uint64_t index, GetLogicalCoreIdCompleter::Sync _completer) = 0;

  };

  // Attempts to dispatch the incoming message to a handler function in the server implementation.
  // If there is no matching handler, it returns false, leaving the message and transaction intact.
  // In all other cases, it consumes the message and returns true.
  // It is possible to chain multiple TryDispatch functions in this manner.
  static bool TryDispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn);

  // Dispatches the incoming message to one of the handlers functions in the interface.
  // If there is no matching handler, it closes all the handles in |msg| and closes the channel with
  // a |ZX_ERR_NOT_SUPPORTED| epitaph, before returning false. The message should then be discarded.
  static bool Dispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn);

  // Same as |Dispatch|, but takes a |void*| instead of |Interface*|. Only used with |fidl::Bind|
  // to reduce template expansion.
  // Do not call this method manually. Use |Dispatch| instead.
  static bool TypeErasedDispatch(void* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
    return Dispatch(static_cast<Interface*>(impl), msg, txn);
  }


  // Helper functions to fill in the transaction header in a |DecodedMessage<TransactionalMessage>|.
  class SetTransactionHeaderFor final {
    SetTransactionHeaderFor() = delete;
   public:
    static void GetPerformanceStateInfoRequest(const ::fidl::DecodedMessage<Device::GetPerformanceStateInfoRequest>& _msg);
    static void GetPerformanceStateInfoResponse(const ::fidl::DecodedMessage<Device::GetPerformanceStateInfoResponse>& _msg);
    static void GetNumLogicalCoresRequest(const ::fidl::DecodedMessage<Device::GetNumLogicalCoresRequest>& _msg);
    static void GetNumLogicalCoresResponse(const ::fidl::DecodedMessage<Device::GetNumLogicalCoresResponse>& _msg);
    static void GetLogicalCoreIdRequest(const ::fidl::DecodedMessage<Device::GetLogicalCoreIdRequest>& _msg);
    static void GetLogicalCoreIdResponse(const ::fidl::DecodedMessage<Device::GetLogicalCoreIdResponse>& _msg);
  };
};

}  // namespace ctrl
}  // namespace cpu
}  // namespace hardware
}  // namespace fuchsia
}  // namespace llcpp

namespace fidl {

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo> : public std::true_type {};
static_assert(std::is_standard_layout_v<::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo>);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo, frequency_hz) == 0);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo, voltage_uv) == 8);
static_assert(sizeof(::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo) == ::llcpp::fuchsia::hardware::cpu::ctrl::CpuPerformanceStateInfo::PrimarySize);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response> : public std::true_type {};
static_assert(std::is_standard_layout_v<::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response>);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response, info) == 0);
static_assert(sizeof(::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response) == ::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Response::PrimarySize);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Result> : public std::true_type {};
static_assert(std::is_standard_layout_v<::llcpp::fuchsia::hardware::cpu::ctrl::Device_GetPerformanceStateInfo_Result>);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoRequest> : public std::true_type {};
template <>
struct IsFidlMessage<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoRequest> : public std::true_type {};
static_assert(sizeof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoRequest)
    == ::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoRequest::PrimarySize);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoRequest, state) == 16);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoResponse> : public std::true_type {};
template <>
struct IsFidlMessage<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoResponse> : public std::true_type {};
static_assert(sizeof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoResponse)
    == ::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoResponse::PrimarySize);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetPerformanceStateInfoResponse, result) == 16);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetNumLogicalCoresResponse> : public std::true_type {};
template <>
struct IsFidlMessage<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetNumLogicalCoresResponse> : public std::true_type {};
static_assert(sizeof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetNumLogicalCoresResponse)
    == ::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetNumLogicalCoresResponse::PrimarySize);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetNumLogicalCoresResponse, count) == 16);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdRequest> : public std::true_type {};
template <>
struct IsFidlMessage<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdRequest> : public std::true_type {};
static_assert(sizeof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdRequest)
    == ::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdRequest::PrimarySize);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdRequest, index) == 16);

template <>
struct IsFidlType<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdResponse> : public std::true_type {};
template <>
struct IsFidlMessage<::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdResponse> : public std::true_type {};
static_assert(sizeof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdResponse)
    == ::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdResponse::PrimarySize);
static_assert(offsetof(::llcpp::fuchsia::hardware::cpu::ctrl::Device::GetLogicalCoreIdResponse, id) == 16);

}  // namespace fidl
