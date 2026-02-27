#ifndef MOVIES_H
#define MOVIES_H

#include <string>

// simple struct to hold movie information
struct Movie {
    std::string name;
    double rating;

    Movie(const std::string &n = "", double r = 0.0);
};

// comparator for priority_queue that orders movies by name (ascending)
struct CompareByName {
    bool operator()(const Movie &a, const Movie &b) const;
};

#endif // MOVIES_H
