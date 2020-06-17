#include "Arduino.h"
#include "Player.h"

Player::Player(String u, bool t, String c) {
  uid = u;
  color = c;
  thief = t;
}

String Player::getUID() {
  return uid;
}

String Player::getColor() {
  return color;
}

boolean Player::isThief() {
  return thief;
}

boolean Player::getCarried() {
  return carried;
}

boolean Player::setCarried(boolean c) {
  carried = c;
}

bool Player::gethardMode() {  //Nieuw!
  return hardMode;
}

void Player::sethardMode(bool h) {  //Nieuw!!
  hardMode = h;
}
