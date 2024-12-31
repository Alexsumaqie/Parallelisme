#ifndef ParallelRecursiveMerge_hpp
#define ParallelRecursiveMerge_hpp

#include <algorithm>
#include <functional>
#include <iterator>
#include <tbb/task_group.h>

namespace merging {

  /**
   * @class ParallelRecursiveMerge ParallelRecursiveMerge.hpp
   *
   * TBB version of the standard library merge algorithm.
   * 
   * @note The proposed implementation is that of recursive merging described in
   *   Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest, and Clifford 
   *   Stein, "Introduction to Algorithms", 3rd ed., 2009, pp 798-802. The 
   *   recursion is interrupted when the sum of the sizes of the two 
   *   subcontainers to be merged falls below a certain tolerance. The merge 
   *   is then performed using the standard library merge algorithm.
   */
  class ParallelRecursiveMerge {
  public:

    /**
     * General form of the algorithm.
     *
     * @param[in] first1 - an iterator pointing to the first element of the first
     *   subcontainer involved in the merge;
     * @param[in] last1 - an iterator pointing to the element just past the 
     *   last element of the first subcontainer involved in the merge;
     * @param[in] first2 - an iterator pointing to the first element of the second
     *   subcontainer involved in the merge;
     * @param[in] last2 - an iterator pointing to the element just past the 
     *   last element of the second subcontainer involved in the merge;
     * @param[in] result - an iterator pointing to the position where the 
     *   first resulting element of the merge should be copied;
     * @param[in] comp - a binary comparator representing the total order
     *   relation governing the subcontainers;
     * @param[in] cutoff - the sum of the sizes of the two subcontainers below 
     *   which the merge is performed using the standard library merge algorithm.
     * @return an iterator pointing to the end of the merge area in the
     *   target container.
     */
    template< typename InputRandomAccessIterator1,
          typename InputRandomAccessIterator2,
          typename OutputRandomAccessIterator,
          typename Compare >
    static OutputRandomAccessIterator 
    apply(const InputRandomAccessIterator1& first1,
      const InputRandomAccessIterator1& last1,
      const InputRandomAccessIterator2& first2,
      const InputRandomAccessIterator2& last2,
      const OutputRandomAccessIterator& result,
      const Compare& comp,
      const size_t& cutoff) {

      // Invoke the appropriate strategy.
      strategyB(first1, 
        last1, 
        first2, 
        last2, 
        result, 
        comp, 
        cutoff);
      
      // Respect the semantics of the merge algorithm.
      return result + (last1 - first1) + (last2 - first2);

    } // apply

    /**
     * Specific form of the algorithm for the total order relation 
     * strictly less than.
     *
     * @param[in] first1 - an iterator pointing to the first element of the first
     *   subcontainer involved in the merge;
     * @param[in] last1 - an iterator pointing to the element just past the 
     *   last element of the first subcontainer involved in the merge;
     * @param[in] first2 - an iterator pointing to the first element of the second
     *   subcontainer involved in the merge;
     * @param[in] last2 - an iterator pointing to the element just past the 
     *   last element of the second subcontainer involved in the merge;
     * @param[in] result - an iterator pointing to the position where the 
     *   first resulting element of the merge should be copied;
     * @param[in] comp - a binary comparator representing the total order
     *   relation governing the subcontainers;
     * @param[in] cutoff - the sum of the sizes of the two subcontainers below 
     *   which the merge is performed using the standard library merge algorithm.
     * @return an iterator pointing to the end of the merge area in the
     *   target container.
     */
    template< typename InputRandomAccessIterator1,
          typename InputRandomAccessIterator2,
          typename OutputRandomAccessIterator >
    static OutputRandomAccessIterator
    apply(const InputRandomAccessIterator1& first1,
      const InputRandomAccessIterator1& last1,
      const InputRandomAccessIterator2& first2,
      const InputRandomAccessIterator2& last2,
      const OutputRandomAccessIterator& result,
      const size_t& cutoff) {

      // Synonym type for the type of elements in the first container. The type 
      // of elements in the second must be implicitly convertible to the 
      // type of elements in the first.
      typedef std::iterator_traits< InputRandomAccessIterator1 > Traits;
      typedef typename Traits::value_type value_type;

      // Create the less comparator and then invoke the method defined 
      // above.
      return apply(first1, 
           last1,
           first2,
           last2,
           result,
           std::less< const value_type& >(),
           cutoff);
      
    } // apply

  protected:

    /**
     * MIMD implementation of this algorithm based on the use of
     * parallel sections.
     *
     * @param[in] first1 - an iterator pointing to the first element of the first
     *   subcontainer involved in the merge;
     * @param[in] last1 - an iterator pointing to the element just past the 
     *   last element of the first subcontainer involved in the merge;
     * @param[in] first2 - an iterator pointing to the first element of the second
     *   subcontainer involved in the merge;
     * @param[in] last2 - an iterator pointing to the element just past the 
     *   last element of the second subcontainer involved in the merge;
     * @param[in] result - an iterator pointing to the position where the 
     *   first resulting element of the merge should be copied;
     * @param[in] comp - a binary comparator representing the total order
     *   relation governing the subcontainers;
     * @param[in] cutoff - the sum of the sizes of the two subcontainers below 
     *   which the merge is performed using the standard library merge algorithm.
     * @return an iterator pointing to the end of the merge area in the
     *   target container.
     */
    template< typename InputRandomAccessIterator1,
          typename InputRandomAccessIterator2,
          typename OutputRandomAccessIterator,
          typename Compare >
    static void strategyA(const InputRandomAccessIterator1& first1,
              const InputRandomAccessIterator1& last1,
              const InputRandomAccessIterator2& first2,
              const InputRandomAccessIterator2& last2,
              const OutputRandomAccessIterator& result,
              const Compare& comp,
              const size_t& cutoff) {
 
      // Size of the two subcontainers.
      const auto size1 = last1 - first1;
      const auto size2 = last2 - first2;

      // We have fallen below the tolerance: recursion stops.
      if (static_cast< size_t >(size1 + size2) < cutoff) {
        std::merge(first1, last1, first2, last2, result, comp);
        return;
      }

      // The subcontainer corresponding to the left operand is conventionally 
      // longer than the one corresponding to the right operand. If not,
      // we put things in order.
      if (size1 < size2) {
        strategyA(first2, 
          last2, 
          first1, 
          last1, 
          result,   
          comp,
          cutoff);
        return;
      }

      // Iterator pointing to the median element in the left subcontainer.
      const InputRandomAccessIterator1 middle1 = 
        first1 + size1 / 2 + size1 % 2;

      // Iterator pointing to the pivot element in the right subcontainer.
      const InputRandomAccessIterator2 middle2 = 
        std::lower_bound(first2, last2, *middle1, comp);

      // Iterator pointing to the position in the result subcontainer where 
      // the median element of the left subcontainer will be placed.
      const OutputRandomAccessIterator middle3 = 
        result + (middle1 - first1) + (middle2 - first2);

      // Copy the median element of the left subcontainer into the 
      // result subcontainer.
      *middle3 = *middle1;

      // Parallel sections dedicated to recursive calls. If nested 
      // parallelism is enabled, the number of threads that can be
      // created does not exceed the number of available threads.
      tbb::task_group tg;
      tg.run(
        [&] {
          strategyA(first1, middle1, first2, middle2, result, comp, cutoff);
        });
      tg.run(
        [&] {
          strategyA(middle1 + 1, last1, middle2, last2, middle3 + 1, comp, cutoff);
        }
      );
      tg.wait();
    } // strategyA

    /**
     * MIMD implementation of this algorithm based on the use of tasks.
     *
     * @param[in] first1 - an iterator pointing to the first element of the first
     *   subcontainer involved in the merge;
     * @param[in] last1 - an iterator pointing to the element just past the 
     *   last element of the first subcontainer involved in the merge;
     * @param[in] first2 - an iterator pointing to the first element of the second
     *   subcontainer involved in the merge;
     * @param[in] last2 - an iterator pointing to the element just past the 
     *   last element of the second subcontainer involved in the merge;
     * @param[in] result - an iterator pointing to the position where the 
     *   first resulting element of the merge should be copied;
     * @param[in] comp - a binary comparator representing the total order
     *   relation governing the subcontainers;
     * @param[in] cutoff - the sum of the sizes of the two subcontainers below 
     *   which the merge is performed using the standard library merge algorithm.
     */
    template< typename InputRandomAccessIterator1,
          typename InputRandomAccessIterator2,
          typename OutputRandomAccessIterator,
          typename Compare >
    static void strategyB(const InputRandomAccessIterator1& first1,
              const InputRandomAccessIterator1& last1,
              const InputRandomAccessIterator2& first2,
              const InputRandomAccessIterator2& last2,
              const OutputRandomAccessIterator& result,
              const Compare& comp,
              const size_t& cutoff) {

      // The set of tasks (recursive calls) is created by a single thread.
      // The nowait clause removes the explicit synchronization barrier 
      // placed at the end of the single block. Therefore, other
      // threads can start executing tasks while another
      // continues to produce them. 
    
      strategyBTasking(first1, last1, first2, last2, result, comp, cutoff);

    } // strategyB

    /**
     * Recursive part of the parallelization strategy @c strategyB.
     *
     * @param[in] first1 - an iterator pointing to the first element of the first
     *   subcontainer involved in the merge;
     * @param[in] last1 - an iterator pointing to the element just past the 
     *   last element of the first subcontainer involved in the merge;
     * @param[in] first2 - an iterator pointing to the first element of the second
     *   subcontainer involved in the merge;
     * @param[in] last2 - an iterator pointing to the element just past the 
     *   last element of the second subcontainer involved in the merge;
     * @param[in] result - an iterator pointing to the position where the 
     *   first resulting element of the merge should be copied;
     * @param[in] comp - a binary comparator representing the total order
     *   relation governing the subcontainers;
     * @param[in] cutoff - the sum of the sizes of the two subcontainers below 
     *   which the merge is performed using the standard library merge algorithm.
     */    
    template< typename InputRandomAccessIterator1,
          typename InputRandomAccessIterator2,
          typename OutputRandomAccessIterator,
          typename Compare >
    static void strategyBTasking(const InputRandomAccessIterator1& first1,
                 const InputRandomAccessIterator1& last1,
                 const InputRandomAccessIterator2& first2,
                 const InputRandomAccessIterator2& last2,
                 const OutputRandomAccessIterator& result,
                 const Compare& comp,
                 const size_t& cutoff) {

      // Size of the two subcontainers.
      const auto size1 = last1 - first1;
      const auto size2 = last2 - first2;

      // We have fallen below the tolerance: recursion stops.
      if (static_cast< size_t >(size1 + size2) < cutoff) {
        std::merge(first1, last1, first2, last2, result, comp);
        return;
      }

      // The subcontainer corresponding to the left operand is conventionally 
      // longer than the one corresponding to the right operand. If not,
      // we put things in order.
      if (size1 < size2) {
        strategyBTasking(first2, 
             last2, 
             first1, 
             last1, 
             result,    
             comp,
             cutoff);
        return;
      }

      // Iterator pointing to the median element in the left subcontainer.
      const InputRandomAccessIterator1 middle1 = 
        first1 + size1 / 2 + size1 % 2;

      // Iterator pointing to the pivot element in the right subcontainer.
      const InputRandomAccessIterator2 middle2 = 
        std::lower_bound(first2, last2, *middle1, comp);

      // Iterator pointing to the position in the result subcontainer where 
      // the median element of the left subcontainer will be placed.
      const OutputRandomAccessIterator middle3 = 
        result + (middle1 - first1) + (middle2 - first2);

      // Copy the median element of the left subcontainer into the 
      // result subcontainer.
      *middle3 = *middle1;

      // Merge the part to the left of the median element in the 
      // left subcontainer with the part to the left of the pivot 
      // element in the right subcontainer. Since an explicit 
      // synchronization barrier is placed before exiting this method,
      // we use the untied clause to allow a sleeping task to be resumed by another thread.
      tbb::task_group tg;
      tg.run(
        [&] {
          strategyBTasking(first1, middle1, first2, middle2, result, comp, cutoff);
        });
      tg.run(
        [&] {
          strategyBTasking(middle1 + 1, last1, middle2, last2, middle3 + 1, comp, cutoff);
        }
      );
      tg.wait();
    } // strategyBTasking

  }; // ParallelRecursiveMerge

} // merging

#endif
