#include "philo.hh"


namespace its {

  class Philo : public GAL {
  public:
    Philo():GAL("philo") {};

    

  };


  GAL * createGAL() {
    return new Philo();
  }

}
