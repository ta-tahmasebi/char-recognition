#include "download.h"

#include <zlib.h>
#include <cstdio>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <curl/curl.h>
#include <curl/easy.h>

#include "indicators.h"
#include "color.h"

int download_progress_callback(void *clientp, curl_off_t dltotal,
                               curl_off_t dlnow, curl_off_t ultotal,
                               curl_off_t ulnow) {
    auto *progress_bar = static_cast<indicators::ProgressBar *>(clientp);

    if (progress_bar->is_completed()) { ;
    } else if (dltotal == 0) {
        progress_bar->set_progress(0);
    } else {
        int percentage = static_cast<float>(dlnow) / static_cast<float>(dltotal) * 100;
        progress_bar->set_progress(percentage);
    }
    return 0;
}

std::string extract_file_name(const std::string &url) {
    int i = url.size();
    for (; i >= 0; i--)
        if (url[i] == '/')
            break;
    return url.substr(i + 1, url.size() - 1);
}

bool download(const std::string &url, const std::string &file_path) {
    const std::string file_name = extract_file_name(url);
    indicators::show_console_cursor(false);
    indicators::ProgressBar progress_bar{
            indicators::option::BarWidth{30}, indicators::option::Start{" ["},
            indicators::option::Fill{"█"}, indicators::option::Lead{"█"},
            indicators::option::Remainder{"-"}, indicators::option::End{"]"},
            indicators::option::PrefixText{file_name},
            indicators::option::ShowElapsedTime{true},
            indicators::option::ShowRemainingTime{true},
    };

    CURL *curl;
    FILE *fp;
    CURLcode res{};
    curl = curl_easy_init();
    if (curl) {
        fp = fopen(file_path.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, download_progress_callback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, static_cast<void *>(&progress_bar));
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    indicators::show_console_cursor(true);

    if (res == CURLE_OK) { return true; }
    else { return false; }
}

bool gunzip_file(const std::string &gz_path, const std::string &out_path) {
    gzFile infile = gzopen(gz_path.c_str(), "rb");
    if (!infile) {
        std::cerr << "Cannot open gzip file: " << gz_path << std::endl;
        return false;
    }

    std::ofstream outfile(out_path, std::ios::binary);
    if (!outfile) {
        gzclose(infile);
        std::cerr << "Cannot create output file: " << out_path << std::endl;
        return false;
    }

    const size_t buffer_size = 8192;
    std::vector<char> buffer(buffer_size);

    int bytes_read;
    while ((bytes_read = gzread(infile, buffer.data(), buffer_size)) > 0) {
        outfile.write(buffer.data(), bytes_read);
    }

    gzclose(infile);
    outfile.close();
    return true;
}

bool downloadProcess() {
    const std::string data_dir = "data";
    if (!std::filesystem::exists(data_dir)) {
        if (std::filesystem::create_directory(data_dir)) {
        } else {
            std::cerr << "Failed to create directory: " << data_dir << std::endl;
            return false;
        }
    }
    struct MNISTFile {
        std::string url;
        std::string name;
    };

    std::vector<MNISTFile> files = {
            {"https://raw.githubusercontent.com/fgnt/mnist/master/train-images-idx3-ubyte.gz", "train-images"},
            {"https://raw.githubusercontent.com/fgnt/mnist/master/train-labels-idx1-ubyte.gz", "train-labels"},
            {"https://raw.githubusercontent.com/fgnt/mnist/master/t10k-images-idx3-ubyte.gz",  "test-images"},
            {"https://raw.githubusercontent.com/fgnt/mnist/master/t10k-labels-idx1-ubyte.gz",  "test-labels"}
    };

    for (auto &file: files) {
        std::string gz_name = file.name + ".gz";

        if (!download(file.url, gz_name)) {
            std::cerr << "✗ Download failed for: " << file.url << std::endl;
            return false;
        }

        if (!gunzip_file(gz_name, file.name)) {
            std::cerr << "✗ Failed to unzip: " << gz_name << std::endl;
            return false;
        }
        std::filesystem::rename(file.name, std::filesystem::path("data") / file.name);
        std::remove(gz_name.c_str());
    }
    colorPrint::println("Download completed.", colorPrint::Color::GREEN);
    return true;
}