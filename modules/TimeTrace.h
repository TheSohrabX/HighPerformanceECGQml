#pragma once

#include <chrono>
#include <string_view>
#include <type_traits>

#include <QDebug>

template <typename Duration>
struct chrono_suffix
{
    static inline constexpr std::string_view
    value()
    {
        return "?";
    }
};

template <>
struct chrono_suffix<std::chrono::nanoseconds>
{
    static inline constexpr std::string_view
    value()
    {
        return "ns";
    }
};

template <>
struct chrono_suffix<std::chrono::microseconds>
{
    static inline constexpr std::string_view
    value()
    {
        return "us";
    }
};

template <>
struct chrono_suffix<std::chrono::milliseconds>
{
    static inline constexpr std::string_view
    value()
    {
        return "ms";
    }
};

template <>
struct chrono_suffix<std::chrono::seconds>
{
    static inline constexpr std::string_view
    value()
    {
        return "s";
    }
};

template <>
struct chrono_suffix<std::chrono::minutes>
{
    static inline constexpr std::string_view
    value()
    {
        return "min";
    }
};

template <>
struct chrono_suffix<std::chrono::hours>
{
    static inline constexpr std::string_view
    value()
    {
        return "h";
    }
};

template <typename T>
struct is_duration : std::false_type
{};

template <typename Rep, typename Period>
struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type
{};

template <typename T>
inline constexpr bool is_duration_v = is_duration<T>::value;

template <typename T>
concept ChronoDuration = is_duration_v<T>;

template <ChronoDuration TimeRepresentation>
class TimeTrace
{
    using Clock     = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

public:
    TimeTrace()  = default;
    ~TimeTrace() = default;

    void
    start()
    {
        m_startTime = Clock::now();
    }

    void
    end(std::string_view label)
    {
        TimePoint endTime = Clock::now();

        m_totalDuration =
          std::chrono::duration_cast<TimeRepresentation>(endTime - m_startTime)
            .count();

        qDebug().nospace().noquote()
          << "[TimeTrace] " << label << ": " << m_totalDuration
          << chrono_suffix<TimeRepresentation>::value();
    }

    static decltype(auto)
    traceFunctionCall(std::string_view functionName, const auto&& func,
                      const auto&&... args)
    {
        TimeTrace timer;
        timer.start();
        auto res = std::invoke(func, std::forward<decltype(args)>(args)...);
        timer.end(functionName);
        return res;
    }

private:
    TimePoint     m_startTime;
    std::uint64_t m_totalDuration = 0;
};

template <ChronoDuration TimeRepresentation>
class ScopedTimeTrace : public TimeTrace<TimeRepresentation>
{
public:
    ScopedTimeTrace(std::string_view label) :
        m_label(label)
    {
        TimeTrace<TimeRepresentation>::start();
    }

    ~ScopedTimeTrace() { TimeTrace<TimeRepresentation>::end(m_label); }

private:
    std::string_view m_label;
};

using TimeTraceNS       = TimeTrace<std::chrono::nanoseconds>;
using TimeTraceUS       = TimeTrace<std::chrono::microseconds>;
using TimeTraceMS       = TimeTrace<std::chrono::milliseconds>;

using ScopedTimeTraceNS = ScopedTimeTrace<std::chrono::nanoseconds>;
using ScopedTimeTraceUS = ScopedTimeTrace<std::chrono::microseconds>;
using ScopedTimeTraceMS = ScopedTimeTrace<std::chrono::milliseconds>;

template <typename Timer>
inline auto
make_scoped_timer(bool condition, std::string_view info)
{
    if(condition)
    {
        return std::optional<Timer>(info);
    }
    else
    {
        return std::optional<Timer> {};
    }
}

#define TRACE_EXPRESSION_TIME(TRACER, TRACE_NAME, EXPRESSION)                  \
    TRACER TRACE_NAME;                                                         \
    TRACE_NAME.start();                                                        \
    EXPRESSION;                                                                \
    TRACE_NAME.end(Q_FUNC_INFO " -> " #TRACE_NAME);

#define TRACE_EXPRESSION_TIME_MS(TRACE_NAME, EXPRESSION)                       \
    TRACE_EXPRESSION_TIME(TimeTraceMS, TRACE_NAME, EXPRESSION)

#define TRACE_EXPRESSION_TIME_NS(TRACE_NAME, EXPRESSION)                       \
    TRACE_EXPRESSION_TIME(TimeTraceNS, TRACE_NAME, EXPRESSION)

#define TRACE_EXPRESSION_TIME_US(TRACE_NAME, EXPRESSION)                       \
    TRACE_EXPRESSION_TIME(TimeTraceUS, TRACE_NAME, EXPRESSION)


#define TRACE_EXPRESSION_TIME_IF(TRACER, TRACE_NAME, CONDITION, EXPRESSION)    \
    TRACER TRACE_NAME;                                                         \
    TRACE_NAME.start();                                                        \
    EXPRESSION;                                                                \
    if(CONDITION) TRACE_NAME.end(Q_FUNC_INFO " -> " #TRACE_NAME);

#define TRACE_EXPRESSION_TIME_MS_IF(TRACE_NAME, CONDITION, EXPRESSION)         \
    TRACE_EXPRESSION_TIME_IF(TimeTraceMS, TRACE_NAME, CONDITION, EXPRESSION)

#define TRACE_EXPRESSION_TIME_NS_IF(TRACE_NAME, CONDITION, EXPRESSION)         \
    TRACE_EXPRESSION_TIME_IF(TimeTraceNS, TRACE_NAME, CONDITION, EXPRESSION)

#define TRACE_EXPRESSION_TIME_US_IF(TRACE_NAME, CONDITION, EXPRESSION)         \
    TRACE_EXPRESSION_TIME_IF(TimeTraceUS, TRACE_NAME, CONDITION, EXPRESSION)

