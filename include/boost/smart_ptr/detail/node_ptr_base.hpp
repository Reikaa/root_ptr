/**
    @file
    Boost node_ptr_base.hpp header file.

    @note
    Copyright (c) 2003 - 2008 Phil Bouchard <pbouchard8@gmail.com>.
    Copyright (c) 2001 - 2007 Peter Dimov

    Distributed under the Boost Software License, Version 1.0.

    See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt

    See http://www.boost.org/libs/smart_ptr/doc/index.html for documentation.
*/


#ifndef BOOST_DETAIL_NODE_PTR_BASE_HPP
#define BOOST_DETAIL_NODE_PTR_BASE_HPP


#include <boost/smart_ptr/detail/classof.hpp>
#include <boost/smart_ptr/detail/node_base.hpp>


namespace boost
{

namespace smart_ptr
{

namespace detail
{


/**
    Smart pointer optimized for speed and memory usage.
    
    This class represents a basic smart pointer interface.
*/

template <typename T>
    class node_ptr_common
    {
        template <typename> friend class node_ptr_common;

        // Borland 5.5.1 specific workaround
        typedef node_ptr_common<T> this_type;

    protected:
        typedef T value_type;
        //typedef node<value_type> element_type;

        value_type * po_;

    public:
        node_ptr_common() 
        : po_(0)
        {
        }

        ~node_ptr_common()
        {
            if (po_)
            {
                header()->release();
            }
        }

        template <typename V, typename PoolAllocator>
            node_ptr_common(node<V, PoolAllocator> * p) 
            : po_(p->element())
            {
            }

        template <typename V>
            node_ptr_common(node_ptr_common<V> const & p) 
            : po_(p.share())
            {
            }

            node_ptr_common(node_ptr_common<value_type> const & p) 
            : po_(p.share())
            {
            }

        template <typename V, typename PoolAllocator>
            node_ptr_common & operator = (node<V> * p)
            {
                reset(p->element());
                return * this;
            }
            
        template <typename V>
            node_ptr_common & operator = (node_ptr_common<V> const & p)
            {
                if (p.po_ != po_)
                {
                    reset(p.share());
                }
                return * this;
            }

            node_ptr_common & operator = (node_ptr_common<value_type> const & p)
            {
                return operator = <value_type>(p);
            }

        value_type * get() const
        {
            return po_;
        }

        value_type * share() const
        {
            if (po_)
            {
                header()->add_ref_copy();
            }
            return po_;
        }

        void reset(value_type * p = 0)
        {
            if (po_)
            {
                header()->release();
            }
            po_ = p;
        }

#if ( defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, < 0x570) ) || defined(__CINT__)
        operator bool () const
        {
            return po_ != 0;
        }
#elif defined( _MANAGED )
        static void unspecified_bool( this_type*** )
        {
        }

        typedef void (*unspecified_bool_type)( this_type*** );

        operator unspecified_bool_type() const // never throws
        {
            return po_ == 0? 0: unspecified_bool;
        }
#elif \
        ( defined(__MWERKS__) && BOOST_WORKAROUND(__MWERKS__, < 0x3200) ) || \
        ( defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__ < 304) ) || \
        ( defined(__SUNPRO_CC) && BOOST_WORKAROUND(__SUNPRO_CC, <= 0x590) )

        typedef value_type * (this_type::*unspecified_bool_type)() const;
        
        operator unspecified_bool_type() const // never throws
        {
            return po_ == 0? 0: &this_type::get;
        }
#else 
        typedef value_type * this_type::*unspecified_bool_type;

        operator unspecified_bool_type() const // never throws
        {
            return po_ == 0? 0: &this_type::po_;
        }
#endif

        // operator! is redundant, but some compilers need it

        bool operator! () const // never throws
        {
            return po_ == 0;
        }

        long use_count() const // never throws
        {
            return header()->use_count();
        }

    protected:
        node_base * header() const
        {
            return static_cast<node_base *>
            (
                (typename node_element<value_type>::classof)
                (
                    static_cast<value_type *>
                    (
                        rootof<is_polymorphic<value_type>::value>::get
                        (
                            po_
                        )
                    )
                )
            );
        }
    };


template <typename T>
    class node_ptr_base : public node_ptr_common<T>
    {
        typedef node_ptr_common<T> base;
        typedef typename base::value_type value_type;
        
    protected:
        using base::po_;

    public:
        node_ptr_base() 
        : base()
        {
        }

        template <typename V, typename PoolAllocator>
            node_ptr_base(node<V, PoolAllocator> * p) 
            : base(p)
            {
            }

        template <typename V>
            node_ptr_base(node_ptr_base<V> const & p) 
            : base(p)
            {
            }

            node_ptr_base(node_ptr_base<value_type> const & p) 
            : base(p)
            {
            }

        template <typename V, typename PoolAllocator>
            node_ptr_base & operator = (node<V, PoolAllocator> * p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

        template <typename V>
            node_ptr_base & operator = (node_ptr_base<V> const & p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

            node_ptr_base & operator = (node_ptr_base<value_type> const & p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

        value_type & operator * () const
        {
            return * po_;
        }

        value_type * operator -> () const
        {
            return po_;
        }
    };


#if !defined(_MSC_VER)
template <typename T, size_t N>
    class node_ptr_base<T [N]> : public node_ptr_common<T [N]>
    {
        typedef node_ptr_common<T [N]> base;
        typedef typename base::value_type value_type;

    protected:
        using base::po_;

    public:
        node_ptr_base() 
        : base()
        {
        }

        template <typename V, typename PoolAllocator>
            node_ptr_base(node<V, PoolAllocator> * p) 
            : base(p)
            {
            }

        template <typename V>
            node_ptr_base(node_ptr_base<V> const & p) 
            : base(p)
            {
            }

            node_ptr_base(node_ptr_base<value_type> const & p) 
            : base(p)
            {
            }

        template <typename V, typename PoolAllocator>
            node_ptr_base & operator = (node<V, PoolAllocator> * p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

        template <typename V>
            node_ptr_base & operator = (node_ptr_base<V> const & p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

            node_ptr_base & operator = (node_ptr_base<value_type> const & p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

        T & operator [] (std::size_t n)
        {
            return * (* po_ + n);
        }

        T const & operator [] (std::size_t n) const
        {
            return * (* po_ + n);
        }
    };
#endif


template <>
    class node_ptr_base<void> : public node_ptr_common<void>
    {
        typedef node_ptr_common<void> base;
        typedef typename base::value_type value_type;

    protected:
        using base::po_;

    public:
        node_ptr_base() 
        : base()
        {
        }

        template <typename V, typename PoolAllocator>
            node_ptr_base(node<V, PoolAllocator> * p) 
            : base(p)
            {
            }

        template <typename V>
            node_ptr_base(node_ptr_base<V> const & p) 
            : base(p)
            {
            }

            node_ptr_base(node_ptr_base<value_type> const & p) 
            : base(p)
            {
            }

        template <typename V, typename PoolAllocator>
            node_ptr_base & operator = (node<V, PoolAllocator> * p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

        template <typename V>
            node_ptr_base & operator = (node_ptr_base<V> const & p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }

            node_ptr_base & operator = (node_ptr_base<value_type> const & p)
            {
                return static_cast<node_ptr_base &>(base::operator = (p));
            }
    };


} // namespace detail

} // namespace smart_ptr

} // namespace boost


#endif
