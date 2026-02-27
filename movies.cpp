#include "movies.h"

Movie::Movie(const std::string &n, double r)
    : name(n), rating(r) {}

bool CompareByName::operator()(const Movie &a, const Movie &b) const {
    // priority_queue is a max-heap by default; to simulate a min-heap
    // on the movie name we return true when a.name > b.name
    return a.name > b.name;
}