#include "graph.cpp"
#include <cfloat>
#include <cinttypes>
#include <cmath>
#include <cstddef>
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
  return o << c.lon << ':' << c.lat;
}
template <> struct std::hash<Coordinate> {
  std::size_t operator()(const Coordinate &c) const noexcept {
    auto hlon =
        std::hash<long long>{}(*reinterpret_cast<const long long *>(&c.lon));
    auto hlat=
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

    g.add(node1);
    g.add(node1, node2, dist);
  }
  f.close();
  return g;
}
double get_dist(const Coordinate &a, const Coordinate &b) {
  return std::sqrt(std::pow(a.lat - b.lat, 2) + std::pow(a.lat - b.lat, 2));
}
const std::shared_ptr<SpbGraphNode> &find_nearest(const SpbGraph &g,
                                                  const Coordinate &c) {
  std::shared_ptr<SpbGraphNode> res;
  for (const auto &i : g.get_nodes()) {
    res = i;
    break;
  }
  auto min_dist = std::numeric_limits<double>::infinity();
  int j = 0;
  for (const auto &i : g.get_nodes()) {
    if (i == res)
      continue;
    auto d = get_dist(res->get_val(), i->get_val());
    if (d < min_dist) {
      min_dist = d;
      res = i;
    }
  }
  return *new std::shared_ptr<SpbGraphNode>(res.get());
}

int main() {
  auto g = recognize_graph_file();
  std::cout << g.size() << std::endl;
  ItmoCord = find_nearest(g, {30.296802, 59.943943}); // Биржа :)
  HomeCord = find_nearest(g, {30.499978, 59.937630}); // Белорусская 8
  std::cout << ItmoCord->get_val() << '\t' << HomeCord->get_val() << std::endl;
  g.bfs(ItmoCord);
  g.dfs(ItmoCord);
  g.bfs(HomeCord);
  g.dfs(HomeCord);
}