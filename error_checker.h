#ifndef ERROR_CHECKER_H
#define ERROR_CHECKER_H

// connect(), bind(), set() functions return void or throw exception
// send(), recv() functions return bool or sometimes throw exception
template <typename Func>
void LOG_SOCKOUT_VOID(const std::string &operation, const std::any &url,
                      Func &&func) {
  try {
    func(url);
  } catch (const zmq::error_t &e) {
    std::cerr << "Warning!! Could not perform the " << operation << " with the "
              << std::any_cast<std::string>(url)
              << ". Error notes: " << e.what() << " err no: " << e.num()
              << std::endl;
  }
}

template <typename Func>
[[nodiscard]] auto LOG_SOCKOUT_BOOL(const std::string &operation,
                                    const std::string &url, Func &&func) {

  using ResultType = decltype(func()); // Get the return type of the lambda
  ResultType result;

  try {
    result = func();
    if constexpr (std::is_same_v<ResultType, bool>) {
      if (!result) {
        std::cerr << "Warning 1: " << operation << " failed for " << url
                  << "\n";
      }
    } else if constexpr (std::is_same_v<ResultType, std::optional<size_t>>) {
      if (!result.has_value()) {
        std::cerr << "Warning 2: " << operation << " failed for " << url
                  << "\n";
      }
    } else {
      std::cerr << "Unsupported return type for LOG_SOCKOUT_LAMBDA\n";
    }
    return result;
  } catch (const zmq::error_t &e) {
    std::cerr << "Warning!! Could not perform the " << operation << " with the "
              << url << ". Error notes: " << e.what() << " err no: " << e.num()
              << std::endl;

    return ResultType{};
  }
}

#endif
