#include <iostream>
#include <string>
#include <time.h>
#include <vector>
int main() {
  int coin = 10;  //持ち時間
  time_t start = time(NULL);
  while(1){
    std::string str;
    std::getline(std::cin,str);
    // std::cin.ignore();
    std::string find_word(" ");
    std::string::size_type pos = str.find(find_word);
    std::size_t n = str.size();

    //お金を追加
    if(str == "10yen"){
      coin +=15;
    } if(str == "100yen"){
      coin += 120;
    }if(str == "1000yen"){
      coin += 1000;
    }
    time_t now = time(NULL);
    time_t t = now - start;
    int l = coin -t;
    std::cout<< l <<std::endl;
    if(0 < l && l <= 5){
      std::cout << "お金を入れてください" << std::endl;
    }
    if(l <= 0){
      std::cout << "GOODBYE" << std::endl;
    }

    if(str == "connect" || str=="exit"|| t == coin){
      break;
    } 
    //  if(pos == std::string::npos){
    // std::cout << "not space fond" << std::endl;
    // }
    std::cout << str.substr(0,pos) << std::endl;
  }
}
