#include "url.hpp"

#include <sys/stat.h>
#include <cstdio>

#include <curl/curl.h>

#include "logger.hpp"

std::size_t astri::url::write_data_to_file(void* data, std::size_t size,
                                           std::size_t nmemb, void* stream) {
  std::size_t written = std::fwrite(data, size, nmemb, (FILE*)stream);
  return written;
}

bool astri::url::retrieve(std::string dest, std::string url) {
  CURL* curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_file);
  FILE* out = std::fopen(dest.c_str(), "wb");
  if (out) {
    Info("cURL \"%s\"", url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, out);
    curl_easy_perform(curl);
    fclose(out);
  } else {
    curl_easy_cleanup(curl);
    return false;
  }
  curl_easy_cleanup(curl);
  return true;
}

bool astri::url::retrieve_once(std::string dest, std::string url) {
  struct stat buffer;
  if (stat(dest.c_str(), &buffer) == 0) {
    return false;
  }
  return retrieve(dest, url);
}
