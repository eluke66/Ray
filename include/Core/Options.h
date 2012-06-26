#ifndef _Options_h_
#define _Options_h_

#include <vector>
#include <string>

class Filter2D;

class Options {
public:
  static Options *Instance();
  static void Initialize( int argc, char ** argv );

  class Slave {
  public:
    std::string machine;
    std::string path;
  };
  typedef std::vector<Slave> SlaveList;

  class Rectangle {
  public:
    Rectangle() :
      _startX(0), _startY(0), _endX(0), _endY(0) {}
    Rectangle(unsigned xs, unsigned ys, unsigned xe, unsigned ye ) :
      _startX(xs), _startY(ys), _endX(xe), _endY(ye) {}
    bool set() const {
      return _startX != 0 || _startY != 0 ||
	_endX != 0 || _endY != 0;
    }
    unsigned int _startX;
    unsigned int _startY;
    unsigned int _endX;
    unsigned int _endY;
  };
  
  const SlaveList & slaves() const { return _slaves; }
  void slaves( SlaveList &slaves ) { _slaves = slaves; }

  const Rectangle & windowRectangle() const { return _rectangle; }
  void windowRectangle( const Rectangle & rectangle )
  { _rectangle = rectangle; }

  unsigned int sizeX() const { return _sizeX; }
  void sizeX(unsigned int value) { _sizeX = value; }
  unsigned int sizeY() const { return _sizeY; }
  void sizeY(unsigned int value) { _sizeY = value; }
  
  bool isSlave() const { return _isSlave; }
  void isSlave( bool value ) { _isSlave = value; }

  unsigned int randomSeed() const { return _randomSeed; }
  void randomSeed( unsigned int value ) { _randomSeed = value; }
  
  std::string uiType() const { return _uiType; }
  void uiType( std::string ui ) { _uiType = ui; }

  unsigned int numThreads() const { return _numThreads; }
  void numThreads( unsigned int num ) { _numThreads = num; }

  std::string scene() const { return _scene; }
  void scene( std::string value ) { _scene = value; }

  std::string outputFilename() const { return _outputFilename; }
  void outputFilename( std::string value ) { _outputFilename = value; }

  std::string restartFilename() const { return _restartFilename; }
  void restartFilename( std::string value ) { _restartFilename = value; }

  bool quiet() const { return _quiet; }
  void quiet( bool value ) { _quiet = value; }
  
  void printList(bool print) { _printList = true; }
  bool printList() { return _printList; }

  int numSamples() const { return _numSamples; }
  bool samplesSet() const { return _numSamples > 0; }
  std::string sampleType() { return _sampleType; }
  Filter2D* filter() { return _filter; }
  std::string filterType() { return _filterType; }

  std::string sceneFile() const { return _sceneFile; }
  void sceneFile( std::string value ) { _sceneFile = value; }

  bool interactive() const { return _interactive; }
  void interactive( bool value ) { _interactive = value; }

  int numFrames() const { return _numFrames; }
  void numFrames( int value ) { _numFrames = value; }
  
  unsigned numPhotons() const { return _numPhotons; }
  void     numPhotons(unsigned value) { _numPhotons = value; }

  double   estimateRadius() const { return _estimateRadius; }
  void     estimateRadius( double value ) { _estimateRadius = value; }

  unsigned photonsInEstimate() const { return _photonsInEstimate; }
  void     photonsInEstimate(unsigned value) { _photonsInEstimate = value; }

  unsigned numCausticPhotons() const { return _numCausticPhotons; }
  void     numCausticPhotons(unsigned value) { _numCausticPhotons = value; }

  double   causticEstimateRadius() const { return _causticEstimateRadius; }
  void     causticEstimateRadius( double value ) { _causticEstimateRadius = value; }

  unsigned photonsInCausticEstimate() const { return _photonsInCausticEstimate; }
  void     photonsInCausticEstimate(unsigned value) { _photonsInCausticEstimate = value; }
  
private:
  SlaveList    _slaves;
  bool         _isSlave;
  unsigned int _randomSeed;
  unsigned int _sizeX, _sizeY;
  Rectangle    _rectangle;
  std::string  _uiType;
  unsigned int _numThreads;
  std::string  _scene;
  std::string  _outputFilename;
  std::string  _restartFilename;
  bool         _printList;
  bool         _quiet;
  int          _numSamples;
  std::string  _sampleType;
  Filter2D *   _filter;
  std::string  _filterType;
  std::string  _sceneFile;
  bool         _interactive;
  int          _numFrames;

  // Global illumination parameters
  unsigned     _numPhotons;
  unsigned     _photonsInEstimate;
  double       _estimateRadius;
  unsigned     _numCausticPhotons;
  unsigned     _photonsInCausticEstimate;
  double       _causticEstimateRadius;
  static Options *_instance;
  
  Options();
  static void Usage(std::string program);
};

#endif
