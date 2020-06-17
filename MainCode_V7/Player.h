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

    bool hardMode; //Nieuw!
    
  public:
    Player(String u, bool t, String c);
    String getUID();
    String getColor();
    boolean isThief();
    boolean getCarried();
    boolean setCarried(boolean c);

    bool gethardMode(); //Nieuw!
    void sethardMode(bool h); //Nieuw!
};



#endif
