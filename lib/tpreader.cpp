#include "tpreader.hpp"
#include <fstream>
#include <iostream>
using namespace std;

#define DEBUG_TPREADER
//#define TRANSPOSE

void TpReader::read()
{
  ifstream is;
  is.open(m_filename.c_str());
  Eigen::Matrix4d tmp = Eigen::Matrix4d();
  
  int i = 0;
  int j = 0;
  while(is.good())
  {
    is >> tmp(i,j);
    j++;
    if(j == 4)
    {
      i++;
      j = 0;
      if(i == 3)
      {
		tmp(3,0) = 0.0;
		tmp(3,1) = 0.0;
		tmp(3,2) = 0.0;
		tmp(3,3) = 1.0;
		m_matrices.push_back(tmp);
		i = 0;
		j = 0;
      }
    }
  }
}
