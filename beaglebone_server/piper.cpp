#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <vector>
#include <math.h>
#include <cstdlib>
#define LINE_OFFSET 5 // we start counting from the timestamp.
using namespace std;

/*
 *
 *
 * Spells that we're doing:
 *
 * - Nox
 * - Lumos
 * - Expelliarmus
 * - Stupify
 * - oppugno
 * - Arresto Momento
 *
 */

/* globals. Already tuned. Do not change unless confident! */

unsigned int tau =150; // 100 is 1 sec
unsigned int minitau =70;
unsigned int expelliarmus_tau = 250;
signed short int z_up_thresh = 1500.0f;//1400.0f;
signed short int z_down_thresh = 100.0f;
signed short int xy_idle_thresh = 900.0f;

signed short int x_right_thresh = 1000.0f;//500.0f;
signed short int x_left_thresh = -200.0f;//-400.0f;
signed short int yz_idle_thresh = 1500.0f;
signed short int z_idle_thresh_expelliarmus= yz_idle_thresh * 1.4;
              
vector<string> split(const string &s, char delim) {
    stringstream ss(s);
    string item;
    vector<string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}
signed short int getXAccel(vector<string>& tokens){
  stringstream ss;
  ss << std::hex << tokens[LINE_OFFSET + 3]+tokens[LINE_OFFSET + 2];
  int d;
  ss >> d;
  return static_cast<signed short int>(d);
}

signed short int getYAccel(vector<string>& tokens){
  stringstream ss;
  ss << std::hex << tokens[LINE_OFFSET + 5]+tokens[LINE_OFFSET + 4];
  int d;
  ss >> d;
  return static_cast<signed short int>(d);
}
signed short int getZAccel(vector<string>& tokens){
  stringstream ss;
  ss << std::hex << tokens[LINE_OFFSET + 7]+tokens[LINE_OFFSET + 6];
  int d;
  ss >> d;
  return static_cast<signed short int>(d);

}

unsigned short int getTime(vector<string>& tokens){
  stringstream ss;
  ss << std::hex << tokens[LINE_OFFSET + 1]+tokens[LINE_OFFSET + 0];
  int d;
  ss >> d;
  return static_cast<unsigned short int>(d);

}
int daniel(string word){
    string w = "/usr/bin/festival -b '(voice_cmu_us_slt_arctic_hts)' '(SayText \" ";
    w.append(word);
    w.append("\")' &");
	return system(w.c_str());
}


int state = 0;
int previous_state = 0;

/* CODE to change state */
void changeState(int newState){
    previous_state= state;
    state = newState;
    cout << "STATECHANGE " << newState << endl;
switch(state){
	case 3:
        /* NOX */
        daniel("NOX");
        cout<<"SUCCESSFUL " << "NOX" << endl;;
        break;
	case 4:
        /* LUMOS*/
        daniel("LUMOS");
        cout<<"SUCCESSFUL " << "LUMOS" << endl;;
        break;
	case 6:
        /* EXPELLIARMUS */
        daniel("EXPELLIARMUS");
        cout<<"SUCCESSFUL " << "EXPELLIARMUS" << endl;;
        break;
	case 9:
        /* STUPIFY */
        daniel("STUPIFY");
        cout<<"SUCCESSFUL " << "STUPIFY" << endl;;
        break;
	case 10:
        /* APARECUM */
        daniel("APARECUM");
        cout<<"SUCCESSFUL " << "APARECUM" << endl;;
    default:
    break;
    }
}
int main(){
    daniel("starting beaglebone C++ .");
    string line;
    unsigned short int msTick = 0;
    int msTickStateChange = 0;
    while(getline(cin,line)){
        vector<string> tokens = split(line,' ');
        if(tokens.size() < 10)
            continue;
        cerr<<"piper.cpp: "<<"time: "<<getTime(tokens);
		signed short int x = getXAccel(tokens);
		signed short int y = getYAccel(tokens);
		signed short int z = getZAccel(tokens);
        cerr<<", got x accel: "<<getXAccel(tokens);
        cerr <<", got y accel: "<<getYAccel(tokens);
        cerr<< ", got z accel: "<<getZAccel(tokens);
        cerr<< ", state: "<<state<<endl;
        msTick = getTime(tokens);
         if(state == 0 && (z > z_up_thresh) && fabs(x) < xy_idle_thresh && fabs(y) < xy_idle_thresh){
              cerr<<"piper.cpp:"<<" STATE 0 -> STATE 1 "<<endl;
              changeState(1);
              msTickStateChange = msTick;

          }else if (state == 0 && z < z_down_thresh && fabs(x) < xy_idle_thresh && fabs(y) < xy_idle_thresh){
              cerr<<"piper.cpp:"<<" STATE 0 -> STATE 2 "<<endl;
              changeState(2);
              msTickStateChange = msTick;

          }else if (state == 1 && z < 0.05*z_up_thresh){
              cerr<<"piper.cpp:"<<" STATE 1 -> STATE 3. NOXXXX"<<endl;
              msTickStateChange = msTick;
              changeState(3);
              //TODO LED ON for NOX

          }else if (state == 2 && z > 0.05*z_up_thresh){
              cerr<<"piper.cpp:"<<" STATE 2 -> STATE 4. LUMOSS "<<endl;
              msTickStateChange = msTick;
              changeState(4);
              //TODO LED ON for LUMOS
              
          }else if ((state == 2 || state == 1)
                  && ((fabs(y) > xy_idle_thresh || fabs(x) > xy_idle_thresh)
                          ||  (( msTick - msTickStateChange) > tau))
            ){
              //xy non idling. go back to original s0.
              cerr<<"piper.cpp:"<<" STATE "<<state<<"going back to 0 .  "<<endl;
              msTickStateChange = msTick;
              changeState(0);

          }else if ((state == 3 || state == 4 || state == 6) && (( msTick - msTickStateChange) > tau)){
              cerr<<"piper.cpp:"<<" STATE "<<state<<"going back to 0 .  "<<endl;
              msTickStateChange = msTick;
              changeState(0);

              // TODO: reuse the below to go from 0 to 5 OR stupify.
          }else if((state == 0 || state == /* STUPIFY AFTER DIAG DOWN STATE */7  )&& x >= x_right_thresh && fabs(y) < yz_idle_thresh && fabs(z) < yz_idle_thresh){
              msTickStateChange = msTick;
              if (state == 7) {
                  changeState(8);
                  cerr<<"piper.cpp:"<<" STATE 7 going to STATE 8. "<<endl;
              }
              else{
                  changeState(5);
                  cerr<<"piper.cpp:"<<" STATE 0 going to STATE 5.  "<<endl;
              }

          }else if(state == 5 && z < z_down_thresh && fabs(x) < xy_idle_thresh && fabs(y) < xy_idle_thresh ){
              cerr<<"piper.cpp:"<<" STATE 5 going to STATE 6. EXPELLIARMUSSSS  "<<endl;
              msTickStateChange = msTick;
              changeState(6);
              //  //TODO LED ON for EXPELLIARMUS

          }else if ((state ) == 5 && ((fabs(y) > yz_idle_thresh || fabs(z) >  z_idle_thresh_expelliarmus)|| (( msTick - msTickStateChange) > expelliarmus_tau))){
                                            // NOTEDIFF: z idling condition above is made higher.. This is because expelliarmus typically will need some z motion
              cerr<<"piper.cpp:"<<" STATE 5 going to STATE 0. EXPELLIARMUS failed."<<endl;
              msTickStateChange = msTick;
              changeState(0);
          }else if ((state) != 0  && state != 5 &&( msTick - msTickStateChange) > tau) {
              cerr<<"piper.cpp:"<<" STATE "<< state<< " going to STATE 0. ."<<endl;
              msTickStateChange = msTick;
              changeState(0);
          
          }else if ((state) == 8 &&( msTick - msTickStateChange) > minitau) {
              cerr<<"piper.cpp:"<<" STATE 8 going to STATE 9: STUPIFY!!!."<<endl;
              msTickStateChange = msTick;
              changeState(9);
          }else if ((state) == 0 && x< x_left_thresh && z< z_down_thresh && fabs(y) < yz_idle_thresh) {
              cerr<<"piper.cpp:"<<" STATE 0 going to STATE 7 ."<<endl;
              msTickStateChange = msTick;
              changeState(7);

          }else if ((state) == 8 && x < x_left_thresh && z > z_up_thresh) {
              cerr<<"piper.cpp:"<<" STATE 8 going to STATE 10: APARECUM!!!."<<endl;
              msTickStateChange = msTick;
              changeState(10);

            }
    }
}
