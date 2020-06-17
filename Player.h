/*


*/
#include "Arduino.h"
#ifndef Player_H
#define Player_H


class Player {
  private:
    String uid;
    boolean thief;
    String color;
    boolean carried; 
    
  public:
    Player(String u, bool t, String c);
    String getUID();
    String getColor();
    boolean isThief();
    boolean getCarried();
    boolean setCarried(boolean c);
};



#endif
