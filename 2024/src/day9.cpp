#include "utils.hpp"
#include <print>

struct file_t {
  int start;
  int size;
  size_t file_id;
};

struct empty_space_t {
  int start;
  int size;
};

constexpr size_t compute_checksum(size_t file_id, int start, int end) {

  return file_id * ((end * (end + 1) - start * (start - 1)) / 2);
}
static_assert(compute_checksum(0, 0, 3) == 0);
static_assert(compute_checksum(1, 0, 3) == 6);
static_assert(compute_checksum(2, 0, 3) == 12);
static_assert(compute_checksum(3, 3, 5) == 3*3 + 3*4 + 3*5);
static_assert(compute_checksum(3, 3, 5) == 3*3 + 3*4 + 3*5);

DPSG_AOC_MAIN(file) {
  std::println("Reading file");
  std::string line;
  std::getline(file, line);

  // files (start, size)
  std::vector<file_t> files;
  // empty spaces (start, size)
  std::vector<empty_space_t> empty_spaces;

  bool is_file = true;
  size_t file_id = 0;
  int vidx = 0;
  for (char c : line) {
    int size = c - '0';

    if (is_file) {
      files.push_back(file_t{vidx, size, file_id});
      file_id++;
    } else {
      empty_spaces.push_back(empty_space_t{vidx, size});
    }
    vidx += size;
    is_file = !is_file;
  }

  size_t checksum = 0;

  size_t idx_last_file = files.size() - 1;
  size_t idx_current_empty_space = 0;

  // we start by computing the empty spaces, we need to calculate the checksum
  // starting from the beginning of the empty spaces, and "filling" it with the
  // files taken from the end of the queue. We stop when the index of the
  // current empty space is greater than the index of the last file.
  while (true) {
    auto &current_empty = empty_spaces[idx_current_empty_space];
    auto &current_file = files[idx_last_file];
    if (current_empty.start > current_file.start) {
      break;
    }

    // If we have enough space to fit the file at the end
    if (current_file.size <= current_empty.size) {
      idx_last_file--;

      checksum += compute_checksum(current_file.file_id, current_empty.start,
                                   current_empty.start + current_file.size - 1);

      current_empty.start += current_file.size;
      current_empty.size -= current_file.size;
      if (current_empty.size == 0) {
        idx_current_empty_space++;
      }
    // We need to split the file
    } else {
      checksum += compute_checksum(current_file.file_id, current_empty.start, current_empty.start + current_empty.size - 1);

      idx_current_empty_space++;
      current_file.size -= current_empty.size;
    }
  }

  for (int i = 0; i <= idx_last_file; ++i) {
    auto &current_file = files[i];
    checksum += compute_checksum(current_file.file_id, current_file.start,
                                 current_file.start + current_file.size - 1);
  }

  std::println("Checksum: {}", checksum);
}
