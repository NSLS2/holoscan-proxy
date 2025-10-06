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
    std::cerr << "Error!! Could not perform the " << operation << " with the "
              << std::any_cast<std::string>(url)
              << ". Error notes: " << e.what() << " err no: " << e.num()
              << std::endl;
  }
}

template <typename Func>
bool LOG_SOCKOUT_BOOL(const std::string &operation, Func &&func) {
  try {
    bool result = func();
    if (!result) {
      std::cerr << "Warning " << operation << "failed\n";
    }
    return result;
  } catch (const zmq::error_t &e) {
    std::cerr << "Error " << operation << e.what() << " err no: " << e.num()
              << std::endl;
  }
}

#endif
