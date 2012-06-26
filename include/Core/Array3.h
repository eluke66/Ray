
#ifndef Array3_h
#define Array3_h

template<typename T>
class Array3 {
 public:
  Array3() {
    nx = ny = nz = extra = 0;
    xidx = yidx = zidx = 0;
    data = 0;
    _refCount = new int; *_refCount = 1;
  }
  Array3(int nx, int ny, int nz, int extra = 0 ) {
    xidx = yidx = zidx = 0;
    data = 0;
    resize(nx, ny, nz, extra);
    _refCount = new int; *_refCount = 1;
  }
  Array3(const Array3&a) : nx(a.nx), ny(a.ny), nz(a.nz), extra(a.extra),
			   xidx(a.xidx), yidx(a.yidx), zidx(a.zidx),
			   data(a.data),  _refCount(a._refCount)
  {
    (*_refCount)++;
  }
  
  Array3& operator=(const Array3& a) {
    if ( a == *this )
      return *this;

    nx = a.nx; ny = a.ny; nz = a.nz; extra = a.extra;
    xidx = a.xidx; yidx = a.yidx; zidx = a.zidx;
    data = a.data; _refCount = a._refCount;

    (*_refCount)++;
    return *this;
  }
  
  ~Array3() {
    --(*_refCount);
    if ( *_refCount == 0 )
      resize(0, 0, 0);
  }

  void resize(int newnx, int newny, int newnz, int newextra = 0) {
    if(xidx)
      delete[] xidx;
    if(yidx)
      delete[] yidx;
    if(zidx)
      delete[] zidx;
    if(data)
      delete[] data;
    nx = newnx; ny = newny; nz = newnz; extra = newextra;
    int total = nx*ny*nz+extra;
    if(total != 0){
      data = new T[total];
      allocateAndInitializeStride(xidx, nx, 1);
      allocateAndInitializeStride(yidx, ny, nx);
      allocateAndInitializeStride(zidx, nz, nx*ny);
    } else {
      xidx = yidx = zidx = 0;
      data = 0;
    }
  }

  void resizeZMajor(int newnx, int newny, int newnz, int newextra = 0) {
    if(xidx)
      delete[] xidx;
    if(yidx)
      delete[] yidx;
    if(zidx)
      delete[] zidx;
    if(data)
      delete[] data;
    nx = newnx; ny = newny; nz = newnz; extra = newextra;
    int total = nx*ny*nz+extra;
    if(total != 0){
      data = new T[total];
      allocateAndInitializeStride(xidx, nx, nz*ny);
      allocateAndInitializeStride(yidx, ny, nz);
      allocateAndInitializeStride(zidx, nz, 1);
    } else {
      xidx = yidx = zidx = 0;
      data = 0;
    }
  }

  void initialize(const T& value) {
    int total = nx*ny*nz+extra;
    for(int i=0;i<total;i++)
      data[i] = value;
  }

  T& operator()(int x, int y, int z) {
    return data[xidx[x] + yidx[y] + zidx[z]];
  }
  const T& operator()(int x, int y, int z) const {
    return data[xidx[x] + yidx[y] + zidx[z]];
  }
  int getIndex(int x, int y, int z) const {
    return xidx[x] + yidx[y] + zidx[z];
  }
  T& operator[](int idx) {
    return data[idx];
  }
  const T& operator[](int idx) const {
    return data[idx];
  }

  int getNx() const {
    return nx;
  }
  int getNy() const {
    return ny;
  }
  int getNz() const {
    return nz;
  }
  


 
private:
  int nx, ny, nz, extra;
  int* xidx;
  int* yidx;
  int* zidx;
  T* data;
  int *_refCount;

  void allocateAndInitializeStride(int*& idx, int num, int stride) {
    idx = new int[num];
    for(int i=0;i<num;i++)
      idx[i] = stride*i;
  }
};

#endif
