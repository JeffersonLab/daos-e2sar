/**
    Baisc DAOS version test.

    First checked in by xmei@jlab.org on Mar/26/2024.
*/

#define BOOST_TEST_MODULE DaosVersionTest
#include <boost/test/included/unit_test.hpp>
#include <daos.h>

// Test whether DAOS version is 2.4.x.
// This ensures <daos.h> can be correctly compiled and linked.
BOOST_AUTO_TEST_CASE(VersionAssertion) {
    BOOST_CHECK_EQUAL(DAOS_VERSION_MAJOR, 2);
    BOOST_CHECK_EQUAL(DAOS_VERSION_MINOR, 4);
    // BOOST_CHECK_EQUAL(DAOS_VERSION_FIX, 0);
}
