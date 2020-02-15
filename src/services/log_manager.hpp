#pragma once

#include "util/filesystem.hpp"

#include "core/service.hpp"

#define LOGURU_USE_FMTLIB 1
#include <debug_assert.hpp>
#include <loguru.hpp>

#include "util/macros.hpp"
#include "services/application.hpp"

namespace otto::services {

  struct LogManager : core::Service {
    /// Initialize the logger
    LogManager(int argc = 0,
               char** argv = nullptr,
               bool enable_console = true,
               const char* logFilePath = nullptr);

    /// Set how the current thread appears in the log
    void set_thread_name(const std::string& name);

    static LogManager& current() {
      return Application::current().log_manager;
    }
  };

} // namespace otto::services

/// Shorthand to the loguru macro LOG_F(INFO, ...)
#define LOGI(...) LOG_F(INFO, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_F(WARNING, ...)
#define LOGW(...) LOG_F(WARNING, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_F(ERROR, ...)
#define LOGE(...) LOG_F(ERROR, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_F(FATAL, ...)
#define LOGF(...) LOG_F(FATAL, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_F(INFO, ...)
#define DLOGI(...) DLOG_F(INFO, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_F(WARNING, ...)
#define DLOGW(...) DLOG_F(WARNING, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_F(ERROR, ...)
#define DLOGE(...) DLOG_F(ERROR, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_F(FATAL, ...)
#define DLOGF(...) DLOG_F(FATAL, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_IF_F(INFO, ...)
#define LOGI_IF(...) LOG_IF_F(INFO, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_IF_F(WARNING, ...)
#define LOGW_IF(...) LOG_IF_F(WARNING, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_IF_F(ERROR, ...)
#define LOGE_IF(...) LOG_IF_F(ERROR, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_IF_F(FATAL, ...)
#define LOGF_IF(...) LOG_IF_F(FATAL, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_IF_F(INFO, ...)
#define DLOGI_IF(...) DLOG_IF_F(INFO, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_IF_F(WARNING, ...)
#define DLOGW_IF(...) DLOG_IF_F(WARNING, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_IF_F(ERROR, ...)
#define DLOGE_IF(...) DLOG_IF_F(ERROR, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_IF_F(FATAL, ...)
#define DLOGF_IF(...) DLOG_IF_F(FATAL, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_SCOPE_F(INFO, ...)
#define LOGI_SCOPE(...) LOG_SCOPE_F(INFO, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_SCOPE_F(WARNING, ...)
#define LOGW_SCOPE(...) LOG_SCOPE_F(WARNING, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_SCOPE_F(ERROR, ...)
#define LOGE_SCOPE(...) LOG_SCOPE_F(ERROR, __VA_ARGS__)

/// Shorthand to the loguru macro LOG_SCOPE_F(FATAL, ...)
#define LOGF_SCOPE(...) LOG_SCOPE_F(FATAL, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_SCOPE_F(INFO, ...)
#define DLOGI_SCOPE(...) DLOG_SCOPE_F(INFO, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_SCOPE_F(WARNING, ...)
#define DLOGW_SCOPE(...) DLOG_SCOPE_F(WARNING, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_SCOPE_F(ERROR, ...)
#define DLOGE_SCOPE(...) DLOG_SCOPE_F(ERROR, __VA_ARGS__)

/// Shorthand to the loguru macro DLOG_SCOPE_F(FATAL, ...)
#define DLOGF_SCOPE(...) DLOG_SCOPE_F(FATAL, __VA_ARGS__)

namespace otto {
  struct assert_module : debug_assert::set_level<999> {
    template<typename... Args>
    static void handle(const debug_assert::source_location& loc,
                       const char* expression,
                       Args&&... args) noexcept
    {
      if constexpr (sizeof...(args) > 0) {
        LOGE("Assertion failed at {}:{}: {} {}", loc.file_name, loc.line_number, expression, fmt::format(args...));
      } else {
        LOGE("Assertion failed at {}:{}: {}", loc.file_name, loc.line_number, expression);
      }
    }
  };
} // namespace otto

#define OTTO_ASSERT_NM(Expr) DEBUG_ASSERT(Expr, ::otto::assert_module{})
#define OTTO_ASSERT_M(Expr, ...) DEBUG_ASSERT(Expr, ::otto::assert_module{}, __VA_ARGS__)
#define OTTO_ASSERT(...) SWITCH_FOR_VARARGS_1(OTTO_ASSERT_NM, OTTO_ASSERT_M, __VA_ARGS__)

#define OTTO_UNREACHABLE \
 (DEBUG_UNREACHABLE(::otto::assert_module{}), DEBUG_ASSERT_MARK_UNREACHABLE)
#define OTTO_UNREACHABLE_M(...) (DEBUG_UNREACHABLE(::otto::assert_module{}, __VA_ARGS__), DEBUG_ASSERT_MARK_UNREACHABLE)
