#include "graph.cpp"
#include <memory>
#include <string>

using node = Node<std::string, double>;
using graph = Graph<std::string, double>;
int main() {
  graph g;
  auto a = std::make_shared<node>(std::string("aaa"));
  auto b = std::make_shared<node>(std::string("bbb"));
  auto c = std::make_shared<node>(std::string("ccc"));
  auto y = std::make_shared<node>(std::string("yyy"));
  auto j = g.add("ttt");
  auto i = g.add(a);
  g.add(b);
  g.add(a, "eee", 10.5);
  g.add(a, "eee", 10.5);
  g.add(a, "eee", 1.5);

  g.add(a, c, -10.5);

  g.add("111", "222", 1);
  g.add(g.find_node("eee"), g.find_node("111"), 999);
  g.add(i, y, 500);
  g.print();
  std::cout << "\t\t" << std::endl;
  g.bfs(i);
  std::cout << "\t\t" << std::endl;
  g.dfs(i);
  std::cout << "\t\t" << std::endl;
  auto r = g.a_star(i, g.find_node("222"), [](auto, auto) { return 0; });
  for (const auto &val : r) {
    std::cout << val->get_val() << '\t';
  }
  std::cout << std::endl;
}