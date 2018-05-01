#pragma once

#include <vector>
#include <queue>
#include <set>


template<class VDATA, class EDATA>
class Graph {

private:

    template<class T>
    class Iterator {
    public:
        virtual bool has_next() = 0;

        virtual void next() = 0;

        virtual T *current() = 0;
    };

    template<class ELEM>
    class ArrayIterator : public Iterator<ELEM> {
    private:
        int pos;
        std::vector<ELEM *> *array;
    public:
        ArrayIterator(std::vector<ELEM *> *p) {
            pos = 0;
            this->array = p;
        }

        bool has_next() { return pos < array->size(); }

        void next() { pos++; }

        ELEM *current() { return has_next() ? (*array)[pos] : 0; }
    };

    template<class ELEM>
    class QueueIterator : public Iterator<ELEM> {
    protected:
        struct Token {
            int priority;
            ELEM *elem;

            Token(int p, ELEM *e) {
                priority = p;
                elem = e;
            }

            bool operator<(const Token &o) const { return priority > o.priority; }
        };

        std::priority_queue<Token> queue;
    public:
        bool has_next() { return queue.size() > 0; }

        void next() { queue.pop(); }

        ELEM *current() { return has_next() ? queue.top().elem : 0; }

        void push(int prio, ELEM *elem) { queue.push(Token(prio, elem)); }
    };

public:
    class Edge;

    class Vertex {
    private:
        VDATA value;
        int index;
        std::vector<Edge *> edges;

    public:
        Vertex(VDATA value, int index) {
            this->value = value;
            this->index = index;
        }

        int get_degree() { return edges.size(); }

        int get_index() { return index; }

        VDATA &get_value() { return value; }

        void add_neighbor(Edge *neighbor) { edges.push_back(neighbor); }

        Edge *get_edge(int i) { return edges[i]; }

        ArrayIterator<Edge> neighbor_iterator() { return ArrayIterator<Edge>(&edges); }
    };

    class VertexIterator : Iterator<Vertex> {
    };

    class Edge {
    private:
        EDATA weight;
        Vertex *source;
        Vertex *destination;
        bool directed;
    public:
        Edge(EDATA weight, Vertex *source, Vertex *destination, bool directed = false) {
            this->weight = weight;
            this->source = source;
            this->destination = destination;
            this->directed = directed;
        }

        EDATA get_weight() { return weight; }

        Vertex *get_source() { return source; }

        Vertex *get_destination() { return destination; }

        Vertex *get_destination(Vertex *source) {
            if (source == this->source)
                return destination;
            if (!directed && source == this->destination)
                return this->source;
            return 0;
        }
    };

    class EdgeIterator : Iterator<Edge> {
    };

private:


    class DepthFirstIterator : public QueueIterator<Vertex> {
    private:
        std::set<int> visited;
        int counter;

        void visit(Vertex *v) {
            int index = v->get_index();
            if (visited.count(index) > 0) return;
            this->push(counter++, v);
            visited.insert(index);
            for (auto ne = v->neighbor_iterator(); ne.has_next(); ne.next()) {
                Edge *edge = ne.current();
                Vertex *neighbor = edge->get_destination(v);
                visit(neighbor);
            }
        }

    public:
        DepthFirstIterator(Vertex *start) {
            counter = 0;
            visit(start);
        }
    };

    class BreadthFirstIterator : public QueueIterator<Vertex> {
    private:
        std::set<unsigned> visited;

    public:
        BreadthFirstIterator(Vertex *start) {
            this->push(0, start);
            visited.insert(start->get_index());
        }

        void next() {
            if (this->queue.size() == 0) return;

            auto token = this->queue.top();
            Vertex *v = token.elem;
            this->queue.pop();

            unsigned depth = token.priority + 1;
            for (auto ne = v->neighbor_iterator(); ne.has_next(); ne.next()) {
                Edge *edge = ne.current();
                Vertex *neighbor = edge->get_destination(v);
                unsigned index = neighbor->get_index();
                if (visited.count(index) == 0) {
                    this->push(depth, neighbor);
                    visited.insert(index);
                }

            }
        }
    };

    // global graph attributes
    std::vector<Vertex *> vertices;
    std::vector<Edge *> edges;
    bool directed;

public:

    ArrayIterator<Vertex> vertex_iterator() { return ArrayIterator<Vertex>(&vertices); }

    ArrayIterator<Edge> edge_iterator() { return ArrayIterator<Edge>(&edges); }

    DepthFirstIterator depth_first_iterator(Vertex *start) { return DepthFirstIterator(start); }

    BreadthFirstIterator breadth_first_iterator(Vertex *start) { return BreadthFirstIterator(start); }

    Graph(bool dir) { directed = dir; }

    int num_vertices() { return vertices.size(); }

    bool is_directed() { return directed; }

    Vertex *get_vertex(int i) { return vertices[i]; }

    Vertex *add_vertex(VDATA data) {
        Vertex *ret = new Vertex(data, vertices.size());
        vertices.push_back(ret);
        return ret;
    }

    Edge *add_edge(EDATA weight, Vertex *source, Vertex *destination) {
        if (source && destination) {
            Edge *ret = new Edge(weight, source, destination, is_directed());
            edges.push_back(ret);
            source->add_neighbor(ret);
            if (!directed)
                destination->add_neighbor(ret);
            return ret;
        }
        return 0;
    }
};
