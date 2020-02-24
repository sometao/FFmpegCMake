#include <iostream>

namespace lxh {
namespace hello {
extern int launch();
}

namespace decoder {
extern int launch();
}



}  // namespace lxh


namespace tmpTest {
  int run();
}



int main(int argc, char* argv[]) { 

  //lxh::decoder::launch();
  tmpTest::run();
  return 0; 

}