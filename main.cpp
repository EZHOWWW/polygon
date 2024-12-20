// std=c++20
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

template <class Iter> void gnome_sort(Iter first, Iter last) {
  // Gnome sort O(n^2)
  if (first == last)
    return;
  auto cur = first;
  while (cur + 1 != last) {

    if (*cur <= *(cur + 1)) {
      cur++;
    } else {
      std::swap(*cur, *(cur + 1));
      cur--;
    }
  }
}
template <class Iter, class Compare>
void gnome_sort(Iter first, Iter last, Compare comp) {
  // Gnome sort with custom comporator O(n^2)
  if (first == last)
    return;
  auto cur = first;
  while (cur + 1 != last) {

    if (comp(*cur, *(cur + 1)) || *cur == *(cur + 1)) {
      cur++;
    } else {
      std::swap(*cur, *(cur + 1));
      cur--;
    }
  }
}

template <class Iter> void shel_sort(Iter first, Iter last) {
  // shel_sort O(n^2); O(nlong^2 n)
  auto size = std::distance(first, last);
  for (auto d = size / 2; d != 0; d /= 2)
    for (auto i = first + d; i != last; ++i)
      for (auto j = i; j - first >= d && (*j < *(j - d)); j -= d)
        std::swap(*j, *(j - d));
}
template <class Iter, class Compare>
void shel_sort(Iter first, Iter last, Compare comp) {
  auto size = std::distance(first, last);
  for (auto d = size / 2; d != 0; d /= 2)
    for (auto i = first + d; i != last; ++i)
      for (auto j = i; j - first >= d && comp(*j, *(j - d)); j -= d)
        std::swap(*j, *(j - d));
}
template <typename Iter> void counting_sort(Iter first, Iter last, int exp) {
  const int range = 10;
  std::vector<typename std::iterator_traits<Iter>::value_type> output(
      std::distance(first, last));
  std::vector<int> count(range, 0);
  for (Iter it = first; it != last; ++it) {
    count[(*it / exp) % range]++;
  }
  for (int i = 1; i < range; i++) {
    count[i] += count[i - 1];
  }
  for (Iter it = last - 1; it != first - 1; --it) {
    int index = (*it / exp) % range;
    output[count[index] - 1] = *it;
    count[index]--;
  }
  for (auto i = 0; i < output.size(); i++) {
    *(first + i) = output[i];
  }
}

template <class Iter> void radix_sort(Iter first, Iter last) {
  if (first == last)
    return;
  auto max_elem = *std::max_element(first, last);
  for (int exp = 1; max_elem / exp > 0; exp *= 10) {
    counting_sort(first, last, exp);
  }
}
template <typename T> void print_vec(const std::vector<T> &v) {
  for (auto i : v) {
    std::cout << i << '\t';
  }
  std::cout << std::endl;
}
int main(int argc, char *argv[]) {
  using namespace std::chrono;
  srand(time(0));
  void (*sort)(std::vector<int>::iterator, std::vector<int>::iterator) =
      shel_sort<std::vector<int>::iterator>;
  size_t size = 0;
  do {
    std::cout << "write size\n";
    std::cin >> size;
    std::vector<int> vec(size);
    for (auto i = 0; i < size; ++i) {
      vec[i] = rand();
    }
    auto start = high_resolution_clock::now();
    sort(vec.begin(), vec.end());
    auto end = high_resolution_clock::now();

    std::cout << "Time to sort " << size << "element:\t"
              << duration_cast<milliseconds>(end - start).count() << std::endl;

  } while (size != 0);
}
