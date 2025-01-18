#include "graph.cpp"
#include <cfloat>
#include <chrono>
#include <cinttypes>
#include <cmath>
#include <cstddef>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>

class Coordinate {
public:
  double lon;
  double lat;
  bool operator==(const Coordinate &c) const noexcept {
    return lon == c.lon && lat == c.lat;
  }
};
std::ostream &operator<<(std::ostream &o, const Coordinate &c) noexcept {
  return o << std::format("{} : {}", c.lon, c.lat);
}
template <> struct std::hash<Coordinate> {
  std::size_t operator()(const Coordinate &c) const noexcept {
    auto hlon =
        std::hash<long long>{}(*reinterpret_cast<const long long *>(&c.lon));
    auto hlat =
        std::hash<long long>{}(*reinterpret_cast<const long long *>(&c.lat));
    return hlon ^ (hlat << 1);
  }
};
using Distance = double;
using SpbGraph = Graph<Coordinate, Distance>;
using SpbGraphNode = Node<Coordinate, Distance>;
std::shared_ptr<SpbGraphNode> ItmoCord;
std::shared_ptr<SpbGraphNode> HomeCord;
template <> struct std::hash<std::shared_ptr<Node<Coordinate, Distance>>> {
  std::size_t operator()(
      const std::shared_ptr<Node<Coordinate, Distance>> &n) const noexcept {
    return std::hash<Coordinate>{}(n->get_val());
  }
};

SpbGraph &recognize_graph_file(
    std::string path = "/home/dima/m/ITMO/algos/08/polygon/spb_graph.txt") {
  auto &g = *new SpbGraph();
  std::ifstream f(path);
  if (!f.is_open()) {
    std::cerr << "file dont open: " << path << std::endl;
    return g;
  }
  std::string line;
  while (std::getline(f, line, ';')) {
    std::stringstream linestrm(line);
    std::string lonstr, latstr;
    std::getline(linestrm, lonstr, ',');
    std::getline(linestrm, latstr, ':');
    auto cord1 = new Coordinate();
    std::stringstream(lonstr) >> cord1->lon;
    std::stringstream(latstr) >> cord1->lat;
    std::getline(linestrm, lonstr, ',');
    std::getline(linestrm, latstr, ',');
    auto cord2 = new Coordinate();
    std::stringstream(lonstr) >> cord2->lon;
    std::stringstream(latstr) >> cord2->lat;
    std::string diststr;
    Distance dist;
    std::getline(linestrm, diststr, ';');
    std::stringstream(diststr) >> dist;
    auto node1 = *new std::shared_ptr<SpbGraphNode>(new SpbGraphNode(*cord1));
    auto node2 = *new std::shared_ptr<SpbGraphNode>(new SpbGraphNode(*cord2));

    g.add(*cord1, *cord2, dist);
  }
  f.close();
  return g;
}
double get_dist(const Coordinate &a, const Coordinate &b) {
  return std::sqrt(std::pow(a.lat - b.lat, 2) + std::pow(a.lat - b.lat, 2));
}
double dist(const std::shared_ptr<SpbGraphNode> &a,
            const std::shared_ptr<SpbGraphNode> &b) {
  return get_dist(a->get_val(), b->get_val());
}
const std::shared_ptr<SpbGraphNode> &find_nearest(const SpbGraph &g,
                                                  const Coordinate &c) {
  std::shared_ptr<SpbGraphNode> res(nullptr);

  auto min_dist = std::numeric_limits<double>::infinity();
  for (const auto &n : g.get_nodes()) {
    auto dist = get_dist(c, n->get_val());
    if (dist < min_dist) {
      min_dist = dist;
      res = n;
    }
  }
  return *new std::shared_ptr<SpbGraphNode>(res);
}
void test_bfs(const SpbGraph &g, const std::shared_ptr<SpbGraphNode> &start,
              const std::shared_ptr<SpbGraphNode> &end) {
  auto start_time = std::chrono::steady_clock::now();
  auto res = g.bfs_find_path(start, end);
  auto end_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end_time - start_time;
  std::cout << std::format("bfs time: {} miliseconds\nbfs result: {}",
                           elapsed.count() * 1000, res)
  << std::endl;
}
void test_dfs(const SpbGraph &g, const std::shared_ptr<SpbGraphNode> &start,
              const std::shared_ptr<SpbGraphNode> &end) {
  auto start_time = std::chrono::steady_clock::now();
  auto res = g.dfs_find_path(start, end);
  auto end_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end_time - start_time;
  std::cout << std::format("dfs time: {} miliseconds\nbfs result: {}",
                           elapsed.count() * 1000, res)
            << std::endl;
}
void test_a_star(const SpbGraph &g, const std::shared_ptr<SpbGraphNode> &start,
                 const std::shared_ptr<SpbGraphNode> &end) {
  auto start_time = std::chrono::steady_clock::now();
  auto vec_res = g.a_star_find_path(start, end, &dist);

  auto end_time = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed = end_time - start_time;
  std::cout << std::format("a_star time: {} miliseconds\na_star result: {}",
                           elapsed.count() * 1000, 0)
            << std::endl;
}

int main() {
  auto g = recognize_graph_file();
  std::cout << g.size() << std::endl;
  ItmoCord = find_nearest(g, {30.296802, 59.943943}); // Биржа :)
  HomeCord = find_nearest(g, {30.499978, 59.937630}); // Белорусская 8
  std::cout << ItmoCord->get_val() << '\t' << HomeCord->get_val() << std::endl;
  std::cout << (ItmoCord == HomeCord) << std::endl;
  test_bfs(g, ItmoCord, HomeCord);
  test_dfs(g, HomeCord, ItmoCord);
  test_a_star(g, HomeCord, ItmoCord);
}