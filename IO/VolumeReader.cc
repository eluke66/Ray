#include "IO/VolumeReader.h"
#include "Core/Vector.h"
#include <fstream>
#include <iostream>

void swap(short& data)
{
  //data = (data >> 8) | (data << 8);
  char* p = (char*)&data;
  char tmp = p[0];
  p[0] = p[1];
  p[1] = tmp;
}

void
VolumeReader::ReadVolume( const std::string &headername, const Vector &diag,
			  double grid_stepsize,
			  double &world_stepsize, Vector &cellsize, Array3<short> &data )
{
  std::ifstream hdr(headername.c_str());
  std::string volumename;
  hdr >> volumename;
  int nx, ny, nz;
  hdr >> nx >> ny >> nz;
  short offsetvalue;
  hdr >> offsetvalue;
  std::string fileendian;
  hdr >> fileendian;
  if(!hdr){
    std::cerr << "Error reading header: " << headername << '\n';
    exit(1);
  }
  if(fileendian != "BigEndian" && fileendian != "LittleEndian"){
    std::cerr << "Bad data endian: " << fileendian << " in " << headername << "\n";
    exit(1);
  }
  std::cerr << "Reading " << volumename << ": " << nx << 'x' << ny << 'x' << nz << '\n';
  data.resize(nx, ny, nz);
  cellsize = diag * Vector(1./(nx-1), 1./(ny-1), 1./(nz-1));
  std::ifstream in(volumename.c_str()); // 
  in.read(reinterpret_cast<char*>(&data(0,0,0)), nx*ny*nz*sizeof(short));
  if(!in){
    std::cerr << "Error reading data: " << volumename << '\n';
    exit(1);
  }

  world_stepsize = cellsize.length()/pow(3, 1./3.) * grid_stepsize;

  short tmp = 0x1234;
  std::string machineendian;
  if(*reinterpret_cast<char*>(&tmp) == 0x12)
    machineendian = "BigEndian";
  else
    machineendian = "LittleEndian";
  if(machineendian != fileendian){
    for(int i=0;i<nz;i++){
      for(int j=0;j<ny;j++){
        for(int k=0;k<nx;k++){
          swap(data(k,j,i));
          data(k,j,i) += offsetvalue;
        }
      }
    }
  } else {
    for(int i=0;i<nz;i++){
      for(int j=0;j<ny;j++){
        for(int k=0;k<nx;k++){
          data(k,j,i) += offsetvalue;
        }
      }
    }
  }
}
