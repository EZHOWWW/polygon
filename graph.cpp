// -std=c++20
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

template <typename V, typename E> class Graph;
template <typename V, typename E> class Node {
  using NPoint = std::shared_ptr<Node>;
  const V &val;
  std::unordered_map<NPoint, const E &> &related;

public:
  Node(const V &val)
      : val(val), related(*new std::unordered_map<NPoint, const E &>()) {}
  Node(const V &val, std::unordered_map<NPoint, const E &> &related)
      : val(val), related(related) {}
  Node(const V &val, Node &node, const E &edge, bool add_to, bool add_from)
      : Node(val) {
    add(node, edge, add_to, add_from);
  }
  Node(const V &val, Node &node, const E &edge_to, const E &edge_from)
      : Node(val) {
    add(node, edge_to, edge_from);
  }
  // {add_to=true, add_from = true}: добавить
  // как в односвязном графе (если нужно что бы
  // веса туда и обратно были одинаковыми)
  // {add_to=false, add_from=true}: добавить ребро
  // только от node {add_to=true, add_from=false}:
  // добавить ребро только к node {add_to = false,
  // add_from = false}: ничего не добавлять
  void add(Node &node, const E &edge, bool add_to = true,
           bool add_from = true) {
    if (add_to) {
      NPoint np(&node);
      related.insert({np, edge});
    }
    if (add_from)
      node.related.insert({NPoint(this), edge});
  }
  // добавить как в двусвязном графе(если нужно
  // что бы веса туда и обратно были разными)
  void add(Node &node, const E &edge_to, const E &edge_from) {
    NPoint np(&node);
    related.insert({np, edge_to});
    node.related.insert({NPoint(this), edge_from});
  }
  // Эти две функции нужно для возможности реализации разных графов

  void print() const {
    std::cout << val << ":\n\t";
    for (const auto &[node, edge] : related) {
      std::cout << node->val << ':' << edge << '\t';
    }
    std::cout << std::endl;
  }
  const V &get_val() const { return val; }
  bool operator<(const Node<V, E> &n) const { return val < n.val; }
};

template <typename V, typename E> struct NodeHash {
  std::size_t operator()(Node<V, E> *n) const noexcept {
    return std::hash<V>{}(n->get_val());
  }
};
template <typename V, typename E>
bool operator==(const Node<V, E> &a, const Node<V, E> &b) {
  return a == b;
}
template <typename V, typename E> class Graph {
  using Nd = Node<V, E>;
  using NdPoint = std::shared_ptr<Nd>;

public:
  std::unordered_set<Nd *, NodeHash<V, E>> &nodes;
  Graph() : nodes(*new std::unordered_set<Nd *, NodeHash<V, E>>()) {}
  Graph(std::unordered_set<Nd *, NodeHash<V, E>> &nodes) : nodes(nodes) {}
  void add(Nd &node) { nodes.insert(&node); }
  void add(Nd &from, Nd &n, const E &edge, bool add_to = true,
           bool add_from = true) {
    if (nodes.contains(&from)) {
      nodes.insert(&n);
      from.add(n, edge, add_to, add_from);
    }
  }

  void print() const {
    for (const auto &n : nodes) {
      n->print();
    }
  }
};

using graph = Graph<std::string, double>;
using node = Node<std::string, double>;
int main() {

  graph g;
  auto a = *new node("aaa");
  auto d = *new node("ddd");
  g.add(a);
  g.add(a, d, 15);
  g.print();
  g.add(*new node("ggg"), d, 10);
  g.add(a, *new node("eee"), 100);
  std::cout << "\n\n";
  g.print();
}
