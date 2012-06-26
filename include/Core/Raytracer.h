
class UI;
class Scene;

class Raytracer {
public:
  Raytracer(UI *ui, Scene *scene) :
    _ui(ui), _scene(scene),
    _xstart(-1), _ystart(-1), _xend(-1), _yend(-1),
    _xres(-1), _yres(-1) {}

  void setFullWindowSize( int xres, int yres ) {
    _xres = xres;
    _yres = yres;
    _xstart = 0; _ystart = 0;
    _xend = xres; _yend = yres;
  }
  void setWindowRectangle( int xstart, int ystart,
			   int xend, int yend ) {
    _xstart = xstart;
    _ystart = ystart;
    _xend = xend;
    _yend = yend;
  }
  
  void trace();
  void scene(Scene *scene) { _scene = scene; }
private:
  UI *_ui;
  Scene *_scene;
  int _xstart, _ystart, _xend, _yend;
  int _xres, _yres;

  Raytracer();
};
