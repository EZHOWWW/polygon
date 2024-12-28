#include <iostream>
#include <list>
#include <map>
#include <memory>
template <typename V, typename E, size_t id = 0> class _Node {
  V &val;
  std::map<const std::shared_ptr<_Node<V, E, id>> &, E> &related;

public:
  _Node(V val) : val(val), related() {}
  _Node(V val, const std::shared_ptr<_Node<V, E, id>> &relate, E edge_val)
      : _Node(val) {}
  V get_val() { return val; }
  void add(const std::shared_ptr<_Node<V, E, id>> &relate,
           E edge_val) // Добавить ребро как в двунаправленном графе
  {
    related[relate] = edge_val;
    relate->related[*new std::shared_ptr<_Node<V, E, id>>(this)] = edge_val;
  }
  void add(const std::shared_ptr<_Node<V, E, id>> &relate, E edge_val_to_node,
           E edge_val_to_this) // Добавить ребро, ребро от текущего до нового и
                               // от нового до текущего могут быть разными
  {
    related[relate] = edge_val_to_node;
    relate->related[*new std::shared_ptr<_Node<V, E, id>>(this)] =
        edge_val_to_this;
  }
  size_t get_count_related() const { return related.size(); }
  E get_val_to_rel(const std::shared_ptr<_Node<V, E, id>> &rel) {
    return related[rel];
  }
  void print() {
    std::cout << val << ":\n\t";
    for (auto &rel : related) {
      std::cout << rel.second << "::" << rel.first << '\t';
    }
    std::cout << std::endl;
  }
};
template <typename V, typename E> class Graph {
  std::list<const _Node<V, E, 0> &> &nodes;

public:
  Graph() : nodes() {}
  Graph(std::list<const _Node<V, E, 0> &> &nodes) : nodes(nodes()) {}
  Graph(const _Node<V, E, 0> &node) : nodes() { nodes.push_back(node); }

  void print() {
    for (auto &n : nodes) {
      n.print();
    }
  }
};
int main() {}
