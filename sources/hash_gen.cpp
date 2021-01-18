// Copyright 2021 Kavykin Andrey Kaviandr@yandex.ru

#include "hash_gen.hpp"

const char end_h[] = "0000";
const size_t end_l = 60;
std::mutex mut;
std::atomic<bool> hash_gen_ = true;
std::atomic<bool> availability_json = false;
const int rotatation_size = 10 * 1024 * 1024;

void hash_gen::init_log() const {
  boost::log::add_common_attributes();
  auto trace_log_f = boost::log::add_file_log(
      boost::log::keywords::file_name =
          "C:\\Users/home/Kavia/CLionProjects/"
          "sem_lab06_multithreads/Logs/LogTrace_%N.log",
      boost::log::keywords::rotation_size = rotatation_size,
      boost::log::keywords::format =
          "[%TimeStamp%][%Severity%][%ThreadID%]: [%Message%]");

  trace_log_f->set_filter(boost::log::trivial::severity ==
                           boost::log::trivial::trace);

  auto info_log_f = boost::log::add_file_log(
      boost::log::keywords::file_name =
          "C:\\Users/home/Kavia/CLionProjects/"
          "sem_lab06_multithreads/Logs/LogInfo_%N.log",
      boost::log::keywords::rotation_size = rotatation_size,
      boost::log::keywords::format =
          "[%TimeStamp%][%Severity%][%ThreadID%]: [%Message%]");
  info_log_f->set_filter(boost::log::trivial::severity ==
                          boost::log::trivial::info);

  auto console_log = boost::log::add_console_log(
      std::cout, boost::log::keywords::format =
                     "[%TimeStamp%][%Severity%][%ThreadID%]: [%Message%]");
  console_log->set_filter(boost::log::trivial::severity >=
                         boost::log::trivial::trace);
}

void hash_gen::save_hash(const std::string &data, const std::string &hash,
                             const std::time_t &timestamp) {
  std::stringstream ss;
  ss << std::uppercase << std::hex << std::stoi(data);
  nlohmann::json js;
  js["timestamp"] = timestamp;
  js["hash"] = hash;
  js["data"] = ss.str();
  std::scoped_lock<std::mutex> lock(mut);
  json.push_back(js);
}

void stop_([[maybe_unused]] int dummy) { hash_gen_ = false; }

void hash_gen::hash_computation() {
  while (hash_gen_) {
    std::string data = std::to_string(std::rand());
    std::string hash = picosha2::hash256_hex_string(data);
    std::time_t timestamp(std::time(nullptr));
    std::string hash_end = hash.substr(end_l);
    if (hash_end == end_h) {
      BOOST_LOG_TRIVIAL(info) << "Hash:" << hash << " Data:" << data;
      if (availability_json) {
        save_hash(data, hash, timestamp);
      }
    } else {
      BOOST_LOG_TRIVIAL(trace) << "Hash:" << hash << " Data:" << data;
    }
  }
}

void hash_gen::find_hash(int &argc, char **argv) {
  size_t size_threads;
  std::string path;
  switch (argc) {
    case 1:
      size_threads = std::thread::hardware_concurrency();
      break;
    case 2:
      size_threads = std::atoi(argv[1]);
      if (size_threads < 1 ||
          size_threads > std::thread::hardware_concurrency()) {
        throw std::out_of_range("Uncorect number threads!");
      }
      break;
    case 3:
      size_threads = std::atoi(argv[1]);
      if (size_threads < 1 ||
          size_threads > std::thread::hardware_concurrency()) {
        throw std::out_of_range("Uncorect number threads!");
      }
      path = argv[2];
      availability_json = true;
      break;
    default:
      throw std::out_of_range("Invalid arguments!");
  }
  std::signal(SIGINT, stop_);
  init_log();
  std::vector<std::thread> threads;
  threads.reserve(size_threads);
  for (size_t i = 0; i < size_threads; ++i) {
    threads.emplace_back(&hash_gen::hash_computation, this);
  }
  for (auto &thread : threads) {
    thread.join();
  }
  if (availability_json) {
    std::ofstream out{path};
    out << json.dump(4);
  }
}
