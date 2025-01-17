// -std=c++20
// В общем я очень много времени потратил на то что бы нормально реализовать
// граф, что бы было минимально копирований и тп. Зато как по мне получилась
// хорошая работоющая быстрая универсальная реализация графа.
#include <algorithm>
#include <cmath> // Для A* (евклидового расстояния или аналогичного эвристики)
#include <functional>
#include <iostream>
#include <limits> // Для определения бесконечности
#include <memory>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>

template <typename V, typename E> class Graph;

template <typename V, typename E> class Node {
  using NdPoint = std::shared_ptr<Node>;
  const V &val;

  std::unordered_map<NdPoint, E> related;

public:
  Node() = delete;
  Node(const V &val) : val(val), related() {}
  Node(const V &val, const std::unordered_map<NdPoint, const E &> &related)
      : val(val), related(related) {}
  void add(const NdPoint &node, const E &edge, bool add_to = true,
           bool add_from = true) {
    if (add_to) {
      related.insert_or_assign(node, edge);
    }
    if (add_from) {
      node->related.insert_or_assign(*new NdPoint(this), edge);
    }
  }
  void add(const NdPoint &node, const E &edge_to, const E &edge_from) {
    related.insert_or_assign(node, edge_to);
    node->related.insert_or_assign(*new NdPoint(this), edge_from);
  }

  void print() const {
    std::cout << val << ":\n\t";
    for (const auto &[node, edge] : related) {
      std::cout << node.use_count() << ' ' << node->val << ':' << edge << '\t';
    }
    std::cout << std::endl;
  }
  const V &get_val() const { return val; }
  bool operator==(const Node<V, E> &n) const noexcept { return val == n.val; }
  friend Graph<V, E>;
};

template <typename V, typename E> struct std::hash<Node<V, E>> {
  std::size_t operator()(const Node<V, E> &n) const noexcept {
    return std::hash<V>{}(n.get_val());
  }
};
template <typename V, typename E>
struct std::hash<std::shared_ptr<Node<V, E>>> {
  std::size_t operator()(const std::shared_ptr<Node<V, E>> &n) const noexcept {
    return std::hash<V>{}(n->get_val());
  }
};
template <typename V, typename E> struct std::equal_to<Node<V, E>> {
  bool operator()(const Node<V, E> &a, const Node<V, E> &b) const noexcept {
    return a.get_val() == b.get_val();
  }
};
template <typename V, typename E>
struct std::equal_to<std::shared_ptr<Node<V, E>>> {
  using NdPoint = std::shared_ptr<Node<V, E>>;
  bool operator()(const NdPoint &a, const NdPoint &b) const noexcept {
    return a->get_val() == b->get_val();
  }
};

template <typename V, typename E> class Graph {
  using Node = Node<V, E>;
  using NdPoint = std::shared_ptr<Node>;
  std::unordered_set<NdPoint> nodes;

public:
  const std::unordered_set<NdPoint> &get_nodes() const noexcept {
    return nodes;
  }
  Graph() = default;
  Graph(const std::unordered_set<NdPoint> nodes) : nodes(nodes) {}

  const NdPoint &add(const V &node_val) {
    return add(std::make_shared<Node>(node_val));
  }
  const NdPoint &add(const NdPoint &node) {
    nodes.insert(node);
    return node;
  }

  void add(const NdPoint &node_in_g, const V &new_node_val, const E &edge,
           bool add_to = true, bool add_from = true) {
    auto new_node = *new std::shared_ptr<Node>(add(new_node_val).get());
    add(node_in_g, new_node, edge, add_to, add_from);
  }
  void add(const NdPoint &node_in_g, const NdPoint &new_node, const E &edge,
           bool add_to = true, bool add_from = true) {
    auto nd = *new std::shared_ptr<Node>(add(new_node).get());
    node_in_g->add(nd, edge, add_to, add_from);
  }
  void add(const V &v_node1, const V &v_node2, const E &edge) {
    auto new_node1 = *new std::shared_ptr<Node>(add(v_node1).get());
    add(new_node1, v_node2, edge);
  }

  const NdPoint &find_node(const V &node_val) {
    auto p = std::make_shared<Node>(node_val);
    return *nodes.find(p);
  }

  void print() const {
    for (const auto &node : nodes) {
      node->print();
    }
  }
  std::size_t size() { return nodes.size(); }
  void dfs(const NdPoint &start) {
    std::unordered_set<NdPoint> visited;
    std::stack<NdPoint> stack;

    stack.push(start);

    while (!stack.empty()) {
      auto node = stack.top();
      stack.pop();

      if (visited.find(node) != visited.end()) {
        continue;
      }
      visited.insert(node);

      // Обрабатываем текущую вершину
      std::cout << "Visited: " << node->related.size() << "  "
                << node->get_val() << std::endl;

      // Проходим по соседям
      for (const auto &[neighbor, edge] : node->related) {
        if (visited.find(neighbor) == visited.end()) {
          stack.push(neighbor);
        }
      }
    }
  }

  // BFS (поиск в ширину)
  void bfs(const NdPoint &start) {
    std::unordered_set<NdPoint> visited;
    std::queue<NdPoint> queue;

    queue.push(start);
    visited.insert(start);

    while (!queue.empty()) {
      auto node = queue.front();
      queue.pop();

      // Обрабатываем текущую вершину
      std::cout << "Visited: " << node->related.size() << "  "
                << node->get_val() << std::endl;

      // Проходим по соседям
      for (const auto &[neighbor, edge] : node->related) {
        if (visited.find(neighbor) == visited.end()) {
          queue.push(neighbor);
          visited.insert(neighbor);
        }
      }
    }
  }
  std::vector<NdPoint>
  a_star(const NdPoint &start, const NdPoint &goal,
         std::function<double(const NdPoint &, const NdPoint &)> heuristic) {
    using Node = typename Graph<V, E>::NdPoint;

    // Очередь с приоритетом для открытых вершин
    struct PriorityQueueElement {
      Node node;
      double priority;

      bool operator<(const PriorityQueueElement &other) const {
        return priority > other.priority; // Меньший приоритет в начало
      }
    };

    std::priority_queue<PriorityQueueElement> open_set;

    // G-стоимость: минимальная стоимость пути из start до вершины
    std::unordered_map<Node, double> g_cost;
    g_cost[start] = 0.0;

    // F-стоимость: G + эвристика
    std::unordered_map<Node, double> f_cost;
    f_cost[start] = heuristic(start, goal);

    // Предшественники (для восстановления пути)
    std::unordered_map<Node, Node> came_from;

    // Установим стартовую вершину в очередь
    open_set.push({start, f_cost[start]});
    std::unordered_set<Node> closed_set; // Набор обработанных вершин

    while (!open_set.empty()) {
      // Извлекаем вершину с наименьшей F-стоимостью
      Node current = open_set.top().node;
      open_set.pop();

      // Если достигли целевой вершины, восстанавливаем путь
      if (current == goal) {
        std::vector<Node> path;
        for (Node at = goal; at != nullptr; at = came_from[at]) {
          path.push_back(at);
        }
        std::reverse(path.begin(), path.end());
        return path;
      }

      // Переносим текущую вершину в множество закрытых
      closed_set.insert(current);

      // Просматриваем соседей текущей вершины
      for (auto &neighbor : current->related) {
        if (closed_set.find(neighbor.first) != closed_set.end()) {
          // Пропускаем уже обработанные вершины
          continue;
        }

        double tentative_g_cost =
            g_cost[current] + neighbor.second; // G + стоимость ребра

        if (g_cost.find(neighbor.first) == g_cost.end() ||
            tentative_g_cost < g_cost[neighbor.first]) {
          // Если нашли лучший путь к соседу
          came_from[neighbor.first] = current;
          g_cost[neighbor.first] = tentative_g_cost;
          f_cost[neighbor.first] =
              g_cost[neighbor.first] + heuristic(neighbor.first, goal);

          open_set.push({neighbor.first, f_cost[neighbor.first]});
        }
      }
    }

    // Если путь не найден
    return {};
  }
};

// Классический A* (поиск пути)