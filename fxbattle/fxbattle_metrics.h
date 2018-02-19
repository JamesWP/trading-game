#pragma once

#include <commonpp/core/Utils.hpp>
#include <commonpp/metric/Metrics.hpp>
#include <commonpp/metric/sink/Console.hpp>
#include <commonpp/metric/sink/Graphite.hpp>
#include <commonpp/metric/sink/InfluxDB.hpp>
#include <commonpp/metric/type/TimeScope.hpp>
#include <commonpp/metric/reservoir/ExponentiallyDecaying.hpp>
#include "commonpp/core/LoggingInterface.hpp"

using commonpp::metric::Metrics;
using commonpp::metric::MetricTag;
using commonpp::metric::MetricValue;
using commonpp::metric::sink::Console;
using commonpp::metric::sink::Graphite;
using commonpp::metric::sink::InfluxDB;
using commonpp::thread::ThreadPool;

using commonpp::metric::reservoir::ExponentiallyDecaying;

using Counter = Metrics::Counter<>;
using DescStat = Metrics::DescStat;
using Gauge = Metrics::Gauge<>;
using TimeScope = commonpp::metric::type::TimeScope<ExponentiallyDecaying<>,
                                                    std::chrono::nanoseconds>;

namespace fxbattle {
class FxMetrics {
public:
  using scope_timer = TimeScope;
  using metric_name = ExponentiallyDecaying<>;
private:
  ThreadPool _pool{1};
  
  Metrics _metrics{_pool, std::chrono::seconds(5)};

  MetricTag _prefix{"FXBattle"};

  Console _sink;
  Graphite _gsink{_pool.getService(), _prefix, "127.0.0.1", "2003"};

  MetricTag _tag;
public:
  FxMetrics(const std::string &nsname) : _tag(nsname) {}

  void start()
  {
    commonpp::core::init_logging();
    commonpp::core::enable_console_logging();
    commonpp::core::enable_builtin_syslog();

    _pool.start();

    _metrics.addMetricsReceiver(std::ref(_sink));
    _metrics.addMetricsReceiver(std::ref(_gsink));
  }

  void stop() { _pool.stop(); }
  
  void register_metric(metric_name& m, const std::string& name)
  {
    _metrics.add<DescStat>(_tag(name), m);
  }

  template <typename T>
  void register_metric(const std::string &name, T fn)
  {
    _metrics.add(_tag(name), Counter(fn, name));
  }
};
}
