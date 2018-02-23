#include <iostream>

#include "CImg.h"
using namespace cimg_library;
using namespace std;

void meansFilter(CImg<unsigned char> I){
  CImg<unsigned char> dest(I,false);
  CImg<> N(5,5);
  cimg_forC(I,k)
    cimg_for5x5(I,x,y,0,k,N,float)
    dest(x,y,k) = N.sum()/(5*5);
  CImgList<unsigned char> visu(I,dest);
  visu.display("Original + Filtered");
}

int main(){
  CImg<unsigned char> image("lena.jpg");
  //CImgDisplay disp(image,"test color");
  image.channel(0);
  //image(10,10,0) = 0;
  //cout << unsigned(image(10,10,0)) << " = couleur pixel en (10,10)" <<endl;
  //CImgDisplay main_disp(image,"Hello just a print");

  //while(!main_disp.is_closed() && !disp.is_closed()){
  //  main_disp.wait();
  //  disp.wait();
  //}
  image.noise(20,1);
  meansFilter(image);
  return 0;
}
