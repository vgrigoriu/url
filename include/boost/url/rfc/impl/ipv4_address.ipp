//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/CPPAlliance/url
//

#ifndef BOOST_URL_RFC_IMPL_IP_V4ADDRESS_IPP
#define BOOST_URL_RFC_IMPL_IP_V4ADDRESS_IPP

#include <boost/url/rfc/ipv4_address.hpp>
#include <boost/url/error.hpp>
#include <boost/url/bnf/parse.hpp>
#include <boost/url/rfc/char_sets.hpp>

namespace boost {
namespace urls {
namespace rfc {

namespace detail {

struct dec_octet
{
    std::uint8_t& v;

    friend
    bool
    parse(
        char const*& it,
        char const* const end,
        error_code& ec,
        dec_octet const& t)
    {
        bnf::digit_chars dc;
        if(it == end)
        {
            // expected DIGIT
            ec = error::syntax;
            return false;
        }
        if(! dc(*it))
        {
            ec = error::syntax;
            return false;
        }
        unsigned v = *it - '0';
        ++it;
        if(it == end)
        {
            t.v = static_cast<
                std::uint8_t>(v);
            ec = {};
            return true;
        }
        if(! dc(*it))
        {
            t.v = static_cast<
                std::uint8_t>(v);
            ec = {};
            return true;
        }
        if(v == 0)
        {
            // bad leading '0'
            ec = error::syntax;
            return false;
        }
        v = (10 * v) + *it - '0';
        ++it;
        if(it == end)
        {
            t.v = static_cast<
                std::uint8_t>(v);
            ec = {};
            return true;
        }
        if(! dc(*it))
        {
            t.v = static_cast<
                std::uint8_t>(v);
            ec = {};
            return true;
        }
        if(v > 25)
        {
            // out of range
            ec = error::syntax;
            return false;
        }
        v = (10 * v) + *it - '0';
        if(v > 255)
        {
            // out of range
            ec = error::syntax;
            return false;
        }
        ++it;
        t.v = static_cast<
            std::uint8_t>(v);
        ec = {};
        return true;
    }
};

} // detail

bool
parse(
    char const*& it,
    char const* const end,
    error_code& ec,
    ipv4_address& t)
{
    using bnf::parse;
    auto const start = it;
    if(! parse(it, end, ec,
        detail::dec_octet{
            t.octets[0]}, '.',
        detail::dec_octet{
            t.octets[1]}, '.',
        detail::dec_octet{
            t.octets[2]}, '.',
        detail::dec_octet{
            t.octets[3]}))
        return false;
    t.str = string_view(
        start, it - start);
    return true;
}

} // rfc
} // urls
} // boost

#endif
