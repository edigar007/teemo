/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/
#include "target_file.h"
#include "file_util.h"
#include <assert.h>
#include "options.h"
#include "md5.h"
#include "crc32.h"
#include "sha1.h"
#include "sha256.h"
#include "filesystem.hpp"

namespace teemo {

TargetFile::TargetFile(const utf8string& file_path)
    : file_path_(file_path), f_(nullptr), fixed_size_(0L), file_seek_pos_(0L) {}

TargetFile::~TargetFile() {
  close();
}

bool TargetFile::createNew(int64_t fixed_size) {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);
  assert(f_ == nullptr);
  if (f_)
    return false;

  if (fixed_size < 0)
    fixed_size = 0;

  if (!FileUtil::CreateFixedSizeFile(file_path_, fixed_size))
    return false;
  f_ = FileUtil::Open(file_path_, "rb+");
  if (!f_)
    return false;

  if (f_) {
    file_seek_pos_ = 0L;
    FileUtil::Seek(f_, 0L, SEEK_SET);
  }

  return (f_ != nullptr);
}

bool TargetFile::open() {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);
  assert(f_ == nullptr);
  if (f_)
    return false;

  f_ = FileUtil::Open(file_path_, "rb+");
  if (f_) {
    file_seek_pos_ = 0L;
    FileUtil::Seek(f_, 0L, SEEK_SET);
  }
  return (f_ != nullptr);
}

void TargetFile::close() {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);
  if (f_) {
    fflush(f_);
    FileUtil::Close(f_);
    f_ = nullptr;
  }
}

bool TargetFile::renameTo(Options* opt,
                          const utf8string& new_file_path,
                          bool need_reopen) {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);
  bool reopen = false;
  if (isOpened()) {
    close();
    reopen = need_reopen;
  }

  bool ret = FileUtil::Rename(file_path_, new_file_path);

  if (reopen && open()) {
    FileUtil::Seek(f_, file_seek_pos_, SEEK_SET);
  }

  return ret;
}

Result TargetFile::calculateFileHash(Options* opt, utf8string& str_hash) {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);

  Result ret = CALCULATE_HASH_FAILED;
  if (opt->hash_type == MD5) {
    ret = f_ ? CalculateFileMd5(f_, opt, str_hash)
             : CalculateFileMd5(file_path_, opt, str_hash);
  }
  else if (opt->hash_type == CRC32) {
    ret = f_ ? CalculateFileCRC32(f_, opt, str_hash)
             : CalculateFileCRC32(file_path_, opt, str_hash);
  }
  else if (opt->hash_type == SHA1) {
    ret = f_ ? CalculateFileSHA1(f_, opt, str_hash)
             : CalculateFileSHA1(file_path_, opt, str_hash);
  }
  else if (opt->hash_type == SHA256) {
    ret = f_ ? CalculateFileSHA256(f_, opt, str_hash)
             : CalculateFileSHA256(file_path_, opt, str_hash);
  }
  return ret;
}

Result TargetFile::calculateFileMd5(Options* opt, utf8string& str_hash) {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);
  Result ret = CALCULATE_HASH_FAILED;

  ret = f_ ? CalculateFileMd5(f_, opt, str_hash)
           : CalculateFileMd5(file_path_, opt, str_hash);

  return ret;
}

int64_t TargetFile::fileSize() {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);
  int64_t ret = 0L;
  if (isOpened()) {
    ret = FileUtil::GetFileSize(f_);
    FileUtil::Seek(f_, file_seek_pos_, SEEK_SET);
  }
  else {
    ret = FileUtil::GetFileSize(file_path_);
  }
  return ret;
}

int64_t TargetFile::write(int64_t pos, const void* data, int64_t data_size) {
  std::lock_guard<std::recursive_mutex> lg(file_mutex_);
  assert(f_);
  int64_t written = 0L;
  do {
    if (!f_)
      break;
    if (!data || data_size == 0)
      break;
    if (pos < 0)
      break;

    if (file_seek_pos_ != pos) {
      if (FileUtil::Seek(f_, pos, SEEK_SET) != 0) {
        assert(false);
        break;
      }
      file_seek_pos_ = pos;
    }

    written = fwrite(data, 1, (long)data_size, f_);
    assert(written == data_size);
    fflush(f_);
    file_seek_pos_ += written;
  } while (false);

  return written;
}

utf8string TargetFile::filePath() const {
  return file_path_;
}

int64_t TargetFile::fixedSize() const {
  return fixed_size_;
}

bool TargetFile::isOpened() const {
  return f_ != nullptr;
}

}  // namespace teemo