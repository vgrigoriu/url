//
// Copyright (c) 2019 Vinnie Falco (vinnie.falco@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/url
//

#ifndef BOOST_URL_IMPL_VIEW_IPP
#define BOOST_URL_IMPL_VIEW_IPP

#include <boost/url/view.hpp>
#include <boost/url/error.hpp>
#include <boost/url/detail/parse.hpp>

namespace boost {
namespace url {

view::
view(string_view s)
    : s_(s.data())
{
    detail::parser pr(s);
    error_code ec;
    detail::parse_url(pt_, s, ec);
    if(ec)
        invalid_part::raise();
}

string_view
view::
encoded_url() const
{
    return pt_.get(
        detail::id_scheme,
        detail::id_end,
        s_);
}

string_view
view::
encoded_origin() const noexcept
{
    return pt_.get(
        detail::id_scheme,
        detail::id_path,
        s_);
}

//----------------------------------------------------------
//
// scheme
//
//----------------------------------------------------------

string_view
view::
scheme() const noexcept
{
    auto s = pt_.get(
        detail::id_scheme,
        s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(s.back() == ':');
    s.remove_suffix(1); // ':'
    return s;
}

//----------------------------------------------------------
//
// authority
//
//----------------------------------------------------------

string_view
view::
encoded_authority() const noexcept
{
    auto s = pt_.get(
        detail::id_username,
        detail::id_path,
        s_);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

//----------------------------------------------------------
//
// userinfo
//
//----------------------------------------------------------

string_view
view::
encoded_userinfo() const noexcept
{
    auto s = pt_.get(
        detail::id_username,
        detail::id_hostname,
        s_);
    if(s.empty())
        return s;
    if(s.back() == '@')
        s.remove_suffix(1);
    BOOST_ASSERT(s.size() >= 2);
    BOOST_ASSERT(
        s.substr(0, 2) == "//");
    s.remove_prefix(2);
    return s;
}

string_view
view::
encoded_username() const noexcept
{
    auto s = pt_.get(
        detail::id_username,
        s_);
    if(! s.empty())
    {
        BOOST_ASSERT(s.size() >= 2);
        BOOST_ASSERT(
            s.substr(0, 2) == "//");
        s.remove_prefix(2);
    }
    return s;
}

string_view
view::
encoded_password() const noexcept
{
    auto s = pt_.get(
        detail::id_password,
        s_);
    switch(s.size())
    {
    case 1:
        BOOST_ASSERT(s.front() == '@');
    case 0:
        return {};
    default:
        BOOST_ASSERT(s.back() == '@');
        s.remove_suffix(1);
        if(s.front() == ':')
            s.remove_prefix(1);
        return s;
    }
}

//----------------------------------------------------------
//
// host
//
//----------------------------------------------------------

string_view
view::
encoded_host() const noexcept
{
    return pt_.get(
        detail::id_hostname,
        detail::id_path,
        s_);
}

string_view
view::
encoded_hostname() const noexcept
{
    return pt_.get(
        detail::id_hostname,
        s_);
}

string_view
view::
port_string() const noexcept
{
    auto s = pt_.get(
        detail::id_port,
        s_);
    BOOST_ASSERT(
        s.empty() || s.front() == ':');
    if(! s.empty())
        s.remove_prefix(1);
    return s;
}

//----------------------------------------------------------
//
// path
//
//----------------------------------------------------------

bool
view::
is_relative() const noexcept
{
    auto const s = pt_.get(
        detail::id_path, s_);
    return ! s.empty() &&
        s.front() != '/';
}

string_view
view::
encoded_path() const noexcept
{
    return pt_.get(
        detail::id_path,
        s_);
}

//----------------------------------------------------------
//
// query
//
//----------------------------------------------------------

string_view
view::
encoded_query() const noexcept
{
    auto s = pt_.get(
        detail::id_query, s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '?');
    return s.substr(1);
}

string_view
view::
query_part() const noexcept
{
    auto s = pt_.get(
        detail::id_query, s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '?');
    return s;
}

//----------------------------------------------------------
//
// fragment
//
//----------------------------------------------------------

string_view
view::
encoded_fragment() const noexcept
{
    auto s = pt_.get(
        detail::id_frag, s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '#');
    return s.substr(1);
}

string_view
view::
fragment_part() const noexcept
{
    auto s = pt_.get(
        detail::id_frag, s_);
    if(s.empty())
        return s;
    BOOST_ASSERT(
        s.front() == '#');
    return s;
}

//----------------------------------------------------------
//
// segments_type
//
//----------------------------------------------------------

view::
segments_type::
iterator::
iterator() noexcept
    : s_(nullptr)
    , pt_(nullptr)
    , off_(0)
    , n_(0)
{
}

view::
segments_type::
iterator::
iterator(
    segments_type const* v,
    bool end) noexcept
    : s_(v->s_)
    , pt_(v->pt_)
{
    if( end ||
        pt_->nseg == 0)
    {
        off_ = pt_->offset[
            detail::id_query];
        n_ = 0;
    }
    else
    {
        off_ = pt_->offset[
            detail::id_path];
        parse();
    }
}

auto
view::
segments_type::
iterator::
operator*() const noexcept ->
    value_type
{
    string_view s = {
        s_ + off_, n_ };
    if(! s.empty() &&
        s.front() == '/')
        s = s.substr(1);    
    return value_type(s);
}

auto
view::
segments_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != pt_->offset[
            detail::id_frag]);
    off_ = off_ + n_;
    if(off_ == pt_->offset[
        detail::id_frag])
    {
        // end
        n_ = 0;
    }
    else
    {
        parse();
    }
    return *this;
}

auto
view::
segments_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != pt_->offset[
            detail::id_path]);
    auto const begin =
        s_ + pt_->offset[
            detail::id_path];
    auto p = s_ + off_;
    while(--p > begin)
    {
        if(*p == '/')
        {
            off_ = p - s_;
            parse();
            return *this;
        }
    }
    // fails for relative-uri
    //BOOST_ASSERT(*p == '/');
    auto const off = p - s_;
    n_ = off_ - off;
    off_ = off;
    return *this;
}

void
view::
segments_type::
iterator::
parse() noexcept
{
    BOOST_ASSERT(off_ !=
        pt_->offset[
            detail::id_frag]);
    auto const end =
        s_ + pt_->offset[
            detail::id_frag];
    auto const p0 = s_ + off_;
    auto p = p0;
    if(*p == '/')
        ++p;
    while(p < end)
    {
        if(*p == '/')
            break;
        ++p;
    }
    n_ = p - p0;
}

//----------------------------------------------------------

auto
view::
segments_type::
begin() const noexcept ->
    iterator
{
    BOOST_ASSERT(pt_);
    return iterator(this, false);
}

auto
view::
segments_type::
end() const noexcept ->
    iterator
{
    BOOST_ASSERT(pt_);
    return iterator(this, true);
}

//----------------------------------------------------------
//
// params_type
//
//----------------------------------------------------------

view::
params_type::
iterator::
iterator() noexcept
    : s_(nullptr)
    , pt_(nullptr)
    , off_(0)
    , nk_(0)
    , nv_(0)
{
}

view::
params_type::
iterator::
iterator(
    params_type const* v,
    bool end) noexcept
    : s_(v->s_)
    , pt_(v->pt_)
{
    if( end ||
        pt_->nparam == 0)
    {
        off_ = pt_->offset[
            detail::id_frag];
        nk_ = 0;
        nv_ = 0;
    }
    else
    {
        off_ = pt_->offset[
            detail::id_query];
        parse();
    }
}

auto
view::
params_type::
iterator::
operator*() const noexcept ->
    value_type
{
    BOOST_ASSERT(nk_ > 0);
    BOOST_ASSERT(
        off_ == pt_->offset[
            detail::id_query] ?
        s_[off_] == '?' :
        s_[off_] == '&');
    string_view const k = {
        s_ + off_ + 1,
        nk_ - 1 };

    BOOST_ASSERT(nv_ == 0 ||
        s_[off_ + nk_] == '=');
    string_view const v = {
        s_ + off_ + nk_ + 1,
        nv_ - 1};

    return { k, v };
}

auto
view::
params_type::
iterator::
operator++() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != pt_->offset[
            detail::id_frag]);
    off_ = off_ + nv_ + nk_;
    if(off_ == pt_->offset[
        detail::id_frag])
    {
        // end
        nv_ = 0;
        nk_ = 0;
    }
    else
    {
        parse();
    }
    return *this;
}

auto
view::
params_type::
iterator::
operator--() noexcept ->
    iterator&
{
    BOOST_ASSERT(
        off_ != pt_->offset[
            detail::id_query]);
    auto const begin =
        s_ + pt_->offset[
            detail::id_query];
    auto p = s_ + off_;
    while(--p > begin)
    {
        if(*p == '&')
        {
            off_ = p - s_;
            parse();
            return *this;
        }
    }
    BOOST_ASSERT(*p == '?');
    off_ = p - s_;
    return *this;
}

void
view::
params_type::
iterator::
parse() noexcept
{
    auto const end =
        s_ + pt_->offset[
            detail::id_end];
    auto p = s_ + off_;
    BOOST_ASSERT(
        ( off_ == pt_->offset[
            detail::id_query] &&
            *p == '?' ) ||
        ( off_ != pt_->offset[
            detail::id_query] &&
            *p == '&' ));
    auto p0 = p++;
    auto const ek =
        detail::qkey_pct_set();
    error_code ec;
    p = ek.parse(p, end, ec);
    BOOST_ASSERT(! ec);
    nk_ = p - p0;
    if(p == end)
    {
        nv_ = 0;
        return;
    }
    auto const ev =
        detail::qval_pct_set();
    BOOST_ASSERT(*p == '=');
    p0 = p++;
    p = ev.parse(p, end, ec);
    BOOST_ASSERT(! ec);
    nv_ = p - p0;
}

//----------------------------------------------------------

auto
view::
params_type::
begin() const noexcept ->
    iterator
{
    BOOST_ASSERT(pt_);
    return iterator(this, false);
}

auto
view::
params_type::
end() const noexcept ->
    iterator
{
    BOOST_ASSERT(pt_);
    return iterator(this, true);
}

bool
view::
params_type::
contains(string_view key) const noexcept
{
    BOOST_ASSERT(pt_);
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            return true;
    return false;
}

std::size_t
view::
params_type::
count(string_view key) const noexcept
{
    BOOST_ASSERT(pt_);
    std::size_t n = 0;
    for(auto e : *this)
        if(detail::key_equal(
            e.encoded_key(),
            key))
            ++n;
    return n;
}

auto
view::
params_type::
find(string_view key) const noexcept ->
    iterator
{
    BOOST_ASSERT(pt_);
    auto it = begin();
    for(auto const last = end();
        it != last; ++it)
        if(detail::key_equal(
            it->encoded_key(),
            key))
            break;
    return it;
}

std::string
view::
params_type::
operator[](string_view key) const
{
    BOOST_ASSERT(pt_);
    auto const it = find(key);
    if(it == end())
        return "";
    return it->value();
}

} // url
} // boost

#endif
