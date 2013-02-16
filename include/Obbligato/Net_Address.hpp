#pragma once
#ifndef Obbligato_Net_Address_hpp
#define Obbligato_Net_Address_hpp

/*
 Copyright (c) 2013, J.D. Koftinoff Software, Ltd. <jeffk@jdkoftinoff.com>
 http://www.jdkoftinoff.com/
 All rights reserved.
 
 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.
 
 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "Obbligato/World.hpp"
#include "Obbligato/Platform.hpp"
#include "Obbligato/IEEE.hpp"
#include "Obbligato/IOStream.hpp"
#include "Obbligato/LexicalCast.hpp"
#include "Obbligato/Net_MAC48.hpp"

namespace Obbligato
{
    namespace Net
    {
        class NetAddress
        {
        private:

			sockaddr_storage m_storage;

        public:
            /// Create an AF_UNSPEC unspecified address
            NetAddress();

            /// Create a MAC48 address
            NetAddress( MAC48 const &v );

            /// Create an address based on the first item in an addrinfo list
            NetAddress( addrinfo const *ai );

            /// Create an address based on a opaque sockaddr
            NetAddress( sockaddr const *addr );

#if defined(__linux__)
            /// Create an address based on a linux link layer address
            NetAddress( sockaddr_ll const *addr );
#elif defined(__APPLE__)
            /// Create an address based on a netbsd link layer address
            NetAddress( sockaddr_dl const *addr );
#elif defined(WIN32) && defined(AF_LINK)
            /// Create an address based on a link layer address
            NetAddress( sockaddr_dl const *addr );
#endif

            /// Create an address based on an ipv4 sockaddr_in
            NetAddress( sockaddr_in const *addr );

            /// Create an address based on an ipv6 sockaddr_in6
            NetAddress( sockaddr_in6 const *addr );

            /// Create an address from an ascii string, with family as a hint
            NetAddress( std::string addrstring, int family=AF_UNSPEC );

            /// copy an address
            NetAddress( NetAddress const &other ) : m_storage( other.m_storage )
            {
            }

            /// Swap the contents of an address with an other
            void swap( NetAddress &other )
            {
                std::swap(m_storage,other.m_storage);
            }

            /// assign one address to another
            NetAddress const &operator = ( NetAddress const &other )
            {
                assign(other);
                return *this;
            }

            /// assign one address to another
            void assign( NetAddress other )
            {
                swap(other);
            }

            /// Return true if the address is a MAC48 address
            bool is_mac48() const;

            /// Get the MAC48 address
            MAC48 get_mac48() const;

            /// Return true if the address is an ipv4 address
            bool is_ipv4() const;

            /// Get the ipv4 address
            sockaddr_in get_ipv4() const;

            /// Return true if the address is an ipv6 address
            bool is_ipv6() const;

            /// Get the ipv6 address
            sockaddr_in6 get_ipv6() const;

            /// Get the address family (AF_LINK,AF_INET,AF_INET6)
            int get_sa_family() const
            {
                return m_storage.ss_family;
            }

            /// Get the opaque sockaddr (const)
            sockaddr const *get_sockaddr() const
            {
                return reinterpret_cast<sockaddr const *>(&m_storage);
            }

            /// Get the opaque sockaddr
            sockaddr *get_sockaddr()
            {
                return reinterpret_cast<sockaddr *>(&m_storage);
            }

            /// Get the addresslen
            socklen_t get_addrlen(sockaddr const *sa) const;

            /// Get the addresslen
			socklen_t get_addrlen() const
			{
				return get_addrlen((sockaddr const *)&m_storage);
			}

            /// Convert the address to an ascii string
            std::string to_string() const;

            /// Assign the address froma an ascii string, with family set to a hint or AF_UNSPEC to attempt auto parse
            void from_string( std::string const &s, int family=AF_UNSPEC );

            /// Assign the address from an ipv4 address ascii string
			bool from_string_ipv4( std::string const &s );

            /// Assign the address from an ipv6 address ascii string
			bool from_string_ipv6( std::string const &s );

            /// Assign the address from an MAC48 address ascii string
            bool from_string_mac48( std::string const &s );

            /// Compare the address with an other address, with comparison of family as well. Return -1, 0, or 1
            int compare( NetAddress const &other ) const
            {
                int r=0;

                if(     get_sa_family() == other.get_sa_family() &&
                        get_addrlen() == other.get_addrlen()
                        )
                {
                    r=memcmp(get_sockaddr(), other.get_sockaddr(), get_addrlen());
                }
                else if( get_sa_family() < other.get_sa_family() )
                {
                    r=-1;
                }
                else if( get_sa_family() > other.get_sa_family() )
                {
                    r=1;
                }
                else
                {
                    r=0;
                }

                return r;
            }

            /// Equality test for address
            inline friend bool operator == ( NetAddress const &a, NetAddress const &b )
            {
                return a.compare(b) == 0;
            }

            /// Inequality test for address
            inline friend bool operator != ( NetAddress const &a, NetAddress const &b )
            {
                return a.compare(b) != 0;
            }

            /// Less than test for address
            friend bool operator < ( NetAddress const &a, NetAddress const &b )
            {
                return a.compare(b) < 0;
            }

            /// Less than or equal test for address
            friend bool operator <= ( NetAddress const &a, NetAddress const &b )
            {
                return a.compare(b) <= 0;
            }

            /// Greater than test for address
            friend bool operator > ( NetAddress const &a, NetAddress const &b )
            {
                return a.compare(b) > 0;
            }


            /// Greater than or equal test for address
            friend bool operator >= ( NetAddress const &a, NetAddress const &b )
            {
                return a.compare(b) >= 0;
            }
        };

        template <typename T>
        class NetAddressFormatter
        {
        public:
            T const &m_value;
            NetAddressFormatter( T const &v )
                : m_value(v)
            {
            }
        };

        template <typename T>
        class NetAddressUnformatter
        {
        public:
            T &m_value;
            NetAddressUnformatter( T &v )
                : m_value(v)
            {
            }
        };

        template <typename Ch, typename Tr>
        inline std::basic_ostream<Ch,Tr> & operator << (std::basic_ostream<Ch,Tr> &o, Net::NetAddressFormatter<NetAddress> const &f )
        {
            o << f.m_value.to_string();
            return o;
        }

        template <typename Ch, typename Tr>
        inline std::basic_istream<Ch,Tr> & operator >> (std::basic_istream<Ch,Tr> &i, Net::NetAddressUnformatter<NetAddress> f )
        {
            std::string s;
            i >> s;
            f.m_value.from_string(s);
            return i;
        }

    }

    namespace IOStream
    {
        template <>
        struct DefaultFormat< Net::NetAddress >
        {
            typedef Net::NetAddressFormatter<Net::NetAddress> formatter_type;
            typedef Net::NetAddressUnformatter<Net::NetAddress> unformatter_type;
        };

        template <typename T>
        inline Net::NetAddressFormatter<T> netaddress_fmt( T const &v )
        {
            return Net::NetAddressFormatter<T>(v);
        }

        template <typename T>
        inline Net::NetAddressUnformatter<T> netaddress_unfmt( T &v )
        {
            return Net::NetAddressUnformatter<T>(v);
        }

    }

    void to_string( std::string &to, Net::NetAddress const &o );
    std::ostream & operator <<(std::ostream& o, Net::NetAddress const &v);
}


#endif

