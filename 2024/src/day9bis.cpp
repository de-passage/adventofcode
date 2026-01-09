#include "utils.hpp"
#include "print.hpp"

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


  // In this version we can simply "move" the files in the first empty space
  // that we find that is big enough. Moving is simply changing the beginning
  // index.
  for (int i = files.size() - 1; i >= 0; --i) {
    auto &current_file = files[i];

    for (auto& e : empty_spaces) {
      if (e.start > current_file.start) { break; }
      if (e.size >= current_file.size) {
        current_file.start = e.start;
        e.size -= current_file.size;
        e.start += current_file.size;
        break;
      }
    }

  }

  for (int i = 0; i <= files.size(); ++i) {
    auto &current_file = files[i];
    checksum += compute_checksum(current_file.file_id, current_file.start,
                                 current_file.start + current_file.size - 1);
  }

  std::println("Checksum: {}", checksum);
}
