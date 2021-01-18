// Copyright 2021 Kavykin Andrey Kaviandr@yandex.ru

#ifndef INCLUDE_HASH_GEN_HPP_
#define INCLUDE_HASH_GEN_HPP_

#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <nlohmann/json.hpp>
#include "picosha2.h"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <mutex>
#include <ctime>
#include <thread>

class hash_gen {
public:
 hash_gen() = default;

    void find_hash(int &argc, char **argv);

private:
    void init_log() const;

    void hash_computation();

    void save_hash(const std::string &data, const std::string &hash,
                  const std::time_t &timestamp);

    nlohmann::json json;
};

#endif //INCLUDE_HASH_GEN_HPP_
