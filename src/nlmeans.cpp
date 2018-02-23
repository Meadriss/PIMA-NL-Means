#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include "CImg.h"
#include "utils.h"
using namespace cimg_library;
using namespace std;
using namespace utils;


CImg<unsigned char> zeroPadding(const CImg<unsigned char>& I,int patchSize){
  int n = I.width();
  int m = I.height();
  CImg<unsigned char> F(n+patchSize,m+patchSize,1,I.spectrum(),0);
  F.rand(0,255);
  F.draw_image((patchSize/2),(patchSize/2),0,0,I);
  return F;
}

unsigned char neighborMeans(CImg<unsigned char>& I, int x, int y, int size){
  vector<float> nbh = getGrayScaleVector(I,x,y,size);
  float somme = 0;
  for (auto& n : nbh){
    somme += n;
  }
  return round((somme/(size*size)));
}

CImg<unsigned char> means(CImg<unsigned char>& I, int patchSize){
  CImg<unsigned char> F(zeroPadding(I,patchSize));
  int n = I.width();
  int m = I.height();
  for(int i = (patchSize/2); i < n+(patchSize/2); i++){
    for(int j = (patchSize/2); j < m+(patchSize/2); j++){
      F(i,j) = neighborMeans(F,i,j,patchSize);
    }
  }
  return F;
}

CImg<unsigned char> nlMeans(CImg<unsigned char>& I, int patchSize){
  CImg<unsigned char> F(zeroPadding(I,patchSize));
  int n = I.width();
  int m = I.height();
  return F;
}


int main(int argc, char ** argv){
  int patchSize = 5;
  char *im;
  int noi = 0;
  for(int i = 1; i < argc; i++){
    string arg = argv[i];
    if (arg.compare("-i") == 0){
      i++;
      im = argv[i];
      cout << "l'image : "<< im << endl;
      continue;
    }
    if (arg.compare("-s") == 0){
      i++;
      patchSize = atoi(argv[i]);
      cout << "la taille du patch est : "<< patchSize << endl;
      continue;
    }
    if (arg.compare("-n") == 0){
      i++;
      noi = atoi(argv[i]);
      continue;
    }
  }
  CImg<unsigned char> image(im);
  /* noise(sigma,noise_type) 
   *0 -> gaussian, 1 -> uniform, 2 -> Salt & Pepper,3 -> Poisson, 4 -> Rician
   */
  image.noise(4,noi);
  CImg<unsigned char> filter = means(image,patchSize);
  CImgDisplay main_display(filter,"nlMeans");
  while(!main_display.is_closed()){
    main_display.wait();
  }
  return 0;
}
