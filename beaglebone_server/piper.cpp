#include <string>
#include <iostream>
#include <sstream>
#include <istream>
#include <vector>
#include <math.h>
#include <cstdlib>
#define LINE_OFFSET 5 // we start counting from the timestamp.
using namespace std;

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
int daniel(int state){
	switch(state) {
	case 3:
	return system("/usr/bin/festival -b '(voice_cmu_us_slt_arctic_hts)' '(SayText \" NOX. \")' &");
	case 4:
	return system("/usr/bin/festival -b '(voice_cmu_us_slt_arctic_hts)' '(SayText \" LUMOS. \")' &");
	case 6:
	return system("/usr/bin/festival -b '(voice_cmu_us_slt_arctic_hts)' '(SayText \" EXPELLIARMUS. \")' &");
	case 9000:
	return system("/usr/bin/festival -b '(voice_cmu_us_slt_arctic_hts)' '(SayText \" whats up.. \")' &");
default:
	cerr<<"piper.cpp:"<<"INVALID DANIEL COMMAND. "<<endl;
    return 1;
}
}


/* globals. Already tuned. Do not change unless confident! */
unsigned int tau =150;
signed short int z_up_thresh = 1400.0f;
signed short int z_down_thresh = 100.0f;
signed short int xy_idle_thresh = 900.0f;

signed short int x_right_thresh = 500.0f;
signed short int yz_idle_thresh = 1500.0f;
              
int state = 0;

/* CODE to change state */
void changeState(int newState){
    state = newState;
    cout << "STATECHANGE " << newState << endl;
switch(state){
	case 3:
        /* NOX */
        daniel(3);
        cout<<"SUCCESSFUL " << "NOX" << endl;;
        break;
	case 4:
        /* LUMOS*/
        daniel(4);
        cout<<"SUCCESSFUL " << "LUMOS" << endl;;
        break;
	case 6:
        /* EXPELLIARMUS */
        daniel(6);
        cout<<"SUCCESSFUL " << "LUMOS" << endl;;
        break;
    default:
    break;
    }
}
int main(){
	daniel(9000);
    string line;
    unsigned short int msTick = 0;
    int msTickStateChange = 0;
    while(getline(cin,line)){
        vector<string> tokens = split(line,' ');
        if(tokens.size() < 10)
            continue;
        cerr<<"piper.cpp:"<<"time: "<<getTime(tokens);
		signed short int x = getXAccel(tokens);
		signed short int y = getYAccel(tokens);
		signed short int z = getZAccel(tokens);
        cerr<<"piper.cpp:"<<", got x accel : "<<getXAccel(tokens);
        cerr<<"piper.cpp:"<<" , got y accel : "<<getYAccel(tokens);
        cerr<<"piper.cpp:"<< " , got z accel : "<<getZAccel(tokens);
        cerr<<"piper.cpp:"<< " , state :  "<<state<<endl;
        msTick = getTime(tokens);
         if(state == 0 && (z > z_up_thresh) && fabs(x) < xy_idle_thresh && fabs(y) < xy_idle_thresh){
              cerr<<"piper.cpp:"<<" STATE 0 -> STATE 1 "<<endl;
              // state = 1;
              changeState(1);
              msTickStateChange = msTick;
          }else if (state == 0 && z < z_down_thresh && fabs(x) < xy_idle_thresh && fabs(y) < xy_idle_thresh){
              cerr<<"piper.cpp:"<<" STATE 0 -> STATE 2 "<<endl;
              //state = 2;
              changeState(2);
              msTickStateChange = msTick;
          }else if (state == 1 && z < 0.05*z_up_thresh){
              cerr<<"piper.cpp:"<<" STATE 1 -> STATE 3. NOXXXX"<<endl;
              msTickStateChange = msTick;
              // state = 3;
              changeState(3);
              //TODO LED ON for NOX
            //daniel(state);
          }else if (state == 2 && z > 0.05*z_up_thresh){
              cerr<<"piper.cpp:"<<" STATE 2 -> STATE 4. LUMOSS "<<endl;
              msTickStateChange = msTick;
              // state = 4;
              changeState(4);
              //TODO LED ON for LUMOS
            //daniel(state);
          }else if ((state == 2 || state == 1)
                  && ((fabs(y) > xy_idle_thresh || fabs(x) > xy_idle_thresh)
                          ||  (( msTick - msTickStateChange) > tau))
            ){
              //xy non idling. go back to original s0.
              cerr<<"piper.cpp:"<<" STATE "<<state<<"going back to 0 .  "<<endl;
              msTickStateChange = msTick;
              changeState(0);
              // state = 0;
          }else if ((state == 3 || state == 4 || state == 6) && (( msTick - msTickStateChange) > tau)){
              cerr<<"piper.cpp:"<<" STATE "<<state<<"going back to 0 .  "<<endl;
              msTickStateChange = msTick;
              // state = 0;
              changeState(0);
          }else if(state == 0 && x >= x_right_thresh && fabs(y) < yz_idle_thresh && fabs(z) < yz_idle_thresh){
              cerr<<"piper.cpp:"<<" STATE 0 going to STATE 5.  "<<endl;
              msTickStateChange = msTick;
              // state = 5;
              changeState(5);
          }else if(state == 5 && z < z_down_thresh && fabs(x) < xy_idle_thresh && fabs(y) < xy_idle_thresh ){
              cerr<<"piper.cpp:"<<" STATE 5 going to STATE 6. EXPELLIARMUSSSS  "<<endl;
              msTickStateChange = msTick;
              changeState(6);
              // state = 6;
              //  //TODO LED ON for EXPELLIARMUS
              //daniel(state);
          }else if ((state ) == 5 && ((fabs(y) > yz_idle_thresh || fabs(z) > yz_idle_thresh) || (( msTick - msTickStateChange) > tau))){
              cerr<<"piper.cpp:"<<" STATE 5 going to STATE 0. EXPELLIARMUS failed."<<endl;
              msTickStateChange = msTick;
              changeState(0);
              // state = 0;
          }
    }
}
