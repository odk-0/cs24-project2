// Winter'24
// Instructor: Diba Mirza
// Student name: Owen Kirchner
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <cstring>
#include <algorithm>
#include <limits.h>
#include <iomanip>
#include <set>
#include <queue>
#include <sstream>
using namespace std;

#include "utilities.h"
#include "movies.h"

bool parseLine(string &line, string &movieName, double &movieRating);

int main(int argc, char** argv){
    if (argc < 2){
        cerr << "Not enough arguments provided (need at least 1 argument)." << endl;
        cerr << "Usage: " << argv[ 0 ] << " moviesFilename prefixFilename " << endl;
        exit(1);
    }

    ifstream movieFile (argv[1]);
 
    if (movieFile.fail()){
        cerr << "Could not open file " << argv[1];
        exit(1);
    }
  
    // Create an object of a STL data-structure to store all the movies
    // we'll use a min-heap (priority_queue with custom comparator) to sort by name
    priority_queue<Movie, vector<Movie>, CompareByName> moviesPQ;

    string line, movieName;
    double movieRating;
    // Read each file and store the name and rating
    while (getline (movieFile, line) && parseLine(line, movieName, movieRating)){
            // Use std::string movieName and double movieRating
            // to construct your Movie objects and insert into PQ
            moviesPQ.push(Movie(movieName, movieRating));
    }

    movieFile.close();

    if (argc == 2){
            //print all the movies in ascending alphabetical order of movie names
            while (!moviesPQ.empty()){
                Movie m = moviesPQ.top();
                cout << m.name << ", " << fixed << setprecision(1) << m.rating << "\n";
                moviesPQ.pop();
            }
            return 0;
    }

    ifstream prefixFile (argv[2]);

    if (prefixFile.fail()) {
        cerr << "Could not open file " << argv[2];
        exit(1);
    }

    vector<string> prefixes;
    while (getline (prefixFile, line)) {
        if (!line.empty()) {
            prefixes.push_back(line);
        }
    }

    //  For each prefix,
    //  Find all movies that have that prefix and store them in an appropriate data structure
    //  If no movie with that prefix exists print the following message

    // Move movies from PQ into a vector 
    vector<Movie> moviesList;
    while (!moviesPQ.empty()){
        moviesList.push_back(moviesPQ.top());
        moviesPQ.pop();
    }

    // We'll collect best movie per prefix and print them at the end 
    vector<pair<string, Movie>> bestPerPrefix;

    // moviesList is already sorted alphabetically by name because
    // we popped everything out of a min-heap.  use lower_bound to jump to
    // the first candidate movie, then scan forward until the prefix fails.
    for (const string &prefix : prefixes) {
        vector<Movie> matches;
        auto it = lower_bound(moviesList.begin(), moviesList.end(), prefix,
            [&](const Movie &m, const string &pref){
                return m.name.compare(0, pref.size(), pref) < 0;
            });
        for (; it != moviesList.end(); ++it) {
            if (it->name.size() >= prefix.size() &&
                it->name.compare(0, prefix.size(), prefix) == 0) {
                matches.push_back(*it);
            } else {
                break;
            }
        }

        if (matches.empty()) {
            cout << "No movies found with prefix " << prefix << endl;
        } else {
            // sort by rating (descending), then name (ascending)
            sort(matches.begin(), matches.end(), [](const Movie &a, const Movie &b){
                if (a.rating != b.rating) return a.rating > b.rating;
                return a.name < b.name;
            });

            for (const Movie &m : matches) {
                cout << m.name << ", " << fixed << setprecision(1) << m.rating << "\n";
            }

            // record best (first after sort)
            bestPerPrefix.emplace_back(prefix, matches.front());
            
            // extra blank line after each prefix block
            cout << "\n";
        }

    }

    if (!bestPerPrefix.empty()) {
        for (const auto &p : bestPerPrefix) {
            cout << "Best movie with prefix " << p.first << " is: " << p.second.name
                 << " with rating " << fixed << setprecision(1) << p.second.rating << endl;
        }
    }

    return 0;
}

/*
Time and space complexity analysis for Part 2:

Assumptions and parameters:
 - n = number of movies in the dataset
 - m = number of prefixes to process
 - k = maximum number of movies that match a given prefix
 - l = maximum length of a movie name (characters)

Data structures:
 - movies are stored in a priority_queue during parsing, then moved to a vector
 - prefixes are stored in a vector of strings
 - matches for a prefix are collected in a temporary vector and sorted

Time complexity (worst case):
 1. Building the priority_queue while reading n lines: each push is O(log n),
    so O(n log n) overall.
 2. Converting heap to vector: popping n items, each pop O(log n): O(n log n).
 3. For each of the m prefixes:
    a. Use binary search on the sorted moviesList to find the first name
       not less than the prefix (O(log n * l) for string comparisons).  
    b. Scan forward from that position collecting at most k matching movies
       (O(k*l) to compare each name).  
    c. Sorting the matches vector. In worst case k = n, so sorting is
       O(n log n); for typical inputs k << n, the cost is O(k log k).  
    d. Printing k results is O(k).
    Per-prefix cost becomes O(log n + k log k + k*l).  When k is small this
    is much faster than scanning all n entries, and the worst-case when k=n
    still matches our previous bound.

 Putting it together:
 Total worst-case runtime = O(n log n)  (heap build)
                            + O(n log n) (heap to vector)
                            + m * O(n*l + n log n)
                          = O((m+2) n log n + m n l).

 Since l is a constant bound on title length, the dominating term is
 O(m n log n).

Measured runtimes on CSIL using prefix_large.txt (randomized inputs):
  input_20_random.csv   -> ~14 ms
  input_100_random.csv  -> ~12 ms
  input_1000_random.csv -> ~15 ms
  input_76920_random.csv-> ~129 ms
(After optimizing with binary search on the sorted movie list, the large
dataset runtime drops dramatically; these figures now reflect
O(m \, log n + k log k) behaviour with k small.)

Space complexity:
 - Storing all movies in vector: O(n)
 - Prefix vector: O(m)
 - Temporary matches vector per prefix: O(k) (<= O(n))
 - Heap itself also uses O(n) until converted.
 Overall space is O(n + m + k) = O(n + m).

These bounds assume that movie names and prefixes themselves occupy
space proportional to their character lengths; since l is constant, we
can regard that as O(1) per string.

3c) Tradeoffs between time and space:

  The implementation prioritizes reasonable time performance.  By
  reading all movies into a heap and then a vector (which we maintain
  in sorted order), the algorithm enables fast prefix scanning.  Using
  binary search to locate the matching range for each prefix reduces the
  scan cost from O(n) to O(log n + k) per prefix, so the effective
  runtime is closer to O(m (log n + k log k)).  This optimization still
  uses extra space (the movies vector plus temporary match lists) rather
  than streaming the file, so space complexity remains O(n+m).

  A lower-space design could process each prefix against the file
  on-the-fly without storing all movies, reducing memory to O(1) aside
  from the prefix list.  However, that would incur a cost of reading the
  movie file m times and repeating string comparisons, leading to
  O(m n l) time (worse than the current approach especially when m is
  large).

*/

bool parseLine(string &line, string &movieName, double &movieRating) {
    int commaIndex = line.find_last_of(",");
    movieName = line.substr(0, commaIndex);
    movieRating = stod(line.substr(commaIndex+1));
    if (movieName[0] == '\"') {
        movieName = movieName.substr(1, movieName.length() - 2);
    }
    return true;
}