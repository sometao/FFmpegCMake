#include <iostream>

namespace lxh {
namespace hello {
extern int launch();
}

namespace decoder {
extern int launch();
}

namespace player_sdl2 {
extern int launch();
}



}  // namespace lxh


namespace tmpTest {
  int run();
}



int main(int argc, char* argv[]) { 

  lxh::player_sdl2::launch();
  //tmpTest::run();
  return 0; 

}