/**
 * \file stat.h
 *
 * \brief Statistics data structure used in measurements.
 */
#ifndef STAT_H
#define STAT_H

#include <stdlib.h>
#include <stdbool.h>

/**
 * \brief Unit to collect statistical values in, should be maximum precision
 * possible to prevent overflow.
 */
typedef long double stat_unit;

/**
 * \brief Statistical information.
 */
struct stat {
  stat_unit value;
    /**< Temporary measurement value */
  stat_unit mean;
    /**< Computed mean */
  stat_unit stdev;
    /**< Computed standard deviation */
  stat_unit sum;
    /**< Computed sum of measurements */
  size_t samples;
    /**< Total number of samples collected */
};

/**
 * \brief Initialize stat.
 *
 * \return       Pointer to allocated stat
 */
struct stat * stat_init();

/**
 * \brief Free stat.
 *
 * \param s      Pointer to stat to be free'd
 */
void stat_free(struct stat *s);

/**
 * \brief Convert a stat into a string representation.
 *
 * If all is not set this only renders the mean.
 *
 * \param s       Pointer to stat
 * \param all     Render all stat information ?
 * \return        String representation
 */
char * stat_to_string(struct stat *s, bool all);

/**
 * \brief Update statistics based on newly measured stat::value.
 *
 * Update occurs inplace.
 *
 * \param s       Pointer to stat
 */
void stat_update(struct stat *s);

/**
 * \brief Determine the size of a statistically significant sample based on
 * values collected in stat.
 *
 * \param s       Pointer to stat
 * \param z       Z-Score
 * \param rho     Determine if sample is within \rho % of mean
 * \return        Statistically significant sample size
 */
size_t stat_sig_sample(struct stat *s, stat_unit z, stat_unit rho);

#endif/*STAT_H*/
