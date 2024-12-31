#include "cpp_argv.hpp"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <tbb/parallel_reduce.h>
#include <tbb/blocked_range.h>

#define DEFAULT_NAME "pearson"

/**
 * @brief Data measurement set.
 *
 */
struct Data_Set {
  size_t n;  /** Number of measurements.  */
  double *x; /** Variable X measurements. */
  double *y; /** Variable Y measurements. */
};

/**
 * @brief Pearson correlation.
 *
 */
struct Correlation {
  double a; /** Right slope.         */
  double b; /** Y-axis shift.        */
  double r; /** Pearson coefficient. */
};

/**
 * @brief Loads a data set from a input stream then returns it.
 *
 * @param stream The input stream.
 * @return Data_Set The data set.
 */
Data_Set load_file(std::istream &stream) noexcept;

/**
 * @brief Calculates then returns the Pearson correlation of a data set.
 *
 * @param data_set The data set.
 * @return Correlation The corresponding Pearson correlation.
 */
Correlation calculate(const Data_Set &data_set) noexcept;

/**
 * @brief Main program.
 *
 * @param argc number of arguments in the command line.
 * @param argv arguments of the command line.
 * @return @c EXIT_SUCCESS if command succeeds else @c EXIT_FAILURE.
 */
int main(int argc, char *argv[]) {

  // User expects help.
  CPP_ARGV_TEST_HELP_REQUEST(argc, argv[0], DEFAULT_NAME, "filename")

  // Bad argument number.
  CPP_ARGV_TEST_ARG_NUM(argc, 2)

  // Retrieves the data filename.
  const char *const filename = argv[1];

  // Tries to open it.
  std::ifstream stream(filename);
  if (not stream) {
    std::cerr << std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  // Loads the data set.
  const Data_Set data_set = load_file(stream);

  // Calculates the corresponding Pearson correlation.
  const Correlation result = calculate(data_set);

  // Prints results onto the standard output.
  std::cout << "a: " << result.a << "\tb: " << result.b << "\tr: " << result.r
            << std::endl;

  // It's over.
  return EXIT_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*                                  load_file                                 */
/* -------------------------------------------------------------------------- */

Data_Set load_file(std::istream &stream) noexcept {
  Data_Set res;

  stream >> res.n;
  res.x = new double[res.n];
  res.y = new double[res.n];

  for (size_t i = 0; i < res.n; i++) {
    stream >> res.x[i] >> res.y[i];
  }

  return res;
}

/* -------------------------------------------------------------------------- */
/*                                  calculate                                 */
/* -------------------------------------------------------------------------- */

class AverageReducer {
  public:
    const double* x;
    const double* y;
    double sum_x, sum_y;
    size_t n;

    AverageReducer (const double* x, const double* y) : x(x), y(y), sum_x(0.0), sum_y(0.0) {}

    AverageReducer (const AverageReducer& other, tbb::split) : x(other.x), y(other.y), sum_x(0.0), sum_y(0.0) {}

    void operator() (const tbb::blocked_range<size_t>& r) {
      for (size_t i = r.begin(); i != r.end(); i ++) {
        sum_x += x[i];
        sum_y += y[i];
      }
    }

    void join (const AverageReducer& other) {
      sum_x += other.sum_x;
      sum_y += other.sum_y;
    }
};


class PearsonReducer {
  public:
    const double* x;
    const double* y;
    double avg_x, avg_y;
    double tot_xx, tot_xy, tot_yy;

    PearsonReducer (const double* x, const double* y, double avg_x, double avg_y) : x(x), y(y), avg_x(avg_x), avg_y(avg_y), tot_xx(0.0), tot_xy(0.0), tot_yy(0.0) {}

    PearsonReducer (const PearsonReducer& other, tbb::split) : x(other.x), y(other.y), avg_x(other.avg_x), avg_y(other.avg_y), tot_xx(0.0), tot_xy(0.0), tot_yy(0.0) {}

    void operator() (const tbb::blocked_range<size_t>& r) {
      for (size_t i = r.begin(); i != r.end(); i ++) {
        const double dx = x[i] - avg_x;
        const double dy = y[i] - avg_y;
        tot_xx += dx * dx;
        tot_xy += dx * dy;
        tot_yy += dy * dy;

      }
    }

    void join (const PearsonReducer& other) {
      tot_xx += other.tot_xx;
      tot_xy += other.tot_xy;
      tot_yy += other.tot_yy;
    }
};

Correlation calculate(const Data_Set &data_set) noexcept {

  AverageReducer avg_reducer(data_set.x, data_set.y);
  tbb::parallel_reduce(tbb::blocked_range<size_t>(0, data_set.n), avg_reducer);

  const double moy_x = avg_reducer.sum_x / data_set.n;
  const double moy_y = avg_reducer.sum_y / data_set.n;

  PearsonReducer pearson_reducer(data_set.x, data_set.y, moy_x, moy_y);
  tbb::parallel_reduce(tbb::blocked_range<size_t>(0, data_set.n), pearson_reducer);

  Correlation res;
  res.a = pearson_reducer.tot_xy / pearson_reducer.tot_xx;
  res.b = moy_y - res.a * moy_x;
  res.r = pearson_reducer.tot_xy / std::sqrt(pearson_reducer.tot_xx * pearson_reducer.tot_yy);

  return res;
}

// Correlation calculate (const Data_Set &data_set) noexcept {
//   double tot_x = 0.0, tot_y = 0.0;
//   for (size_t i = 0; i != data_set.n; i ++) {
//     tot_x += data_set.x[i];
//     tot_y += data_set.y[i];
//   }
//   const double moy_x = tot_x / data_set.n, moy_y = tot_y / data_set.n;
  
//   double tot_xx = 0.0, tot_xy = 0.0, tot_yy = 0.0;
//   for (size_t i = 0; i != data_set.n; i ++) {
//     tot_xy += (data_set.x[i] - moy_x) * (data_set.y[i] - moy_y);
//     tot_xx += (data_set.x[i] - moy_x) * (data_set.x[i] - moy_x);
//     tot_yy += (data_set.y[i] - moy_y) * (data_set.y[i] - moy_x);
//   }

//   Correlation res;
//   res.a = tot_xy / tot_xx;
//   res.b = moy_y - res.a * moy_x;
//   res.r = tot_xy / std::sqrt(tot_xx * tot_yy);

//   return res;
// }