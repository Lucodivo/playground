//
// Created by Connor on 3/5/2022.
//

#include <chrono>

struct Timer {
  std::chrono::steady_clock::time_point prev;
  f64 delta;
};
void readFile(const char* filePath, std::vector<char>& fileBytes);

// Returns time in milliseconds
void StartTimer(Timer& timer);
f64 StopTimer(Timer& timer);

void readFile(const char* filePath, std::vector<char>& fileBytes) {
  //opens the file. With cursor at the end
  std::ifstream file(filePath, std::ios::ate | std::ios::binary);

  if(!file.is_open()) {
    std::cout << "Could not open file: " << filePath << std::endl;
    return;
  }

  //find what the size of the file is by looking up the location of the cursor
  s64 fileSize = file.tellg();

  fileBytes.clear();
  fileBytes.resize(fileSize);

  //put file cursor at beginning
  file.seekg(0);
  file.read((char*)fileBytes.data(), fileSize);
  file.close();
}

void StartTimer(Timer& timer) {
  timer.prev = std::chrono::high_resolution_clock::now();
  timer.delta = 0.0;
}

f64 StopTimer(Timer& timer) {
  std::chrono::steady_clock::time_point prevPrev = timer.prev;
  timer.prev = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> dur = timer.prev - prevPrev;
  timer.delta = dur.count();
  return timer.delta;
}