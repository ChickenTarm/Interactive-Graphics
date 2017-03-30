#ifndef IMG_VIEWER_H
#define IMG_VIEWER_H

#include <QMainWindow>
#include <QCheckBox>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QMenuBar>
#include <QAction>
#include <QDockWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QScrollArea>
#include "tiny_obj_loader.h"
#include "triangle.h"

// ":" is just like "extends" in Java
class ImageViewer : public QMainWindow {
  Q_OBJECT // no ; required.  Put this inside any Qt GUI class

public:
  // enums are a convenient way of defining constants
  // By default they map to 0, 1, 2, ..., unless you specify otherwise
  // (This is different from Java, where enums are not numeric.)
  //
  // RED, GREEN, BLUE are all powers of two so we can | the values
  // together to specify multiple color channels in a single int
  enum { BOX = 1, MEDIAN = 2, GUASSIAN = 4 };

  struct pixel_struct {
    unsigned char r, g, b;
  };

  typedef struct pixel_struct pixel_t;

  typedef struct img_struct {
    pixel_t *data;
    int w, h;
  } img_t;

  struct z_buffer {
      float *data;
      int w, h;
  };

  typedef struct pixel_double {
    double r, g, b;
  } dub_p;

  typedef struct between {
    dub_p *data;
    int w, h;
  } dub_i;

  // "explicit" means only use this constructor when you instantiate
  // an object, don't use it to cast from one type to another
  explicit ImageViewer(QWidget *parent = 0);

  // make destructors virtual if you ever want to
  // subclass this, or C++ will could up calling
  // the wrong destructor!
  virtual ~ImageViewer();

public slots:
  /* Signals/Slots are a Qt thing, not a C++ thing.  They only work with Qt
   * and Qt's build process, which includes a "meta-object compiler" that
   * injects messy C++ code into your class to implement them.
   *
   * The idea is that some classes can "signal" certain events (e.g. a
   * checkbox was just clicked) and include information about the even
   * (e.g. whether the box is now checked or unchecked).  Signals look
   * just like function calls, except the function itself is never
   * defined.  Other classes can provide "slots" to receive these
   * events.  These look just like regular class methods, except they
   * are defined under a "slots" label in the header.  Finally, anyone
   * else (e.g. your main function) can connect specific signals to
   * specific slots.  So, for example, your main function can decree
   * that when a certain checkbox is clicked, a slot in your main
   * window class should get called.  Qt keeps track of which slots
   * get called whenever a certain signal is emitted, and takes care
   * of making the calls.
   */

  void openObj();
  void openCamera();
  void saveAs();
  void render();
  void load();
  void apply();
  void changeCamera();
  void resetCam();
  void resetImage();

private:
  void updateLabel(void);
  void createMenu();
  void createFileActions();
  std::vector<triangle> toNDC(camera &c, std::vector<triangle> triangles);
  std::vector<triangle> toScreen(float width, float height, std::vector<triangle> triangles);
  void boundBox(triangle &t, int width, int height);
  std::vector<triangle> visibility(std::vector<triangle> triangles, int width, int height);
  void PixelCalc();
  void boxBlur();
  void medianBlur();
  void guassianBlur();
  void sobel();
  void grayScale();
  void flip();
  void flop();
  void transpose();
  void rotate(double cc_angle);
  unsigned char getMed(unsigned char *lst, int num);


  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::vector<triangle> triangles;
  float w;
  float h;
  int z_method;
  int color_method;
  img_t *renderedPic;
  img_t *modifiedPic;
  z_buffer *z_buf;
  FILE *outputTemp;
  camera camOrig;
  camera cam;
  bool camRead;
  bool firstRender;

  int channels;
  QWidget *main;
  QScrollArea *filterScroll;
  QDockWidget *filterPane;
  QWidget *filters;
  QCheckBox *boxBlurCheck, *medianCheck, *guassianCheck, *sobelCheck, *flipCheck, *flopCheck, *transposeCheck, *grayCheck;
  QSpinBox *blurBox;
  QDoubleSpinBox *stdBox;
  QDoubleSpinBox *imageAngleBox;
  QSpinBox *leftBox;
  QSpinBox *rightBox;
  QSpinBox *upBox;
  QSpinBox *downBox;
  QSpinBox *forwardBox;
  QSpinBox *backBox;
  QDoubleSpinBox *horizontalBox;
  QDoubleSpinBox *verticalBox;
  QDoubleSpinBox *horizontalFOVBox;
  QDoubleSpinBox *verticalFOVBox;
  QDoubleSpinBox *drawBox;
  QLabel *imgShower;
  QImage image;
  QPixmap img;
  QMenuBar *menu;
  QMenu *fileMenu;
  QPushButton *applyBut;
  QPushButton *cameraChangeBut;
  QPushButton *resetCameraBut;
  QPushButton *resetImageBut;
  QAction *openOBJAct;
  QAction *openCameraAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *renderAct;
  QAction *loadImageAct;
  QString objName;
  QString cameraName;
  QString filePath;
};

#endif /* IMG_VIEWER_H */
