
#include <map>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>



// class person;
class person;
typedef boost::shared_ptr<person>   spPerson;
typedef boost::function<void(const spPerson&)>   Rcv1CallBack;
// class game;


