// Process control method throw_it

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <systemc>

using namespace sc_core;
using std::cout;
using std::endl;

struct i_f: virtual sc_interface
{
  virtual void meth() = 0;
};

struct Chan: i_f, sc_module
{
  int f1;
  Chan(sc_module_name _name)
  {
    f1 = 0;
  }
  virtual void meth() { f1 = 1; }
};

struct my_port: sc_port<i_f>
{
  void bind(i_f& _if)
  {
    sc_assert(false);
  }
};

struct extended_port: my_port
{
  int f2;
  extended_port() { f2 = 0; }
  
  void bind(i_f& _if)
  {
    sc_port<i_f>::bind(_if);
    f2 = 1;
  }
};

struct my_export: sc_export<i_f>
{
  void bind(i_f& _if)
  {
    sc_assert(false);
  }
};

struct extended_export: my_export
{
  int f3;
  extended_export() { f3 = 0; }
  
  void bind(i_f& _if)
  {
    sc_export<i_f>::bind(_if);
    f3 = 1;
  }
};

struct Child: sc_module
{
  extended_port p;
  extended_export xp;
  
  Chan chan;
  
  Child(sc_module_name _name)
  : chan("chan")
  {
    my_export* mxp = static_cast<my_export*>( &xp );
    mxp->bind(chan); // bind should be virtual
    SC_THREAD(T);
  }
  
  void T()
  {
    p->meth();
  }

  SC_HAS_PROCESS(Child);
};

struct Top: sc_module
{
  Child *child;
  Chan chan;
  
  Top(sc_module_name _name)
  : chan("chan")
  {
    child = new Child("child");
    extended_port* ep = &(child->p);
    my_port* mp = static_cast<my_port*>(ep);
    mp->bind(chan); // bind should be virtual
    
    SC_THREAD(T);
  }
  
  void T()
  {
    child->xp->meth();
  }

  SC_HAS_PROCESS(Top);
};

int sc_main(int argc, char* argv[])
{
  Top top("top");
  
  sc_start();
  
  sc_assert( top.chan.f1 );
  sc_assert( top.child->chan.f1 );
  sc_assert( top.child->p.f2 );
  sc_assert( top.child->xp.f3 );
  
  cout << endl << "Success" << endl;
  return 0;
}
  
