#ifndef ASTRI_URL_HPP_
#define ASTRI_URL_HPP_

#include <curl/curl.h>
#include <string>

namespace astri {
namespace url {
  std::size_t write_data_to_file(void* data, std::size_t size,
                                 std::size_t nmemb, void* stream);
  bool retrieve(std::string dest, std::string url);
  bool retrieve_once(std::string dest, std::string url);
}  // namespace url
}  // namespace astri

#endif  // ASTRI_URL_HPP_
