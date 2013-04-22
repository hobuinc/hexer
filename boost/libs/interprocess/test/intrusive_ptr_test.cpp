//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Peter Dimov 2002-2005.
// (C) Copyright Ion Gaztanaga 2006-2012. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/smart_ptr/intrusive_ptr.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>

#include <boost/detail/lightweight_test.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <functional>

typedef hexerboost::interprocess::offset_ptr<void> VP;

namespace N
{

class base
{
   private:

   int use_count_;

   base(base const &);
   base & operator=(base const &);

   protected:

   base(): use_count_(0)
   {
   }

   virtual ~base()
   {
   }

   public:

   long use_count() const
   {
      return use_count_;
   }

   void add_ref()
   {
      ++use_count_;
   }

   void release()
   {
      if(--use_count_ == 0) delete this;
   }
};

inline void intrusive_ptr_add_ref(N::base *p)
{  p->add_ref();  }

inline void intrusive_ptr_release(N::base *p)
{  p->release();  }

} // namespace N

struct X: public virtual N::base
{
};

struct Y: public X
{
};

//

namespace n_element_type
{

void f(X &)
{
}

void test()
{
   typedef hexerboost::interprocess::intrusive_ptr<X, VP>::element_type T;
   T t;
   f(t);
}

} // namespace n_element_type

namespace n_constructors
{

void default_constructor()
{
   hexerboost::interprocess::intrusive_ptr<X, VP> px;
   BOOST_TEST(px.get() == 0);
}

void pointer_constructor()
{
   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px(0);
      BOOST_TEST(px.get() == 0);
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px(0, false);
      BOOST_TEST(px.get() == 0);
   }

   {
      hexerboost::interprocess::offset_ptr<X> p = new X;
      BOOST_TEST(p->use_count() == 0);

      hexerboost::interprocess::intrusive_ptr<X, VP> px(p);
      BOOST_TEST(px.get() == p);
      BOOST_TEST(px->use_count() == 1);
   }

   {
      hexerboost::interprocess::offset_ptr<X> p = new X;
      BOOST_TEST(p->use_count() == 0);

      intrusive_ptr_add_ref(p.get());
      BOOST_TEST(p->use_count() == 1);

      hexerboost::interprocess::intrusive_ptr<X, VP> px(p, false);
      BOOST_TEST(px.get() == p);
      BOOST_TEST(px->use_count() == 1);
   }
}

void copy_constructor()
{
   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px;
      hexerboost::interprocess::intrusive_ptr<X, VP> px2(px);
      BOOST_TEST(px2.get() == px.get());
   }

   {
      hexerboost::interprocess::intrusive_ptr<Y, VP> py;
      hexerboost::interprocess::intrusive_ptr<X, VP> px(py);
      BOOST_TEST(px.get() == py.get());
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px(0);
      hexerboost::interprocess::intrusive_ptr<X, VP> px2(px);
      BOOST_TEST(px2.get() == px.get());
   }

   {
      hexerboost::interprocess::intrusive_ptr<Y, VP> py(0);
      hexerboost::interprocess::intrusive_ptr<X, VP> px(py);
      BOOST_TEST(px.get() == py.get());
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px(0, false);
      hexerboost::interprocess::intrusive_ptr<X, VP> px2(px);
      BOOST_TEST(px2.get() == px.get());
   }

   {
      hexerboost::interprocess::intrusive_ptr<Y, VP> py(0, false);
      hexerboost::interprocess::intrusive_ptr<X, VP> px(py);
      BOOST_TEST(px.get() == py.get());
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px(new X);
      hexerboost::interprocess::intrusive_ptr<X, VP> px2(px);
      BOOST_TEST(px2.get() == px.get());
   }

   {
      hexerboost::interprocess::intrusive_ptr<Y, VP> py(new Y);
      hexerboost::interprocess::intrusive_ptr<X, VP> px(py);
      BOOST_TEST(px.get() == py.get());
   }
}

void test()
{
   default_constructor();
   pointer_constructor();
   copy_constructor();
}

} // namespace n_constructors

namespace n_destructor
{

void test()
{
   hexerboost::interprocess::intrusive_ptr<X, VP> px(new X);
   BOOST_TEST(px->use_count() == 1);

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px2(px);
      BOOST_TEST(px->use_count() == 2);
   }

   BOOST_TEST(px->use_count() == 1);
}

} // namespace n_destructor

namespace n_assignment
{

void copy_assignment()
{
}

void conversion_assignment()
{
}

void pointer_assignment()
{
}

void test()
{
   copy_assignment();
   conversion_assignment();
   pointer_assignment();
}

} // namespace n_assignment

namespace n_access
{

void test()
{
   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px;
      BOOST_TEST(px? false: true);
      BOOST_TEST(!px);
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px(0);
      BOOST_TEST(px? false: true);
      BOOST_TEST(!px);
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px
         (hexerboost::interprocess::offset_ptr<X>(new X));
      BOOST_TEST(px? true: false);
      BOOST_TEST(!!px);
      BOOST_TEST(&*px == hexerboost::interprocess::ipcdetail::to_raw_pointer(px.get()));
      BOOST_TEST(px.operator ->() == px.get());
   }
}

} // namespace n_access

namespace n_swap
{

void test()
{
   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px;
      hexerboost::interprocess::intrusive_ptr<X, VP> px2;

      px.swap(px2);

      BOOST_TEST(px.get() == 0);
      BOOST_TEST(px2.get() == 0);

      using std::swap;
      swap(px, px2);

      BOOST_TEST(px.get() == 0);
      BOOST_TEST(px2.get() == 0);
   }

   {
      hexerboost::interprocess::offset_ptr<X> p = new X;
      hexerboost::interprocess::intrusive_ptr<X, VP> px;
      hexerboost::interprocess::intrusive_ptr<X, VP> px2(p);
      hexerboost::interprocess::intrusive_ptr<X, VP> px3(px2);

      px.swap(px2);

      BOOST_TEST(px.get() == p);
      BOOST_TEST(px->use_count() == 2);
      BOOST_TEST(px2.get() == 0);
      BOOST_TEST(px3.get() == p);
      BOOST_TEST(px3->use_count() == 2);

      using std::swap;
      swap(px, px2);

      BOOST_TEST(px.get() == 0);
      BOOST_TEST(px2.get() == p);
      BOOST_TEST(px2->use_count() == 2);
      BOOST_TEST(px3.get() == p);
      BOOST_TEST(px3->use_count() == 2);
   }

   {
      hexerboost::interprocess::offset_ptr<X> p1 = new X;
      hexerboost::interprocess::offset_ptr<X> p2 = new X;
      hexerboost::interprocess::intrusive_ptr<X, VP> px(p1);
      hexerboost::interprocess::intrusive_ptr<X, VP> px2(p2);
      hexerboost::interprocess::intrusive_ptr<X, VP> px3(px2);

      px.swap(px2);

      BOOST_TEST(px.get() == p2);
      BOOST_TEST(px->use_count() == 2);
      BOOST_TEST(px2.get() == p1);
      BOOST_TEST(px2->use_count() == 1);
      BOOST_TEST(px3.get() == p2);
      BOOST_TEST(px3->use_count() == 2);

      using std::swap;
      swap(px, px2);

      BOOST_TEST(px.get() == p1);
      BOOST_TEST(px->use_count() == 1);
      BOOST_TEST(px2.get() == p2);
      BOOST_TEST(px2->use_count() == 2);
      BOOST_TEST(px3.get() == p2);
      BOOST_TEST(px3->use_count() == 2);
   }
}

} // namespace n_swap

namespace n_comparison
{

template<class T, class U, class VP>
void test2(hexerboost::interprocess::intrusive_ptr<T, VP> const & p,
           hexerboost::interprocess::intrusive_ptr<U, VP> const & q)
{
   BOOST_TEST((p == q) == (p.get() == q.get()));
   BOOST_TEST((p != q) == (p.get() != q.get()));
}

template<class T, class VP>
void test3(hexerboost::interprocess::intrusive_ptr<T, VP> const & p,
           hexerboost::interprocess::intrusive_ptr<T, VP> const & q)
{
   BOOST_TEST((p == q) == (p.get() == q.get()));
   BOOST_TEST((p.get() == q) == (p.get() == q.get()));
   BOOST_TEST((p == q.get()) == (p.get() == q.get()));
   BOOST_TEST((p != q) == (p.get() != q.get()));
   BOOST_TEST((p.get() != q) == (p.get() != q.get()));
   BOOST_TEST((p != q.get()) == (p.get() != q.get()));

   // 'less' moved here as a g++ 2.9x parse error workaround
   std::less<hexerboost::interprocess::offset_ptr<T> > less;
   BOOST_TEST((p < q) == less(p.get(), q.get()));
}

void test()
{
   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px;
      test3(px, px);

      hexerboost::interprocess::intrusive_ptr<X, VP> px2;
      test3(px, px2);

      hexerboost::interprocess::intrusive_ptr<X, VP> px3(px);
      test3(px3, px3);
      test3(px, px3);
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px;

      hexerboost::interprocess::intrusive_ptr<X, VP> px2(new X);
      test3(px, px2);
      test3(px2, px2);

      hexerboost::interprocess::intrusive_ptr<X, VP> px3(new X);
      test3(px2, px3);

      hexerboost::interprocess::intrusive_ptr<X, VP> px4(px2);
      test3(px2, px4);
      test3(px4, px4);
   }

   {
      hexerboost::interprocess::intrusive_ptr<X, VP> px(new X);

      hexerboost::interprocess::intrusive_ptr<Y, VP> py(new Y);
      test2(px, py);

      hexerboost::interprocess::intrusive_ptr<X, VP> px2(py);
      test2(px2, py);
      test3(px, px2);
      test3(px2, px2);
   }
}

} // namespace n_comparison

namespace n_static_cast
{

void test()
{
}

} // namespace n_static_cast

namespace n_dynamic_cast
{

void test()
{
}

} // namespace n_dynamic_cast

namespace n_transitive
{

struct X: public N::base
{
   hexerboost::interprocess::intrusive_ptr<X, VP> next;
};

void test()
{
   hexerboost::interprocess::intrusive_ptr<X, VP> p(new X);
   p->next = hexerboost::interprocess::intrusive_ptr<X, VP>(new X);
   BOOST_TEST(!p->next->next);
   p = p->next;
   BOOST_TEST(!p->next);
}

} // namespace n_transitive

namespace n_report_1
{

class foo: public N::base
{
   public:

   foo(): m_self(this)
   {
   }

   void suicide()
   {
      m_self = 0;
   }

   private:

   hexerboost::interprocess::intrusive_ptr<foo, VP> m_self;
};

void test()
{
   hexerboost::interprocess::offset_ptr<foo> foo_ptr = new foo;
   foo_ptr->suicide();
}

} // namespace n_report_1

int main()
{
   n_element_type::test();
   n_constructors::test();
   n_destructor::test();
   n_assignment::test();
   n_access::test();
   n_swap::test();
   n_comparison::test();
   n_static_cast::test();
   n_dynamic_cast::test();

   n_transitive::test();
   n_report_1::test();

   return hexerboost::report_errors();
}

#include <boost/interprocess/detail/config_end.hpp>
