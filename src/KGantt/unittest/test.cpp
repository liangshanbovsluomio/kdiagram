/*
 * Copyright (C) 2001-2015 Klaralvdalens Datakonsult AB.  All rights reserved.
 *
 * This file is part of the KGantt library.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef KDAB_NO_UNIT_TESTS

#include "test.h"

#ifdef TEMPORARILY_REMOVED
#include <trinav/ascshared/libfakes/fakes.h>
#endif

#include <cmath>
#include <limits>

KDAB::UnitTest::Test::Test( const std::string & n )
    : mName( n ), mFailed( 0 ), mSucceeded( 0 ) {}

void KDAB::UnitTest::Test::_assertNotNull( const void * x, const char * expression, const char * file, unsigned int line ) {
    if ( x ) success();
    else fail( file, line ) << '"' << expression << "\" is NULL, expected non-NULL" << std::endl;
}

void KDAB::UnitTest::Test::_assertNull( const void * x, const char * expression, const char * file, unsigned int line ) {
    if ( !x ) success();
    else fail( file, line ) << '"' << expression << "\" is not NULL, expected NULL" << std::endl;
}

#if 0
void KDAB::UnitTest::Test::_assertIsNaN( qreal d, const char * expression, const char * file, unsigned int line ) {
    if ( std::isnan( d ) ) success();
    else fail( file, line ) << '"' << expression << "\" yielded " << d << "; expected NaN" << std::endl;
}

void KDAB::UnitTest::Test::_assertIsNotNaN( qreal d, const char * expression, const char * file, unsigned int line ) {
    if ( !std::isnan( d ) ) success();
    else fail( file, line ) << '"' << expression << "\" yielded nan; expected something else" << std::endl;
}
#endif

void KDAB::UnitTest::Test::_assertTrue( bool x, const char * expression, const char * file, unsigned int line ) {
    if ( x ) success();
    else fail( file, line ) << '"' << expression << "\" != TRUE" << std::endl;
}

void KDAB::UnitTest::Test::_assertFalse( bool x, const char * expression, const char * file, unsigned int line ) {
    if ( !x ) success();
    else fail( file, line ) << '"' << expression << "\" != FALSE" << std::endl;
}

void KDAB::UnitTest::Test::_assertEqualWithEpsilons( float x1, float x2, int prec, const char * expr1, const char * expr2, const char * exprP, const char * file, unsigned int line ) {
    if ( qAbs( x1/x2 - 1.0f ) <= prec * std::numeric_limits<float>::epsilon() ) success();
    else fail( file, line ) << x1 << " (" << expr1 << ") deviates from expected "
                            << x2 << " (" << expr2 << ") by more than "
                            << prec << " (" << exprP << ") epsilons." << std::endl;
}

void KDAB::UnitTest::Test::_assertEqualWithEpsilons( qreal x1, qreal x2, int prec, const char * expr1, const char * expr2, const char * exprP, const char * file, unsigned int line ) {
    if ( qAbs( x1/x2 - 1.0 ) <= prec * std::numeric_limits<qreal>::epsilon() ) success();
    else fail( file, line ) << x1 << " (" << expr1 << ") deviates from expected "
                            << x2 << " (" << expr2 << ") by more than "
                            << prec << " (" << exprP << ") epsilons." << std::endl;
}

void KDAB::UnitTest::Test::_assertEqualWithEpsilons( long double x1, long double x2, int prec, const char * expr1, const char * expr2, const char * exprP, const char * file, unsigned int line ) {
    if ( qAbs( x1/x2 - 1.0l ) <= prec * std::numeric_limits<long double>::epsilon() ) success();
    else fail( file, line ) << x1 << " (" << expr1 << ") deviates from expected "
                            << x2 << " (" << expr2 << ") by more than "
                            << prec << " (" << exprP << ") epsilons." << std::endl;
}

std::ostream & KDAB::UnitTest::Test::fail( const char * file, unsigned int line ) {
    ++mFailed;
    return std::cerr << "FAIL: " << file << ':' << line << ": ";
}

#endif // KDAB_NO_UNIT_TESTS
