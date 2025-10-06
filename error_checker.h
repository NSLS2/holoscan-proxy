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
                                    const std::any &url, Func &&func) {
  try {
    auto result = func();
    if (!result) {
      std::cerr << "Warning " << operation << " failed for the url "
                << std::any_cast<std::string>(url)
                << " . Check the connection\n";
    }
    return result;
  } catch (const zmq::error_t &e) {
    std::cerr << "Warning!! Could not perform the " << operation << " with the "
              << std::any_cast<std::string>(url)
              << ". Error notes: " << e.what() << " err no: " << e.num()
              << std::endl;
  }
}

#endif
